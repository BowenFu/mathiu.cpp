#ifndef INEQUATION_H
#define INEQUATION_H

#include "core.h"

namespace mathiu
{
    namespace impl
    {
        ExprPtr solveInequation(ExprPtr const& ex, ExprPtr const& var, ExprPtr const& domain = true_);
    } // namespace impl
} // namespace mathiu

#endif // INEQUATION_H