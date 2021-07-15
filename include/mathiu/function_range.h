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

        inline Interval functionRangeImpl(ExprPtr const& function, ExprPtr const& symbol, ExprPtr const& domain)
        {
            auto const domain_ = std::get<Interval>(*domain);
            auto const subs = [&] (ExprPtr const& dst) { return substitute(function, symbol >> dst); };
            auto const firstP = subs(domain_.first.first);
            auto const secondP = subs(domain_.second.first);
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
            return Interval{IntervalEnd{min_, true}, IntervalEnd{max_, true}};
        }

        inline Interval union_(Interval const& lhs, Interval const& rhs)
        {
            if (lhs.first.first == nullptr && lhs.second.first == nullptr)
            {
                return rhs;
            }
            assert(evald(lhs.second.first) >= evald(rhs.first.first));
            return Interval{lhs.first, rhs.second};
        }

        inline ExprPtr functionRange(ExprPtr const& function, ExprPtr const& symbol, ExprPtr const& domain)
        {
            Id<PieceWise> iPieceWise;
            auto result = match(*function)(
                pattern | as<PieceWise>(iPieceWise)   = [&] {
                    return std::accumulate((*iPieceWise).begin(), (*iPieceWise).end(), Interval{}, [&] (auto&& result, auto&& e)
                    {
                        return union_(result, functionRangeImpl(e.first, symbol, intersect(solveInequation(e.second, symbol), domain)));
                    });
                },
                pattern | _ = [&]
                { return functionRangeImpl(function, symbol, domain); });
            return std::make_shared<Expr const>(std::move(result));
        }
    } // namespace impl
} // namespace mathiu

#endif // FUNCTION_RANGE_H