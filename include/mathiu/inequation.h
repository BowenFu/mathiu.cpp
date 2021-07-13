#ifndef SOLVE_H
#define SOLVE_H

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

            auto const var_ = std::get<Symbol>(*var);
            using namespace matchit;
            const auto freeOfVar = meet([&](auto&& e) { return freeOf(e, var); });
            Id<Product> iP;
            Id<RelationalKind> iRelKind;
            Id<ExprPtr> iE1, iE2;
            return match(ex)(
                pattern | freeOfVar = [&]{
                    return relational(relKind, ex, 0_i);
                },
                pattern | _ = [&]
                {
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
            const auto freeOfVar = meet([&](auto&& e) { return freeOf(e, var); });
            Id<Product> iP;
            Id<RelationalKind> iRelKind;
            Id<ExprPtr> iE1, iE2;
            return match(ex)(
                pattern | or_(true_, false_) = (ex),
                pattern | some(as<Relational>(ds(iRelKind, iE1, iE2))) = [&]
                { return solveInequationImpl(expand(*iE1 - *iE2), *iRelKind, var, domain); });
        }

    } // namespace impl
    using impl::solve;
} // namespace mathiu

#endif // SOLVE_H