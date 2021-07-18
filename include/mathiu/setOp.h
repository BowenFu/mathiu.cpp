#ifndef SET_OP_H
#define SET_OP_H

#include "matchit.h"
#include "core.h"
#include "diff.h"
#include "polynomial.h"

namespace mathiu
{
    namespace impl
    {
        ExprPtr intersect(ExprPtr const& lhs, ExprPtr const& rhs);

        ExprPtr union_(ExprPtr const &lhs, ExprPtr const &rhs);

        inline auto const realInterval = [](auto &&left, auto &&right)
        { return ds(and_(ds(asDouble, _), left), and_(ds(asDouble, _), right)); };

        ExprPtr intersectInterval(Interval const& lhs, Interval const& rhs);

        bool inInterval(ExprPtr const& e, Interval const& interval);

    } // namespace impl
} // namespace mathiu

#endif // SET_OP_H