#include <iterator>
#include "matchit.h"
#include "mathiu/core.h"
#include "mathiu/diff.h"
#include "mathiu/polynomial.h"
#include "mathiu/setOp.h"
#include "mathiu/solve.h"
#include <numeric>

namespace mathiu::impl
{
    ExprPtr solvePoly(ExprPtr const &ex, ExprPtr const &var)
    {
#if DEBUG
        std::cout << "solvePoly: " << toString(ex) << ",\t" << toString(var) << std::endl;
#endif // DEBUG

        auto coeffList = coefficientList(ex, var);

        Id<ExprPtr> c, b, a;

        using namespace matchit;
        return match(std::get<List>(*coeffList))(
            pattern | ds(c, b, a) = [&]
            {
                auto _2a = 2_i * (*a);
                auto b2_4ac = ((*b) ^ 2_i) - 4_i * (*a) * (*c);
                auto sqrtB2_4ac = sqrt(b2_4ac);
                return match(b2_4ac)(
                    pattern | asDouble = [&]
                    {
                        auto d = evald(b2_4ac);
                        if (d > 0)
                        {
                            return set((-(*b) + sqrtB2_4ac) / _2a, (-(*b) - sqrtB2_4ac) / _2a);
                        }
                        else if (d == 0)
                        {
                            return set((-(*b) + sqrtB2_4ac) / _2a);
                        }
                        else
                        {
                            return false_;
                        }
                    },
                    pattern | _ = [&]
                    { return set((-(*b) + sqrtB2_4ac) / _2a, (-(*b) - sqrtB2_4ac) / _2a); });
            },
            pattern | ds(c, b) = [&]
            { return set(-(*c) / (*b)); },
            pattern | ds(c) = [&]
            { return equal(*c, 0_i) ? set(var) : set(); },
            pattern | _ = [&]
            {
                throw std::runtime_error{"No match in solve!"};
                return set();
            });
    }

    ExprPtr solve(ExprPtr const &ex, ExprPtr const &var, ExprPtr const &domain)
    {
#if DEBUG
        std::cout << "solve: " << toString(ex) << ",\t" << toString(var) << ",\t" << toString(domain) << std::endl;
#endif // DEBUG

        auto const var_ = std::get<Symbol>(*var);
        using namespace matchit;
        const auto freeOfVar = meet([&](auto &&e)
                                    { return freeOf(e, var); });
        Id<Product> iP;
        Id<ExprPtr> iE1, iE2;
        Id<PieceWise> iPieceWise;
        return match(ex)(
            pattern | some(as<Relational>(ds(RelationalKind::kEQUAL, iE1, iE2))) = [&]
            { return solve(expand(*iE1 - *iE2), var, domain); },
            pattern | some(as<Integer>(0)) = expr(set(var)),
            pattern | freeOfVar = expr(set()),
            pattern | some(as<Product>(iP)) = [&]
            {
                auto solutionSet = std::accumulate((*iP).begin(), (*iP).end(), Set{}, [&](Set solutions, auto &&e)
                                                   {
                                                       solutions.merge(const_cast<Set &>(std::get<Set>(*solve(e.second, var, domain)))); // it is safe to const_cast a temp.
                                                       return solutions;
                                                   });
                return makeSharedExprPtr(std::move(solutionSet));
            },
            pattern | some(as<PieceWise>(iPieceWise)) = [&]
            { return std::accumulate((*iPieceWise).begin(), (*iPieceWise).end(), false_, [&](auto &&result, auto &&e)
                                     { return union_(result, solve(e.first, var, solveInequation(e.second, var, domain))); }); },
            // assume is poly
            pattern | _ = [&]
            {
                auto const sol = solvePoly(ex, var);
                return intersect(sol, domain);
            });
    }

} // namespace mathiu::impl
