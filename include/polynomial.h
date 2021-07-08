#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "matchit.h"
#include "mathiu.h"
#include "diff.h"

namespace mathiu
{
    namespace impl
    {
        inline int32_t degreeMonomial(ExprPtr const& monomial, Set const& varSet)
        {
            Id<Product> iP;
            Id<ExprPtr> base, exp;
            return match(*monomial)
            (
                pattern | as<Product>(iP) = [&]
                {
                    return std::accumulate(varSet.begin(), varSet.end(), 0, [&](int32_t sum, auto&& e) 
                    {
                        auto const exp = baseAndExp(*(*iP).at(e)).second;
                        return sum + std::get<int32_t>(*exp);
                    });
                },
                pattern | asBaseAndExp(base, exp) = [&]
                {
                    auto const iter = varSet.find(*base);
                    if (iter== varSet.end())
                    {
                        return 0;
                    }
                    return std::get<int32_t>(**exp);
                }
            );
        }

        inline int32_t degree(ExprPtr const& ex, ExprPtr const& varSet)
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
                    return std::accumulate((*iS).begin(), (*iS).end(), 0, [&](int32_t sum, auto&& e) 
                    {
                        return std::max(sum, degreeMonomial(e.second, varSet_));
                    });
                },
                pattern | _ = [&] { return degreeMonomial(ex, varSet_);}
            );
        }

        // Fix me, check monomial first
        inline ExprPtr coefficientMonomial(ExprPtr const& monomial, ExprPtr const& x, int32_t i)
        {
#if DEBUG
            std::cout << "coefficientMonomial: " << toString(monomial) << ",\t" << toString(x) << ",\t" << i << std::endl;
#endif // DEBUG

            Id<Product> iP;
            Id<ExprPtr> base, exp;
            auto const xi = x^integer(i);
            return match(monomial)
            (
                pattern | some(as<Product>(iP)) = [&]
                {
                    auto const iter = (*iP).find(x);
                    if (iter == (*iP).end() || !equal((*iter).second, xi))
                    {
                        return integer(0);
                    }
                    return monomial / xi;
                },
                pattern | xi = [&]
                {
                    return integer(1);
                },
                pattern | _ = expr(0_i)
            );
        }

        inline ExprPtr coefficient(ExprPtr const& u, ExprPtr const& x, int32_t i)
        {
#if DEBUG
            std::cout << "coefficient: " << toString(u) << ",\t" << toString(x) << ",\t" << i << std::endl;
#endif // DEBUG

            using namespace matchit;
            Id<Sum> iS;
            return match(*u)(
                pattern | as<Sum>(iS) = [&]
                {
                    return std::accumulate((*iS).begin(), (*iS).end(), 0_i, [&](ExprPtr sum, auto&& e) 
                    {
                        return sum + coefficientMonomial(e.second, x, i);
                    });
                },
                pattern | _ = [&] { return coefficientMonomial(u, x, i);}
            );
        }

    } // namespace impl
} // namespace mathiu

#endif // POLYNOMIAL_H