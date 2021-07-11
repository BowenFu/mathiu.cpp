#ifndef FUNCTION_RANGE_H
#define FUNCTION_RANGE_H

#include "matchit.h"
#include "core.h"

namespace mathiu
{
    namespace impl
    {
        // implement limit

        inline CCInterval functionRange(ExprPtr const& function, ExprPtr const& symbol, CCInterval const& domain)
        {
            auto const subs = [&] (ExprPtr const& dst) { return substitute(function, symbol >> dst); };
            auto const firstP = subs(domain.first);
            auto const secondP = subs(domain.second);
            // TODO solve diff = 0 within the domain.
            return CCInterval{{min(firstP, secondP), max(firstP, secondP)}};
        }
    } // namespace impl
} // namespace mathiu

#endif // FUNCTION_RANGE_H