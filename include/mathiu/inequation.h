#ifndef INEQUATION_H
#define INEQUATION_H

#include "matchit.h"
#include "core.h"
#include "diff.h"
#include "polynomial.h"

namespace mathiu
{
    namespace impl
    {
        ExprPtr solveInequation(ExprPtr const& ex, ExprPtr const& var, ExprPtr const& domain = complexes);
    } // namespace impl
} // namespace mathiu

#endif // INEQUATION_H