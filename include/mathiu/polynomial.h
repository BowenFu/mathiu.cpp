#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "matchit.h"
#include "core.h"
#include "diff.h"

namespace mathiu
{
    namespace impl
    {
        inline Integer degreeMonomial(ExprPtr const& monomial, Set const& varSet)
        {
#if DEBUG
            std::cout << "degreeMonomial: " << toString(monomial) << ",\t" << std::endl;
#endif // DEBUG

            Id<Product> iP;
            Id<ExprPtr> base, exp;
            return match(*monomial)
            (
                pattern | as<Product>(iP) = [&]
                {
                    return std::accumulate(varSet.begin(), varSet.end(), 0, [&](Integer sum, auto&& e) 
                    {
                        // FIXME, iP does not contain e. instead a sum of e.
                        auto const exp = baseAndExp(*(*iP).at(e)).second;
                        return sum + std::get<Integer>(*exp);
                    });
                },
                pattern | asBaseAndExp(base, exp) = [&]
                {
                    auto const iter = varSet.find(*base);
                    if (iter== varSet.end())
                    {
                        return 0;
                    }
                    return std::get<Integer>(**exp);
                }
            );
        }

        inline Integer degree(ExprPtr const& ex, ExprPtr const& varSet)
        {
#if DEBUG
            std::cout << "degree: " << toString(ex) << ",\t" << toString(varSet) << std::endl;
#endif // DEBUG

            using namespace matchit;
            Id<Set> iSet;
            auto const varSet_ = match(*varSet)
            (
                pattern | as<Set>(iSet) = [&]
                {
                    return *iSet;
                },
                pattern | _ = [&] { return Set{{{varSet}}};}
            );

            Id<Sum> iS;
            return match(*ex)(
                pattern | as<Sum>(iS) = [&]
                {
                    return std::accumulate((*iS).begin(), (*iS).end(), 0, [&](Integer sum, auto&& e) 
                    {
                        return std::max(sum, degreeMonomial(e.second, varSet_));
                    });
                },
                pattern | _ = [&] { return degreeMonomial(ex, varSet_);}
            );
        }

        // Fix me, check monomial first
        inline std::pair<ExprPtr, Integer> coefficientMonomial(ExprPtr const& monomial, ExprPtr const& x)
        {
#if DEBUG
            std::cout << "coefficientMonomial: " << toString(monomial) << ",\t" << toString(x) << std::endl;
#endif // DEBUG

            const auto freeOfVar = meet([&](auto&& e) { return freeOf(e, x); });

            Id<Product> iP;
            Id<Integer> iiExp;
            return match(monomial)
            (
                pattern | x = [&]
                {
                    return std::make_pair(1_i, 1);
                },
                pattern | some(as<Power>(ds(x, some(as<Integer>(iiExp.at(_ > 1)))))) = [&]
                {
                    return std::make_pair(1_i, *iiExp);
                },
                pattern | some(as<Product>(iP)) = [&]
                {
                    return std::accumulate((*iP).begin(), (*iP).end(), std::make_pair(monomial, 0), [&](std::pair<ExprPtr, Integer> const& result, auto&& e) 
                    {
                        auto f = coefficientMonomial(e.second, x);
                        Integer const m = f.second;
                        if (m == 0)
                        {
                            return result;
                        }
                        return std::make_pair(monomial / (x ^ integer(m)), m);
                    });
                },
                pattern | freeOfVar = expr(std::make_pair(monomial, 0)),
                pattern | _ = [&] { throw std::runtime_error("Mismatch in coefficientMonomial!"); return std::make_pair(monomial, 0); }
            );
        }

        inline ExprPtr coefficient(ExprPtr const& u, ExprPtr const& x, Integer i)
        {
#if DEBUG
            std::cout << "coefficient: " << toString(u) << ",\t" << toString(x) << ",\t" << i << std::endl;
#endif // DEBUG

            using namespace matchit;
            Id<Sum> iS;
            return match(u)(
                pattern | x = [&] { return i == 1 ? 1_i : 0_i;  },
                pattern | some(as<Sum>(iS)) = [&]
                {
                    return std::accumulate((*iS).begin(), (*iS).end(), 0_i, [&](ExprPtr c, auto&& e) 
                    {
                        auto f = coefficientMonomial(e.second, x);
                        return (f.second == i) ? c + f.first : c;
                    });
                },
                pattern | _ = [&] {
                    auto f = coefficientMonomial(u, x);
                    return (f.second == i) ? f.first : 0_i;
                }
            );
        }

        inline ExprPtr coefficientList(ExprPtr const& u, ExprPtr const& x)
        {
            auto const deg = degree(u, x);
            List result;
            for (auto i = 0; i <= deg; ++i)
            {
                result.emplace_back(coefficient(u, x, i));
            }
            return std::make_shared<Expr const>(std::move(result));
        }


    } // namespace impl
} // namespace mathiu

#endif // POLYNOMIAL_H