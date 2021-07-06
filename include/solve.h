#ifndef SOLVE_H
#define SOLVE_H

#include "matchit.h"
#include "mathiu.h"
#include "diff.h"

namespace mathiu
{
    namespace impl
    {

        inline ExprPtr solve(ExprPtr const& lhs, ExprPtr const& rhs, ExprPtr const& var)
        {
#if DEBUG
            std::cout << "solveImpl: " << toString(lhs) << "\t == \t" << toString(rhs) << ",\t" << toString(var) << std::endl;
#endif // DEBUG

            auto const var_ = std::get<Symbol>(*var);
            using namespace matchit;
            const auto freeOfVar = app([&](auto&& e) { return diffImpl(e, var_); }, integer(0));
            return match(lhs, rhs)(
                pattern | ds(some(as<Symbol>(var_)), freeOfVar) = expr(set({rhs})),
                pattern | ds(freeOfVar, some(as<Symbol>(var_))) = expr(set({lhs})),
                pattern | ds(freeOfVar, freeOfVar) = [&]{ return equal(lhs, rhs) ? set({var}) : set({}); },
                pattern | _ = [&]
                {
                    throw std::runtime_error{"No match in solve!"};
                    return set({});
                });
        }

    } // namespace impl
    using impl::solve;
} // namespace mathiu

#endif // SOLVE_H