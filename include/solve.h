#ifndef SOLVE_H
#define SOLVE_H

#include "matchit.h"
#include "mathiu.h"
#include "diff.h"

namespace mathiu
{
    namespace impl
    {

        inline ExprPtr solveImpl(ExprPtr const& lhs, ExprPtr const& rhs, Symbol const& var)
        {
#if DEBUG
            std::cout << "solveImpl: " << toString(lhs) << "\t == \t" << toString(rhs) << ",\t" << var[0] << std::endl;
#endif // DEBUG

            using namespace matchit;
            const auto freeOfVar = app([&](auto&& e) { return diffImpl(e, var); }, integer(0));
            return match(lhs, rhs)(
                pattern | ds(some(as<Symbol>(var)), freeOfVar) = expr(rhs),
                pattern | ds(freeOfVar, some(as<Symbol>(var))) = expr(lhs),
                pattern | _ = [&]
                {
                    throw std::runtime_error{"No match in diff!"};
                    return integer(0);
                });
        }

        inline ExprPtr solve(ExprPtr const& lhs, ExprPtr const& rhs, ExprPtr const& var)
        {
            auto const var_ = std::get<Symbol>(*var);
            return solveImpl(lhs, rhs, var_);
        }
    } // namespace impl
    using impl::solve;
} // namespace mathiu

#endif // SOLVE_H