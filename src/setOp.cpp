#include <iterator>
#include "matchit.h"
#include "mathiu/core.h"
#include "mathiu/setOp.h"

namespace mathiu::impl
{
    using namespace matchit;

    inline auto const realInterval = [](auto &&left, auto &&right)
    { return ds(and_(ds(asDouble, _), left), and_(ds(asDouble, _), right)); };

    ExprPtr intersectInterval(Interval const &lhs, Interval const &rhs)
    {
        Id<IntervalEnd> iIEL1, iIER1, iIEL2, iIER2;
        return match(lhs, rhs)(
            pattern | ds(realInterval(iIEL1, iIER1), realInterval(iIEL2, iIER2)) = [&]
            {
                auto const left = evald((*iIEL1).first) > evald((*iIEL2).first) ? *iIEL1 : *iIEL2;
                auto const right = evald((*iIER1).first) < evald((*iIER2).first) ? *iIER1 : *iIER2;
                if (evald((*iIEL1).first) <= evald((*iIER2).first) && evald((*iIEL2).first) <= evald((*iIER1).first))
                {
                    return makeSharedExprPtr(Interval{left, right});
                }
                return false_;
            },
            pattern | _ = [&]
            {
                throw std::logic_error{"Mismatch!"};
                return makeSharedExprPtr(lhs);
            });
    }

    bool inInterval(ExprPtr const &e, Interval const &interval)
    {
#if DEBUG
        std::cout << "inInterval: " << toString(e) << ",\t" << toString(makeSharedExprPtr(interval)) << std::endl;
#endif // DEBUG

        Id<IntervalEnd> iIE1, iIE2;
        auto result = match(interval)(
            pattern | ds(ds(e, _), _) = [&]
            { return interval.first.second == true; },
            pattern | ds(_, ds(e, _)) = [&]
            { return interval.second.second == true; },
            pattern | realInterval(iIE1, iIE2) = [&]
            {
                auto const v = evald(e);
                auto const left = evald((*iIE1).first);
                auto const right = evald((*iIE2).first);
                if (left < v && v < right)
                {
                    return true;
                }
                if (left == v && (*iIE1).second)
                {
                    return true;
                }
                if (right == v && (*iIE2).second)
                {
                    return true;
                }
                return false;
            },
            pattern | _ = [&]
            {
                throw std::logic_error{"Mismatch in inInterval!"};
                return false;
            });
#if DEBUG
        std::cout << "inInterval: " << toString(e) << ",\t" << toString(makeSharedExprPtr(interval)) << ", result: " << result << std::endl;
#endif // DEBUG
        return result;
    }

    ExprPtr intersect(ExprPtr const &lhs, ExprPtr const &rhs)
    {
#if DEBUG
        std::cout << "intersect: " << toString(lhs) << ",\t" << toString(rhs) << std::endl;
#endif // DEBUG

        Id<Interval> iInterval1, iInterval2;
        Id<IntervalEnd> iIE1, iIE2;
        Id<Set> iSet1, iSet2;
        Id<Union> iUnion;
        auto result = match(lhs, rhs)(
            pattern | ds(some(as<Interval>(iInterval1)), some(as<Interval>(iInterval2))) = [&]
            { return intersectInterval(*iInterval1, *iInterval2); },
            pattern | ds(some(as<Set>(iSet1)), some(as<Interval>(iInterval2))) = [&]
            {
                Set result;
                for (auto &&e : *iSet1)
                {
                    if (inInterval(e, *iInterval2))
                    {
                        result.insert(e);
                    }
                }
                return makeSharedExprPtr(std::move(result));
            },
            pattern | ds(some(as<Set>(_)), some(as<True>(_))) = [&]
            { return lhs; },
            pattern | ds(some(as<Set>(iSet1)), some(as<Set>(iSet2))) = [&]
            {
                Set result;
                auto const lessLambda = [](auto &&e1, auto &&e2)
                { return less(e1, e2); };
                std::set_intersection((*iSet1).begin(), (*iSet1).end(),
                                      (*iSet2).begin(), (*iSet2).end(), std::inserter(result, result.end()), lessLambda);
                return makeSharedExprPtr(std::move(result));
            },
            pattern | ds(some(as<Set>(_)), _) = [&]
            {
                throw std::logic_error{"Mismatch in intersect!"};
                return lhs; // FIXME
            },
            pattern | ds(_, some(as<SetOp>(as<Union>(iUnion)))) = [&]
            { return intersect(rhs, lhs); },
            pattern | ds(some(as<SetOp>(as<Union>(iUnion))), _) = [&]
            {
                Union result;
                for (auto &&e : *iUnion)
                {
                    auto ret = intersect(e, rhs);
                    if (!equal(ret, false_))
                    {
                        result.insert(ret);
                    }
                }
                return makeSharedExprPtr(SetOp{std::move(result)});
            },
            pattern | ds(false_, _) = expr(false_), pattern | ds(_, false_) = expr(false_), pattern | ds(true_, _) = expr(rhs), pattern | ds(_, true_) = expr(lhs), pattern | ds(true_, _) = expr(rhs), pattern | ds(_, true_) = expr(lhs), pattern | _ = [&]
                                                                                                                                                                                                                                                    {
                                                                                                                                                                                                                                                        throw std::logic_error{"Mismatch in intersect!"};
                                                                                                                                                                                                                                                        return false_;
                                                                                                                                                                                                                                                    });
#if DEBUG
        std::cout << "intersect: " << toString(lhs) << ",\t" << toString(rhs) << ",\tresult: " << toString(result) << std::endl;
#endif // DEBUG

        return result;
    }

