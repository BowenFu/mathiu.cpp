#include "matchit.h"
#include "mathiu/core.h"
#include "mathiu/inequation.h"
#include "mathiu/diff.h"
#include "mathiu/polynomial.h"

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
            return match(ex)(
                pattern | freeOfVar = [&]{
                    throw std::logic_error("Cannot solve inequation!");
                    return relational(relKind, ex, 0_i);
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

                            return match(b2_4ac)(
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
                                        return std::make_shared<Expr const>(Interval({left, false}, {right, false}));

                                    case RelationalKind::kLESS_EQUAL:
                                        return std::make_shared<Expr const>(Interval({left, true}, {right, true}));

                                    case RelationalKind::kGREATER:
                                        return std::make_shared<Expr const>(SetOp{Union{{std::make_shared<Expr const>(Interval({-infinity, false}, {left, false})),
                                                                                         std::make_shared<Expr const>(Interval({right, false}, {infinity, false}))}}});

                                    case RelationalKind::kGREATER_EQUAL:
                                        return std::make_shared<Expr const>(SetOp{Union{{std::make_shared<Expr const>(Interval({-infinity, false}, {left, true})),
                                                                                         std::make_shared<Expr const>(Interval({right, true}, {infinity, false}))}}});

                                    case RelationalKind::kEQUAL:
                                        throw std::logic_error{"Unreachable!"};
                                    }
                                    throw std::logic_error{"Unreachable!"};
                                    return false_;
                                });

                        },
                        pattern | ds(c, b.at(some(isRational))) = [&] {
                            auto const root = -(*c) / (*b);

                            auto b_ = evald(*b);
                            auto const rKind = b_ > 0 ? relKind : invertRelational(relKind);
                            switch (rKind)
                            {
                            case RelationalKind::kLESS:
                                return std::make_shared<Expr const>(Interval({-infinity, false}, {root, false}));
                            
                            case RelationalKind::kLESS_EQUAL:
                                return std::make_shared<Expr const>(Interval({-infinity, false}, {root, true}));
                            
                            case RelationalKind::kGREATER:
                                return std::make_shared<Expr const>(Interval({root, false}, {infinity, false}));
                            
                            case RelationalKind::kGREATER_EQUAL:
                                return std::make_shared<Expr const>(Interval({root, true}, {infinity, false}));

                            case RelationalKind::kEQUAL:
                                throw std::logic_error{"Unreachable!"};
                            }
                            throw std::logic_error{"Unreachable!"};
                            return false_;
                        },
                        pattern | _ = [&]
                        {
                            throw std::runtime_error{"No match in solve!"};
                            return false_;
                        });

                    throw std::runtime_error{"Not implemented yet!"};
                    return false_;
                });
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
            return match(ex)(
                pattern | or_(true_, false_) = expr(domain),
                pattern | some(as<Relational>(ds(iRelKind, iE1, iE2))) = [&]
                { return solveInequationImpl(expand(*iE1 - *iE2), *iRelKind, var, domain); });
        }

    } // namespace impl
} // namespace mathiu
