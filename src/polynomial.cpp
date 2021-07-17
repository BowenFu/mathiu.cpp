#include "matchit.h"
#include "mathiu/core.h"
#include "mathiu/diff.h"
#include "mathiu/polynomial.h"
#include <cmath>

namespace mathiu::impl
{
    Integer degreeMonomial(ExprPtr const &monomial, Set const &varSet)
    {
#if VERBOSE_DEBUG
        std::cout << "degreeMonomial: " << toString(monomial) << ",\t" << std::endl;
#endif // VERBOSE_DEBUG

        Id<Product> iP;
        Id<ExprPtr> base, exp;
        return match(*monomial)(
            pattern | as<Product>(iP) = [&]
            {
                return std::accumulate(varSet.begin(), varSet.end(), 0, [&](Integer sum, auto &&e)
                                       {
                                           auto const iter = (*iP).find(e);
                                           if (iter == (*iP).end())
                                           {
                                               return sum;
                                           }
                                           auto const exp = baseAndExp(*(*iter).second).second;
                                           return sum + std::get<Integer>(*exp);
                                       });
            },
            pattern | asBaseAndExp(base, exp) = [&]
            {
                auto const iter = varSet.find(*base);
                if (iter == varSet.end())
                {
                    return 0;
                }
                return std::get<Integer>(**exp);
            });
    }

    Integer degree(ExprPtr const &ex, ExprPtr const &varSet)
    {
#if DEBUG
        std::cout << "degree: " << toString(ex) << ",\t" << toString(varSet) << std::endl;
#endif // DEBUG

        using namespace matchit;
        Id<Set> iSet;
        auto const varSet_ = match(*varSet)(
            pattern | as<Set>(iSet) = [&]
            { return *iSet; },
            pattern | _ = [&]
            { return Set{{{varSet}}}; });

        Id<Sum> iS;
        return match(*ex)(
            pattern | as<Sum>(iS) = [&]
            { return std::accumulate((*iS).begin(), (*iS).end(), 0, [&](Integer sum, auto &&e)
                                     { return std::max(sum, degreeMonomial(e.second, varSet_)); }); },
            pattern | _ = [&]
            { return degreeMonomial(ex, varSet_); });
    }

    // Fix me, check monomial first
    std::pair<ExprPtr, Integer> coefficientMonomial(ExprPtr const &monomial, ExprPtr const &x)
    {
#if VERBOSE_DEBUG
        std::cout << "coefficientMonomial: " << toString(monomial) << ",\t" << toString(x) << std::endl;
#endif // VERBOSE_DEBUG

        const auto freeOfVar = meet([&](auto &&e)
                                    { return freeOf(e, x); });

        Id<Product> iP;
        Id<Integer> iiExp;
        return match(monomial)(
            pattern | x = [&]
            { return std::make_pair(1_i, 1); },
            pattern | some(as<Power>(ds(x, some(as<Integer>(iiExp.at(_ > 1)))))) = [&]
            { return std::make_pair(1_i, *iiExp); },
            pattern | some(as<Product>(iP)) = [&]
            {
                return std::accumulate((*iP).begin(), (*iP).end(), std::make_pair(monomial, 0), [&](std::pair<ExprPtr, Integer> const &result, auto &&e)
                                       {
                                           auto f = coefficientMonomial(e.second, x);
                                           Integer const m = f.second;
                                           if (m == 0)
                                           {
                                               return result;
                                           }
                                           return std::make_pair(monomial / (x ^ integer(m)), m);
                                       });
            },
            pattern | freeOfVar = expr(std::make_pair(monomial, 0)), pattern | _ = [&]
                                                                     {
                                                                         throw std::runtime_error("Mismatch in coefficientMonomial!");
                                                                         return std::make_pair(monomial, 0);
                                                                     });
    }

