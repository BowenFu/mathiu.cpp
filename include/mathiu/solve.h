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

        inline auto const asDouble = meet([](auto&& e)
        {
            try{
                evald(e);
                return true;
            }
            catch (...)
            {
                return false;
            }
        });
        inline auto const realInterval = [](auto &&left, auto &&right)
        { return ds(left.at(ds(asDouble, _)), right.at(ds(asDouble, _))); };
        inline Interval intersectInterval(Interval const& lhs, Interval const& rhs)
        {
            Id<IntervalEnd> iIEL1, iIER1, iIEL2, iIER2;
            return match(lhs, rhs)
            (
                pattern | ds(realInterval(iIEL1, iIER1), realInterval(iIEL2, iIER2)) = [&]
                {
                    auto const left = evald((*iIEL1).first) > evald((*iIEL2).first) ? *iIEL1 : *iIEL2;
                    auto const right = evald((*iIER1).first) < evald((*iIER2).first) ? *iIER1 : *iIER2;
                    assert(evald((*iIEL1).first) <= evald((*iIER2).first) && evald((*iIEL2).first) <= evald((*iIER1).first));
                    return Interval{left, right};
                },
                pattern | _ = [&]
                {
                    throw std::logic_error{"Mismatch!"};
                    return lhs;
                }
            );
        }

        inline bool inInterval(ExprPtr const& e, Interval const& interval)
        {
            Id<IntervalEnd> iIE1, iIE2;
            return match(interval)(
                pattern | ds(ds(e, _), _) = [&]
                { return interval.first.second == true; },
                pattern | ds(_, ds(e, _)) = [&]
                { return interval.second.second == true; },
                pattern | realInterval(iIE1, iIE2) = [&]
                {
                    auto const v = evald(e);
                    auto const left = evald((*iIE1).first);
                    auto const right = evald((*iIE2).first);
                    if (left < v && v < right)
                    {
                        return true;
                    }
                    if (left == v && (*iIE1).second)
                    {
                        return true;
                    }
                    if (right == v && (*iIE2).second)
                    {
                        return true;
                    }
                    return false;
                },
                pattern | _ = [&]
                {
                    throw std::logic_error{"Mismatch in inInterval!"};
                    return false;
                });
        }

        inline ExprPtr intersect(ExprPtr const& lhs, ExprPtr const& rhs)
        {
#if DEBUG
            std::cout << "intersect: " << toString(lhs) << ",\t" << toString(rhs) << std::endl;
#endif // DEBUG

            Id<Interval> iInterval1, iInterval2;
            Id<IntervalEnd> iIE1, iIE2;
            Id<Set> iSet;
            return match(*lhs, *rhs)
            (
                pattern | ds(as<Interval>(iInterval1), as<Interval>(iInterval2)) = [&] {
                    return std::make_shared<Expr const>(intersectInterval(*iInterval1, *iInterval2));
                },
                pattern | ds(as<Set>(iSet), as<Interval>(iInterval2)) = [&] {
                    Set result;
                    for (auto&& e: *iSet)
                    {
                        if (inInterval(e, *iInterval2))
                        {
                            result.insert(e);
                        }
                    }
                    return std::make_shared<Expr const>(std::move(result));
                },
                pattern | ds(as<Set>(_), _) = [&] {
                    return lhs; // FIXME
                },
                pattern | _ = [&] {
                    throw std::logic_error{"Mismatch in intersect!"};
                    return false_;
                }
            );
        }

        inline ExprPtr solve(ExprPtr const& ex, ExprPtr const& var, ExprPtr const& domain = complexes)
        {
#if DEBUG
            std::cout << "solve: " << toString(ex) << ",\t" << toString(var) << ",\t" << toString(domain) << std::endl;
#endif // DEBUG

            auto const var_ = std::get<Symbol>(*var);
            using namespace matchit;
            const auto freeOfVar = meet([&](auto&& e) { return freeOf(e, var); });
            Id<Product> iP;
            Id<ExprPtr> iE1, iE2;
            return match(ex)(
                pattern | some(as<Relational>(ds(RelationalKind::kEQUAL, iE1, iE2))) = [&] { return solve(expand(*iE1 - *iE2), var, domain); },
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