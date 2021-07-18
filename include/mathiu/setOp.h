#ifndef SET_OP_H
#define SET_OP_H

#include "core.h"

namespace mathiu
{
    namespace impl
    {
        ExprPtr intersect(ExprPtr const& lhs, ExprPtr const& rhs);

        ExprPtr union_(ExprPtr const &lhs, ExprPtr const &rhs);

        ExprPtr intersectInterval(Interval const& lhs, Interval const& rhs);

        bool inInterval(ExprPtr const& e, Interval const& interval);

    } // namespace impl
} // namespace mathiu

#endif // SET_OP_H