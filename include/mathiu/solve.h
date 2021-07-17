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

        inline auto const realInterval = [](auto &&left, auto &&right)
        { return ds(left.at(ds(asDouble, _)), right.at(ds(asDouble, _))); };

        ExprPtr intersectInterval(Interval const& lhs, Interval const& rhs);

        bool inInterval(ExprPtr const& e, Interval const& interval);

        ExprPtr intersect(ExprPtr const& lhs, ExprPtr const& rhs);

        ExprPtr solve(ExprPtr const& ex, ExprPtr const& var, ExprPtr const& domain = complexes);

    } // namespace impl
    using impl::solve;
} // namespace mathiu

#endif // SOLVE_H