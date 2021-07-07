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
                        return sum + degreeMonomial(e.second, varSet_);
                    });
                },
                pattern | _ = [&] { return degreeMonomial(ex, varSet_);}
            );
        }

    } // namespace impl
} // namespace mathiu

#endif // POLYNOMIAL_H