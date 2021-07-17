#ifndef DIFF_H
#define DIFF_H

#include "matchit.h"
#include "core.h"

namespace mathiu
{
    namespace impl
    {
        ExprPtr diff(ExprPtr const& exp, ExprPtr const& var);
    } // namespace impl
    using impl::diff;
} // namespace mathiu

#endif // DIFF_H