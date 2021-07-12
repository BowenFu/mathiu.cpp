#ifndef FUNCTION_RANGE_H
#define FUNCTION_RANGE_H

#include "matchit.h"
#include "core.h"
#include "solve.h"

namespace mathiu
{
    namespace impl
    {
        // implement limit

        inline CCInterval functionRangeImpl(ExprPtr const& function, ExprPtr const& symbol, ExprPtr const& domain)
        {
            auto const domain_ = std::get<CCInterval>(*domain);
            auto const subs = [&] (ExprPtr const& dst) { return substitute(function, symbol >> dst); };
            auto const firstP = subs(domain_.first);
            auto const secondP = subs(domain_.second);
            // TODO solve diff = 0 within the domain.
            auto const derivative = diff(function, symbol);
            auto const criticalPoints = solve(derivative, symbol, domain);
            auto min_ = min(firstP, secondP);
            auto max_ = max(firstP, secondP);
            for (auto const& e : std::get<Set>(*criticalPoints))
            {
                auto const v = subs(e);
                min_ = min(min_, v);
                max_ = max(max_, v);
            }
            return CCInterval{{min_, max_}};
        }

        inline CCInterval union_(CCInterval const& lhs, CCInterval const& rhs)
        {
            if (lhs.first == nullptr && lhs.second == nullptr)
            {
                return rhs;
            }
            assert(lhs.second >= rhs.first);
            return CCInterval{{lhs.first, rhs.second}};
        }

        inline CCInterval functionRange(ExprPtr const& function, ExprPtr const& symbol, ExprPtr const& domain)
        {
            Id<PieceWise> iPieceWise;
            return match(*function)(
                pattern | as<PieceWise>(iPieceWise)   = [&] {
                    return std::accumulate((*iPieceWise).begin(), (*iPieceWise).end(), CCInterval{}, [&] (auto&& result, auto&& e)
                    {
                        return union_(result, functionRangeImpl(e.first, symbol, intersect(domain, e.second)));
                    });
                },
                pattern | _ = [&]
                { return functionRangeImpl(function, symbol, domain); });
        }
    } // namespace impl
} // namespace mathiu

#endif // FUNCTION_RANGE_H