#include "matchit.h"
#include "mathiu/core.h"
#include "mathiu/diff.h"

namespace mathiu::impl
{
    ExprPtr diff(ExprPtr const &exp, ExprPtr const &var)
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
        Id<PieceWise> iPieceWise;
        return match(exp)(
            pattern | some(isRational) = expr(0_i),
            pattern | var = expr(1_i),
            pattern | some(as<Symbol>(_)) = expr(0_i),
            pattern | some(as<Sum>(iS)) = [&]
            { return std::accumulate((*iS).begin(), (*iS).end(), 0_i, [&](auto &&sum, auto &&e)
                                     { return sum + diff(e.second, var); }); },
            pattern | some(as<Product>(iP)) = [&]
            { return std::accumulate((*iP).begin(), (*iP).end(), 0_i, [&](auto &&sum, auto &&e)
                                     { return sum + diff(e.second, var) / e.second * exp; }); },
            pattern | some(as<Power>(ds(iEBase, iEExp))) = [&]
            {
                auto const &n = *iEExp;
                auto const &u = *iEBase;
                return n * (u ^ (n - 1_i)) * diff(u, var);
            },
            pattern | some(as<Sin>(ds(iE))) = [&]
            { return sin(pi / 2_i - *iE) * diff(*iE, var); },
            pattern | some(as<PieceWise>(iPieceWise)) = [&]
            {
                PieceWise result;
                for (auto const &e : *iPieceWise)
                {
                    result.push_back({diff(e.first, var), e.second});
                }
                return std::make_shared<Expr const>(std::move(result));
            },
            pattern | _ = [&]
            {
                throw std::runtime_error{"No match in diff!"};
                return 0_i;
            });
    }
} // namespace mathiu::impl