    ExprPtr coefficient(ExprPtr const &u, ExprPtr const &x, Integer i)
    {
#if DEBUG
        std::cout << "coefficient: " << toString(u) << ",\t" << toString(x) << ",\t" << i << std::endl;
#endif // DEBUG

        using namespace matchit;
        Id<Sum> iS;
        return match(u)(
            pattern | x = [&]
            { return i == 1 ? 1_i : 0_i; },
            pattern | some(as<Sum>(iS)) = [&]
            {
                return std::accumulate((*iS).begin(), (*iS).end(), 0_i, [&](ExprPtr c, auto &&e)
                                       {
                                           auto f = coefficientMonomial(e.second, x);
                                           return (f.second == i) ? c + f.first : c;
                                       });
            },
            pattern | _ = [&]
            {
                auto f = coefficientMonomial(u, x);
                return (f.second == i) ? f.first : 0_i;
            });
    }

    ExprPtr coefficientList(ExprPtr const &u, ExprPtr const &x)
    {
        auto const deg = degree(u, x);
        List result;
        for (auto i = 0; i <= deg; ++i)
        {
            result.emplace_back(coefficient(u, x, i));
        }
        return makeSharedExprPtr(std::move(result));
    }

    // algebraic expand
    ExprPtr expandProduct(ExprPtr const &lhs, ExprPtr const &rhs)
    {
#if VERBOSE_DEBUG
        std::cout << "expandProduct: " << toString(lhs) << ",\t" << toString(rhs) << std::endl;
#endif // VERBOSE_DEBUG

        Id<Sum> iSum;
        return match(*lhs, *rhs)(
            // clang-format off
                pattern | ds(as<Sum>(iSum), _) = [&]
                {
                    return std::accumulate((*iSum).begin(), (*iSum).end(), 0_i, [&](auto&& sum, auto&& e)
                        {
                            return sum + expandProduct(e.second, rhs);
                        }
                    );
                },
                pattern | ds(_, as<Sum>(_)) = [&]
                {
                    return expandProduct(rhs, lhs);
                },
                pattern | _ = [&] {return lhs*rhs; }
            // clang-format on
        );
    }

    Integer factorial(Integer n)
    {
        assert(n >= 0);
        return static_cast<Integer>(std::tgamma(n + 1));
    }

    ExprPtr expandPower(ExprPtr const &base, int32_t const n)
    {
#if VERBOSE_DEBUG
        std::cout << "expandPower: " << toString(base) << ",\t" << n << std::endl;
#endif // VERBOSE_DEBUG

        Id<ExprPtr> iFirst;
        return match(*base)(
            pattern | as<Sum>(ds(ds(_, iFirst), ooo)) = [&]
            {
                auto const f = *iFirst;
                auto const r = base - f;
                auto sum = 0_i;
                for (int k = 0; k <= n; ++k)
                {
                    auto const c = factorial(n) / (factorial(k) * factorial(n - k));
                    sum = sum + expandProduct(integer(c) * (f ^ integer(n - k)), expandPower(r, k));
                }
                return sum;
            },
            pattern | _ = [&]
            { return base ^ integer(n); });
    }

    ExprPtr expand(ExprPtr const &ex)
    {
#if VERBOSE_DEBUG
        std::cout << "expand: " << toString(ex) << std::endl;
#endif // VERBOSE_DEBUG

        Id<Sum> iSum;
        Id<Product> iProduct;
        Id<ExprPtr> iBase;
        Id<Integer> iiExp;
        return match(*ex)(
            pattern | as<Sum>(iSum) = [&]
            {
                return std::accumulate((*iSum).begin(), (*iSum).end(), 0_i, [&](auto &&sum, auto &&e)
                                       {
                                           // Use Sum{} + merge for better perf
                                           return sum + expand(e.second);
                                       });
            },
            pattern | as<Product>(iProduct) = [&]
            { return std::accumulate((*iProduct).begin(), (*iProduct).end(), 1_i, [&](auto &&product, auto &&e)
                                     { return expandProduct(product, expand(e.second)); }); },
            pattern | as<Power>(ds(iBase, some(as<Integer>(iiExp.at(_ > 1))))) = [&]
            { return expandPower(expand(*iBase), *iiExp); },
            pattern | _ = expr(ex));
    }

} // namespace mathiu
