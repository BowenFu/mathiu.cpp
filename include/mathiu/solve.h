#ifndef SOLVE_H
#define SOLVE_H

#include "core.h"

namespace mathiu
{
    namespace impl
    {
        ExprPtr solvePoly(ExprPtr const& ex, ExprPtr const& var);

        ExprPtr solve(ExprPtr const& ex, ExprPtr const& var, ExprPtr const& domain = true_);

        ExprPtr solveInequation(ExprPtr const& ex, ExprPtr const& var, ExprPtr const& domain);

    } // namespace impl
    using impl::solve;
} // namespace mathiu

#endif // SOLVE_H