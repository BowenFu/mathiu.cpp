#include "matchit.h"
#include "mathiu/core.h"
#include "mathiu/solve.h"
#include "mathiu/diff.h"
#include "mathiu/inequation.h"
#include "mathiu/setOp.h"
#include "mathiu/polynomial.h"
#include "mathiu/function_range.h"
#include <numeric>

namespace mathiu::impl
{
    // implement limit

    using namespace matchit;

    Interval functionRangeImplIntervalDomain(ExprPtr const &function, ExprPtr const &symbol, ExprPtr const &domain)
    {
#if DEBUG
        std::cout << "functionRangeImplIntervalDomain: " << toString(function) << ",\t" << toString(symbol) << ",\t" << toString(domain) << std::endl;
#endif // DEBUG

        auto const domain_ = std::get<Interval>(*domain);
        auto const subs = [&](ExprPtr const &dst)
        { return substitute(function, symbol >> dst); };
        auto const firstP = IntervalEnd{subs(domain_.first.first), domain_.first.second};
        auto const secondP = IntervalEnd{subs(domain_.second.first), domain_.second.second};
        auto const derivative = diff(function, symbol);
        auto const criticalPoints = solve(derivative, symbol, domain);
        auto lessThan = [](auto&& l, auto&& r)
        {
            auto e = expand(l - r);
            return evald(e) < 0;
        };
        auto const lLTr = lessThan(firstP.first, secondP.first);
        auto min_ = lLTr ? firstP : secondP;
        auto max_ = lLTr ? secondP : firstP;
        // optimize me
        for (auto const &e : std::get<Set>(*criticalPoints))
        {
            auto const v = IntervalEnd{subs(e), true};
            min_ = lessThan(min_.first, v.first) ? min_ : v;
            max_ = lessThan(v.first, max_.first) ? max_ : v;
        }
        auto result = Interval{min_, max_};
#if DEBUG
        std::cout << "functionRangeImplIntervalDomain: " << toString(function) << ",\t" << toString(symbol) << ",\t" << toString(domain) << ",\t resutl: " << toString(makeSharedExprPtr(result)) << std::endl;
#endif // DEBUG

        return result;
    }

    ExprPtr functionRangeImpl(ExprPtr const &function, ExprPtr const &symbol, ExprPtr const &domain)
    {
#if DEBUG
        std::cout << "functionRangeImpl: " << toString(function) << ",\t" << toString(symbol) << ",\t" << toString(domain) << std::endl;
#endif // DEBUG

        Id<Union> iUnion;
        auto result = match(*domain)(
            pattern | as<Interval>(_) = [&]
            { return makeSharedExprPtr(functionRangeImplIntervalDomain(function, symbol, domain)); },
            pattern | as<SetOp>(as<Union>(iUnion)) = [&]
            {
                ExprPtr result = false_;
                for (auto &&e : *iUnion)
                {
                    result = union_(result, functionRangeImpl(function, symbol, e));
                }
                return result;
            },
            pattern | _ = [&]
            {
                throw std::logic_error{"Mismatch in functionRangeImpl!"};
                return false_;
            });
#if DEBUG
        std::cout << "functionRangeImpl: " << toString(function) << ",\t" << toString(symbol) << ",\t" << toString(domain) << ",\tresult: " << toString(result) << std::endl;
#endif // DEBUG

        return result;
    }

    ExprPtr functionRange(ExprPtr const &function, ExprPtr const &symbol, ExprPtr const &domain)
    {
#if DEBUG
        std::cout << "functionRange: " << toString(function) << ",\t" << toString(symbol) << ",\t" << toString(domain) << std::endl;
#endif // DEBUG

        Id<PieceWise> iPieceWise;
        auto result = match(*function)(
            pattern | as<PieceWise>(iPieceWise) = [&]
            {
                return std::accumulate((*iPieceWise).begin(), (*iPieceWise).end(), false_, [&](auto &&result, auto &&e)
                                       {
                                           auto const newDomain = intersect(solveInequation(e.second, symbol), domain);
                                           if (equal(newDomain, false_))
                                           {
                                               return result;
                                           }
                                           return union_(result, functionRange(e.first, symbol, newDomain));
                                       });
            },
            pattern | _ = [&]
            { return functionRangeImpl(function, symbol, domain); });
#if DEBUG
        std::cout << "functionRange: " << toString(function) << ",\t" << toString(symbol) << ",\t" << toString(domain) << ",\tresult: " << toString(result) << std::endl;
#endif // DEBUG

        return result;
    }
} // namespace mathiu::impl
