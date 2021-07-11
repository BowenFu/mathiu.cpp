#ifndef FUNCTION_RANGE_H
#define FUNCTION_RANGE_H

#include "matchit.h"
#include "core.h"
#include "solve.h"

namespace mathiu
{
    namespace impl
    {
        // implement limit

        inline CCInterval functionRange(ExprPtr const& function, ExprPtr const& symbol, ExprPtr const& domain)
        {
            auto const domain_ = std::get<CCInterval>(*domain);
            auto const subs = [&] (ExprPtr const& dst) { return substitute(function, symbol >> dst); };
            auto const firstP = subs(domain_.first);
            auto const secondP = subs(domain_.second);
            // TODO solve diff = 0 within the domain.
            auto const derivative = diff(function, symbol);
            auto const criticalPoints = solve(derivative, symbol, domain);
            auto min_ = min(firstP, secondP);
            auto max_ = max(firstP, secondP);
            for (auto const& e : std::get<Set>(*criticalPoints))
            {
                auto const v = subs(e);
                min_ = min(min_, v);
                max_ = max(max_, v);
            }
            return CCInterval{{min_, max_}};
        }
    } // namespace impl
} // namespace mathiu

#endif // FUNCTION_RANGE_H