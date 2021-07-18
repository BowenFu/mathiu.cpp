#ifndef SOLVE_H
#define SOLVE_H

#include "matchit.h"
#include "core.h"
#include "diff.h"
#include "polynomial.h"

namespace mathiu
{
    namespace impl
    {
        ExprPtr solvePoly(ExprPtr const& ex, ExprPtr const& var);

        ExprPtr solve(ExprPtr const& ex, ExprPtr const& var, ExprPtr const& domain = complexes);

        ExprPtr solveInequation(ExprPtr const& ex, ExprPtr const& var, ExprPtr const& domain);

    } // namespace impl
    using impl::solve;
} // namespace mathiu

#endif // SOLVE_H