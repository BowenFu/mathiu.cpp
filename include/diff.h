#ifndef DIFF_H
#define DIFF_H

#include "matchit.h"
#include "mathiu.h"

namespace mathiu
{
    namespace impl
    {

        ExprPtr diffImpl(ExprPtr const& exp, Symbol const& var)
        {
#if DEBUG
            std::cout << "diffImpl: " << toString(exp) << "\t" << var[0] << std::endl;
#endif // DEBUG

            using namespace matchit;
            constexpr auto isRational = or_(as<int>(_), as<Fraction>(_));
            Id<Sum> iS;
            Id<Product> iP;
            Id<ExprPtr> iEBase, iEExp;
            return match(*exp)(
                pattern | isRational = expr(integer(0)),
                pattern | as<Symbol>(var) = expr(integer(1)),
                pattern | as<Symbol>(_) = expr(integer(0)),
                pattern | as<Sum>(iS) = [&]
                { return std::accumulate((*iS).begin(), (*iS).end(), integer(0), [&](auto &&sum, auto &&e)
                                         { return sum + diffImpl(e.second, var); }); },
                pattern | as<Product>(iP) = [&]
                { return std::accumulate((*iP).begin(), (*iP).end(), integer(0), [&](auto &&sum, auto &&e)
                                         { return sum + diffImpl(e.second, var) / e.second * exp; }); },
                pattern | as<Power>(ds(iEBase, iEExp)) = [&]
                {
                    auto const &n = *iEExp;
                    auto const &u = *iEBase;
                    return n * (u ^ (n - integer(1))) * diffImpl(u, var);
                },
                pattern | _ = [&]
                {
                    throw std::runtime_error{"No match in diff!"};
                    return integer(0);
                });
        }

        ExprPtr diff(ExprPtr const& exp, ExprPtr const& var)
        {
            auto const var_ = std::get<Symbol>(*var);
            return diffImpl(exp, var_);
        }
    } // namespace impl
    using impl::diff;
} // namespace mathiu

#endif // DIFF_H