    ExprPtr unionInterval(Interval const &lhs, Interval const &rhs)
    {
        assert(lhs.first.first != nullptr);
        assert(rhs.second.first != nullptr);

#if DEBUG
        std::cout << "unionInterval: " << toString(makeSharedExprPtr(lhs)) << ",\t" << toString(makeSharedExprPtr(rhs)) << std::endl;
#endif // DEBUG

        Id<IntervalEnd> iIEL1, iIER1, iIEL2, iIER2;
        return match(lhs, rhs)(
            pattern | ds(realInterval(iIEL1, iIER1), realInterval(iIEL2, iIER2)) = [&]
            {
                auto dL1 = evald((*iIEL1).first);
                auto dL2 = evald((*iIEL2).first);
                auto dR1 = evald((*iIER1).first);
                auto dR2 = evald((*iIER2).first);
                auto const left = dL1 <= dL2 ? *iIEL1 : *iIEL2;
                auto const right = dR1 >= dR2 ? *iIER1 : *iIER2;
                if (dL1 <= dR2 && dL2 <= dR1)
                {
                    return makeSharedExprPtr(Interval{left, right});
                }
                return makeSharedExprPtr(SetOp{Union{{makeSharedExprPtr(lhs), makeSharedExprPtr(rhs)}}});
            },
            pattern | _ = [&]
            {
                throw std::logic_error{"Mismatch!"};
                return makeSharedExprPtr(lhs);
            });
    }

    auto mergeU(Union const &c1, Union const &c2) -> Union
    {
        if (c1.size() < c2.size())
        {
            return mergeU(c2, c1);
        }
        Union result = c1;
        for (auto const &e : c2)
        {
            result.insert(e);
        }
        return result;
    }

    auto mergeUnion(Union const &c1, Union const &c2)
    {
        auto result = mergeU(c1, c2);
        if (result.size() == 1)
        {
            return (*result.begin());
        }
        return makeSharedExprPtr(SetOp{std::move(result)});
    }

    ExprPtr union_(ExprPtr const &lhs, ExprPtr const &rhs)
    {
#if DEBUG
        std::cout << "union_: " << toString(lhs) << ",\t" << toString(rhs) << std::endl;
#endif // DEBUG

        Id<Set> iSet1, iSet2;
        Id<Interval> iInterval1, iInterval2, iInterval3;
        Id<Union> iUnion1, iUnion2;
        return match(lhs, rhs)(
            pattern | ds(some(as<Set>(iSet1)), some(as<Set>(iSet2))) = [&]
            {
                Set solutions1 = *iSet1;
                Set solutions2 = *iSet2;
                solutions1.merge(solutions2);
                return makeSharedExprPtr(std::move(solutions1));
            },
            pattern | ds(some(as<Interval>(iInterval1)), some(as<Interval>(iInterval2))) = [&]
            { return unionInterval(*iInterval1, *iInterval2); },
            pattern | ds(false_, _) = expr(rhs), pattern | ds(_, false_) = expr(lhs), pattern | ds(some(as<SetOp>(as<Union>(iUnion1))), some(as<SetOp>(as<Union>(iUnion2)))) = [&]
                                                                                      { return mergeUnion(*iUnion1, *iUnion2); },
            pattern | ds(some(as<Interval>(iInterval1.at(realInterval(_, _)))), some(as<SetOp>(as<Union>(ds(some(as<Interval>(iInterval2.at(realInterval(_, _)))), some(as<Interval>(iInterval3.at(realInterval(_, _))))))))) = [&]
            {
                auto result = unionInterval(*iInterval1, *iInterval2);
                auto newI = std::get<Interval>(*result);
                return unionInterval(newI, *iInterval3);
            },
            pattern | ds(_, some(as<SetOp>(as<Union>(iUnion2)))) = [&]
            { return mergeUnion(Union{{lhs}}, *iUnion2); },
            pattern | ds(some(as<SetOp>(as<Union>(iUnion1))), _) = [&]
            { return mergeUnion(Union{{rhs}}, *iUnion2); },
            pattern | _ = [&]
            { return makeSharedExprPtr(SetOp{Union{{lhs, rhs}}}); });
    }

} // namespace mathiu::impl
