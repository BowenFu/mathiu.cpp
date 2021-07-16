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
            auto const firstP = IntervalEnd{subs(domain_.first.first), domain_.first.second};
            auto const secondP = IntervalEnd{subs(domain_.second.first), domain_.second.second};
            auto const derivative = diff(function, symbol);
            auto const criticalPoints = solve(derivative, symbol, domain);
            auto min_ = evald(firstP.first) < evald(secondP.first) ? firstP : secondP;
            auto max_ = evald(firstP.first) < evald(secondP.first) ? secondP : firstP;
            // optimize me
            for (auto const& e : std::get<Set>(*criticalPoints))
            {
                auto const v = IntervalEnd{subs(e), true};
                min_ = evald(min_.first) < evald(v.first) ? min_ : v;
                max_ = evald(max_.first) > evald(v.first) ? max_ : v;
            }
            return Interval{min_, max_};
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