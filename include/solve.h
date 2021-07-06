#ifndef SOLVE_H
#define SOLVE_H

#include "matchit.h"
#include "mathiu.h"
#include "diff.h"

namespace mathiu
{
    namespace impl
    {
        // TODO: solve poly
        inline ExprPtr solve(ExprPtr const& ex, ExprPtr const& var)
        {
#if DEBUG
            std::cout << "solve: " << toString(ex) << ",\t" << toString(var) << std::endl;
#endif // DEBUG

            auto const var_ = std::get<Symbol>(*var);
            using namespace matchit;
            const auto freeOfVar = app([&](auto&& e) { return diffImpl(e, var_); }, integer(0));
            Id<Product> iP;
            return match(ex)(
                pattern | some(as<int32_t>(0)) = expr(set({var})),
                pattern | freeOfVar = expr(set({})),
                pattern | var = expr(set({integer(0)})), // todo -> solve poly
                pattern | some(as<Product>(iP)) = [&]
                {
                    auto solutionSet = std::accumulate((*iP).begin(), (*iP).end(), Set{}, [&](Set solutions, auto&& e) 
                    {
                        solutions.merge(const_cast<Set&>(std::get<Set>(*solve(e.second, var)))); // it is safe to const_cast a temp.
                        return solutions;
                    });
                    return std::make_shared<Expr const>(std::move(solutionSet));
                },
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