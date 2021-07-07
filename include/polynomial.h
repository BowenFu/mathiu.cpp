#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "matchit.h"
#include "mathiu.h"
#include "diff.h"

namespace mathiu
{
    namespace impl
    {
        inline int32_t degreeMonomial(Product const& monomial, Set const& varSet)
        {
            return std::reduce(varSet.begin(), varSet.end(), 0, [&](int32_t sum, auto&& e) 
            {
                auto const exp = baseAndExp(*monomial.at(e)).second;
                return sum + std::get<int32_t>(*exp);
            });
        }

        inline int32_t degree(ExprPtr const& ex, ExprPtr const& varSet)
        {
#if DEBUG
            std::cout << "degree: " << toString(ex) << ",\t" << toString(varSet) << std::endl;
#endif // DEBUG

            using namespace matchit;
            Id<Set> iSet;
            Id<ExprPtr> base, exp;
            auto const varSet_ = match(*varSet)
            (
                pattern | as<Set>(iSet) = [&]
                {
                    return *iSet;
                },
                pattern | _ = [&] { return Set{{{varSet}}};}
            );

            Id<Sum> iS;
            Id<Product> iP;
            return match(*ex)(
                pattern | as<Sum>(iS) = [&]
                {
                    return std::reduce((*iS).begin(), (*iS).end(), 0, [&](int32_t sum, auto&& e) 
                    {
                        return sum + degree(e.second, varSet);
                    });
                },
                pattern | as<Product>(iP) = [&]
                {
                    return degreeMonomial(*iP, varSet_);
                },
                pattern | asBaseAndExp(base, exp) = [&]
                {
                    auto const iter = varSet_.find(*base);
                    if (iter== varSet_.end())
                    {
                        return 0;
                    }
                    return std::get<int32_t>(**exp);
                });
        }

    } // namespace impl
} // namespace mathiu

#endif // POLYNOMIAL_H