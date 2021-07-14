#ifndef INEQUATION_H
#define INEQUATION_H

#include "matchit.h"
#include "core.h"
#include "diff.h"
#include "polynomial.h"

namespace mathiu
{
    namespace impl
    {
        inline ExprPtr solveInequationImpl(ExprPtr const& ex, RelationalKind relKind, ExprPtr const& var, ExprPtr const& domain = complexes)
        {
#if DEBUG
            std::cout << "solveInequationImpl: " << toString(ex) << ",\t" << toString(relKind)  << ",\t" << toString(var) << ",\t" << toString(domain) << std::endl;
#endif // DEBUG

            using namespace matchit;
            const auto freeOfVar = meet([&](auto&& e) { return freeOf(e, var); });
            return match(ex)(
                pattern | freeOfVar = [&]{
                    return relational(relKind, ex, 0_i);
                },
                pattern | _ = [&]
                {
                    auto coeffList = coefficientList(ex, var);

                    Id<ExprPtr> c, b, a;
                    return match(std::get<List>(*coeffList))(
                        pattern | ds(c, b, a.at(some(isRational))) = [&]
                        {
                            auto _2a = 2_i * (*a);
                            auto sqrtB2_4ac = sqrt(((*b)^2_i) - 4_i * (*a) * (*c));
                            auto left = (-(*b) - sqrtB2_4ac) / _2a;
                            auto right = (-(*b) + sqrtB2_4ac) / _2a;

                            auto a_ = evald(*a);
                            auto const rKind = a_ > 0 ? relKind : invertRelational(relKind);
                            switch (rKind)
                            {
                            case RelationalKind::kLESS:
                                return (var > left) && (var < right);
                            
                            case RelationalKind::kLESS_EQUAL:
                                return (var >= left) && (var <= right);
                            
                            case RelationalKind::kGREATER:
                                return (var < left) || (var > right);
                            
                            case RelationalKind::kGREATER_EQUAL:
                                return (var <= left) || (var >= right);
                            case RelationalKind::kEQUAL:
                                throw std::logic_error{"Unreachable!"};
                            }
                            throw std::logic_error{"Unreachable!"};
                            return false_;
                        },
                        pattern | ds(c, b.at(some(isRational))) = [&] {
                            auto const root = -(*c) / (*b);

                            auto b_ = evald(*b);
                            auto const rKind = b_ > 0 ? relKind : invertRelational(relKind);
                            return relational(rKind, var, root);
                        },
                        pattern | _ = [&]
                        {
                            throw std::runtime_error{"No match in solve!"};
                            return set();
                        });

                    throw std::runtime_error{"Not implemented yet!"};
                    return false_;
                });
        }

        inline ExprPtr solveInequation(ExprPtr const& ex, ExprPtr const& var, ExprPtr const& domain = complexes)
        {
#if DEBUG
            std::cout << "solveInequation: " << toString(ex) << ",\t" << toString(var) << ",\t" << toString(domain) << std::endl;
#endif // DEBUG

            auto const var_ = std::get<Symbol>(*var);
            using namespace matchit;
            Id<RelationalKind> iRelKind;
            Id<ExprPtr> iE1, iE2;
            return match(ex)(
                pattern | or_(true_, false_) = expr(ex),
                pattern | some(as<Relational>(ds(iRelKind, iE1, iE2))) = [&]
                { return solveInequationImpl(expand(*iE1 - *iE2), *iRelKind, var, domain); });
        }

    } // namespace impl
} // namespace mathiu

#endif // INEQUATION_H