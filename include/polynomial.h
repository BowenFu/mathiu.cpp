#ifndef SOLVE_H
#define SOLVE_H

#include "matchit.h"
#include "mathiu.h"
#include "diff.h"

namespace mathiu
{
    namespace impl
    {
        inline int32_t degreeMonomial(ExprPtr const& ex, ExprPtr const& varSet)
        {
#if DEBUG
            std::cout << "degreeMonomial: " << toString(ex) << ",\t" << toString(var) << std::endl;
#endif // DEBUG

            auto const varSet_ = std::get<Set>(*varSet);
            using namespace matchit;
            Id<Product> iP;
            return match(*ex)(
                pattern | as<Product>(iP) = [&]
                {
                    return std::reduce(varSet_.begin(), varSet_.end(), 0, [&](int32_t sum, auto&& e) 
                    {
                        auto coeff = baseAndExp((*iP).at(e)).first;
                        return sum + std::get<int32_t>(*coeff);
                    });
                },
                pattern | _ = [&]
                {
                    throw std::runtime_error{"No match in solve!"};
                    return 0;
                });
        }

    } // namespace impl
    using impl::solve;
} // namespace mathiu

#endif // SOLVE_H