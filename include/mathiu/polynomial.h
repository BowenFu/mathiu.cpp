#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "matchit.h"
#include "core.h"
#include "diff.h"
#include <cmath>

namespace mathiu
{
    namespace impl
    {
        Integer degree(ExprPtr const& ex, ExprPtr const& varSet);

        ExprPtr coefficient(ExprPtr const& u, ExprPtr const& x, Integer i);

        ExprPtr coefficientList(ExprPtr const& u, ExprPtr const& x);

        ExprPtr expand(ExprPtr const& ex);
    } // namespace impl
} // namespace mathiu

#endif // POLYNOMIAL_H