#ifndef SOLVE_H
#define SOLVE_H

#include "matchit.h"
#include "core.h"
#include "diff.h"
#include "polynomial.h"

namespace mathiu
{
    namespace impl
    {
        inline ExprPtr solvePoly(ExprPtr const& ex, ExprPtr const& var)
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
                    auto sqrtB2_4ac = sqrt(((*b)^2_i) - 4_i * (*a) * (*c));
                    return set((-(*b) + sqrtB2_4ac) / _2a, (-(*b) - sqrtB2_4ac) / _2a);
                },
                pattern | ds(c, b) = [&] { return set(-(*c) / (*b)); },
                pattern | ds(c) = [&] { return equal(*c, 0_i) ? set(var) : set(); },
                pattern | _ = [&]
                {
                    throw std::runtime_error{"No match in solve!"};
                    return set();
                });
        }

        inline ExprPtr intersect(ExprPtr const& lhs, ExprPtr const& rhs)
        {
            (void)rhs;
            return lhs;
            // Id<CCInterval> iCCInterval;
            // return match(*lhs, *rhs)
            // (
            // );
        }

        inline ExprPtr solve(ExprPtr const& ex, ExprPtr const& var, ExprPtr const& domain = complexes)
        {
#if DEBUG
            std::cout << "solve: " << toString(ex) << ",\t" << toString(var) << ",\t" << toString(domain) << std::endl;
#endif // DEBUG

            auto const var_ = std::get<Symbol>(*var);
            using namespace matchit;
            const auto freeOf = [](auto&& e, auto&& v) { return equal(diff(e, v), 0_i); };
            const auto freeOfVar = meet([&](auto&& e) { return freeOf(e, var); });
            Id<Product> iP;
            Id<ExprPtr> iE1, iE2;
            return match(ex)(
                pattern | some(as<Relational>(as<Equal>(ds(iE1, iE2)))) = [&] { return solve(expand(*iE1 - *iE2), var, domain); },
                pattern | some(as<Integer>(0)) = expr(set(var)),
                pattern | freeOfVar = expr(set()),
                pattern | var = expr(set(0_i)), // todo -> solve poly
                pattern | some(as<Product>(iP)) = [&]
                {
                    auto solutionSet = std::accumulate((*iP).begin(), (*iP).end(), Set{}, [&](Set solutions, auto&& e) 
                    {
                        solutions.merge(const_cast<Set&>(std::get<Set>(*solve(e.second, var, domain)))); // it is safe to const_cast a temp.
                        return solutions;
                    });
                    return std::make_shared<Expr const>(std::move(solutionSet));
                },
                // assume is poly
                pattern | _ = [&]
                {
                    auto const sol = solvePoly(ex, var);
                    return intersect(sol, domain);
                });
        }

    } // namespace impl
    using impl::solve;
} // namespace mathiu

#endif // SOLVE_H