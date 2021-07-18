#include "matchit.h"
#include "internal.h"
#include "mathiu/inequation.h"
#include "mathiu/diff.h"
#include "mathiu/solve.h"
#include "mathiu/polynomial.h"
#include "mathiu/setOp.h"
#include <numeric>

namespace mathiu
{
    namespace impl
    {
        ExprPtr solveInequationImpl(ExprPtr const& ex, RelationalKind relKind, ExprPtr const& var, ExprPtr const& domain)
        {
#if DEBUG
            std::cout << "solveInequationImpl: " << toString(ex) << ",\t" << toString(relKind)  << ",\t" << toString(var) << ",\t" << toString(domain) << std::endl;
#endif // DEBUG

            using namespace matchit;
            const auto freeOfVar = meet([&](auto&& e) { return freeOf(e, var); });
            Id<PieceWise> iPieceWise;
            auto result = match(ex)(
                pattern | freeOfVar = [&]{
                    throw std::logic_error("Cannot solve inequation!");
                    return relational(relKind, ex, 0_i);
                },
                pattern | some(as<PieceWise>(iPieceWise)) = [&]
                {
                    return std::accumulate((*iPieceWise).begin(), (*iPieceWise).end(), false_, [&](auto&& result, auto&& e)
                    {
                        return union_(result, solveInequationImpl(e.first, relKind, var, solveInequation(e.second, var, domain)));
                    });
                },
                pattern | _ = [&]
                {
                    auto coeffList = coefficientList(ex, var);

                    Id<ExprPtr> c, b, a;
                    return match(std::get<List>(*coeffList))(
                        pattern | ds(c, b, a.at(some(isRational))) = [&]
                        {
                            auto a_ = evald(*a);
                            auto const rKind = a_ > 0 ? relKind : invertRelational(relKind);

                            auto _2a = 2_i * (*a);
                            auto b2_4ac = ((*b)^2_i) - 4_i * (*a) * (*c);
                            auto sqrtB2_4ac = sqrt(b2_4ac);

                            auto result = match(b2_4ac)(
                                pattern | asDouble | when([&]
                                                          { return evald(b2_4ac) < 0; }) = [&]
                                {
                                    switch (rKind)
                                    {
                                    case RelationalKind::kLESS:
                                        return false_;

                                    case RelationalKind::kLESS_EQUAL:
                                        return false_;

                                    case RelationalKind::kGREATER:
                                        return true_;

                                    case RelationalKind::kGREATER_EQUAL:
                                        return true_;

                                    case RelationalKind::kEQUAL:
                                        throw std::logic_error{"Unreachable!"};
                                        return false_;
                                    }
                                    throw std::logic_error{"Unreachable!"};
                                    return false_;
                                },
                                pattern | _ = [&]
                                {
                                    auto left = (-(*b) - sqrtB2_4ac) / _2a;
                                    auto right = (-(*b) + sqrtB2_4ac) / _2a;
                                    switch (rKind)
                                    {
                                    case RelationalKind::kLESS:
                                        return interval(left, false, right, false);

                                    case RelationalKind::kLESS_EQUAL:
                                        return interval(left, true, right, true);

                                    case RelationalKind::kGREATER:
                                        return makeSharedExprPtr(SetOp{Union{{interval(-infinity, false, left, false),
                                                                                         interval(right, false, infinity, false)}}});

                                    case RelationalKind::kGREATER_EQUAL:
                                        return makeSharedExprPtr(SetOp{Union{{interval(-infinity, false, left, true),
                                                                                         interval(right, true, infinity, false)}}});

                                    case RelationalKind::kEQUAL:
                                        throw std::logic_error{"Unreachable!"};
                                    }
                                    throw std::logic_error{"Unreachable!"};
                                    return false_;
                                });
                            return intersect(result, domain);
                        },
                        pattern | ds(c, b.at(some(isRational))) = [&]
                        {
                            auto const root = -(*c) / (*b);

                            auto b_ = evald(*b);
                            auto const rKind = b_ > 0 ? relKind : invertRelational(relKind);
                            auto result = [&]
                            {
                                switch (rKind)
                                {
                                case RelationalKind::kLESS:
                                    return interval(-infinity, false, root, false);

                                case RelationalKind::kLESS_EQUAL:
                                    return interval(-infinity, false, root, true);

                                case RelationalKind::kGREATER:
                                    return interval(root, false, infinity, false);

                                case RelationalKind::kGREATER_EQUAL:
                                    return interval(root, true, infinity, false);

                                case RelationalKind::kEQUAL:
                                    throw std::logic_error{"Unreachable!"};
                                }
                                throw std::logic_error{"Unreachable!"};
                                return false_;
                            }();
                            return intersect(result, domain);
                        },
                        pattern | _ = [&]
                        {
                            throw std::runtime_error{"No match in solve!"};
                            return false_;
                        });

                    throw std::runtime_error{"Not implemented yet!"};
                    return false_;
                });

#if DEBUG
            std::cout << "solveInequationImpl: " << toString(ex) << ",\t" << toString(relKind)  << ",\t" << toString(var) << ",\t" << toString(domain) << ",\tresult: " << toString(result) << std::endl;
#endif // DEBUG
                return result;
        }

        ExprPtr solveInequation(ExprPtr const& ex, ExprPtr const& var, ExprPtr const& domain)
        {
#if DEBUG
            std::cout << "solveInequation: " << toString(ex) << ",\t" << toString(var) << ",\t" << toString(domain) << std::endl;
#endif // DEBUG

            auto const var_ = std::get<Symbol>(*var);
            using namespace matchit;
            Id<RelationalKind> iRelKind;
            Id<ExprPtr> iE1, iE2;
            auto result = match(ex)(
                pattern | or_(true_, false_) = expr(domain),
                pattern | some(as<Relational>(ds(iRelKind, iE1, iE2))) = [&]
                { return solveInequationImpl(expand(*iE1 - *iE2), *iRelKind, var, domain); });
#if DEBUG
            std::cout << "solveInequation: " << toString(ex) << ",\t" << toString(var) << ",\t" << toString(domain) << ",\tresult: " << toString(result) << std::endl;
#endif // DEBUG
            return result;
        }

    } // namespace impl
} // namespace mathiu
