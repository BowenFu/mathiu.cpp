#ifndef DIFF_H
#define DIFF_H

#include "matchit.h"
#include "mathiu.h"

namespace mathiu
{
    namespace impl
    {

        inline ExprPtr diff(ExprPtr const& exp, ExprPtr const& var)
        {
#if DEBUG
            std::cout << "diff: " << toString(exp) << "\t" << toString(var) << std::endl;
#endif // DEBUG

            using namespace matchit;
            constexpr auto isRational = or_(as<int>(_), as<Fraction>(_));
            Id<Sum> iS;
            Id<Product> iP;
            Id<ExprPtr> iEBase, iEExp, iENat;
            Id<ExprPtr> iE;
            return match(exp)(
                pattern | some(isRational) = expr(integer(0)),
                pattern | var = expr(integer(1)),
                pattern | some(as<Symbol>(_)) = expr(integer(0)),
                pattern | some(as<Sum>(iS)) = [&]
                { return std::accumulate((*iS).begin(), (*iS).end(), integer(0), [&](auto &&sum, auto &&e)
                                         { return sum + diff(e.second, var); }); },
                pattern | some(as<Product>(iP)) = [&]
                { return std::accumulate((*iP).begin(), (*iP).end(), integer(0), [&](auto &&sum, auto &&e)
                                         { return sum + diff(e.second, var) / e.second * exp; }); },
                pattern | some(as<Power>(ds(iEBase, iEExp))) = [&]
                {
                    auto const &n = *iEExp;
                    auto const &u = *iEBase;
                    return n * (u ^ (n - integer(1))) * diff(u, var);
                },
                pattern | some(as<Sin>(ds(iE))) = [&]
                {
                    return sin(pi / 2_i - *iE) * diff(*iE, var);
                },
                pattern | _ = [&]
                {
                    throw std::runtime_error{"No match in diff!"};
                    return integer(0);
                });
        }
    } // namespace impl
    using impl::diff;
} // namespace mathiu

#endif // DIFF_H