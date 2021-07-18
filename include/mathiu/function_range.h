#ifndef FUNCTION_RANGE_H
#define FUNCTION_RANGE_H

#include "core.h"

namespace mathiu
{
    namespace impl
    {
        // implement limit

        ExprPtr functionRange(ExprPtr const& function, ExprPtr const& symbol, ExprPtr const& domain);

        ExprPtr functionRange(ExprPtr const& function, ExprPtr const& domainSet);
        
    } // namespace impl
} // namespace mathiu

#endif // FUNCTION_RANGE_H