#ifndef EVALIT_H
#define EVALIT_H

#include "matchit.h"
#include <variant>
#include <memory>
#include <string>
#include <cmath>
#include <complex>
#include <iostream>
#include <vector>
#include <numeric>

namespace evalit
{
    namespace impl
    {
        using namespace matchit;

        struct Expr;

        struct Symbol : std::array<std::string, 1>
        {
        };

        struct Constant : std::variant<double, std::complex<double>>
        {
        };

        constexpr double pi_ = 3.1415926;
        inline const auto pi = std::make_shared<Expr>(Constant{{pi_}});

        inline const double e_ = std::exp(1);
        inline const auto e = std::make_shared<Expr>(Constant{{e_}});

        constexpr std::complex<double> i_ = std::complex<double>(0, 1);
        inline const auto i = std::make_shared<Expr>(Constant{{i_}});

        struct Sum : std::vector<std::shared_ptr<Expr>>
        {
        };

        struct Product : std::vector<std::shared_ptr<Expr>>
        {
        };

        struct Power : std::array<std::shared_ptr<Expr>, 2>
        {
        };

        struct Log : std::array<std::shared_ptr<Expr>, 2>
        {
        };

        struct Sin : std::array<std::shared_ptr<Expr>, 1>
        {
        };

        struct Arctan : std::array<std::shared_ptr<Expr>, 1>
        {
        };

        struct Fraction : std::array<int32_t, 2>
        {
        };

        using ExprVariant = std::variant<int32_t, Fraction, Symbol, Constant, Sum, Product, Power, Log, Sin, Arctan>;

        struct Expr : ExprVariant
        {
            using variant::variant;
        };

        inline bool operator==(Expr const &l, Expr const &r)
        {
            return static_cast<ExprVariant const &>(l) ==
                   static_cast<ExprVariant const &>(r);
        }

        inline std::shared_ptr<Expr> constant(int32_t v)
        {
            return std::make_shared<Expr>(v);
        }

        inline std::shared_ptr<Expr> fraction(int32_t l, int32_t r)
        {
            return std::make_shared<Expr>(Fraction{{l, r}});
        }

        inline std::shared_ptr<Expr> constant(double v)
        {
            return std::make_shared<Expr>(Constant{{v}});
        }

        inline std::shared_ptr<Expr> constant(std::complex<double> v)
        {
            return std::make_shared<Expr>(Constant{{v}});
        }

        inline std::shared_ptr<Expr> sin(std::shared_ptr<Expr> const &expr)
        {
            return std::make_shared<Expr>(Sin{{expr}});
        }

        inline std::shared_ptr<Expr> symbol(std::string const &name)
        {
            return std::make_shared<Expr>(Symbol{{name}});
        }

        template <typename C>
        inline bool less(C const& v1, C const& v2)
        {
            auto const size = std::min(v1.size(), v2.size());
            for (auto i = size ; i > 0; --i)
            {
                if (v1[i - 1] == v2[i - 1])
                {
                    continue;
                }
                else
                {
                    return v1[i - 1] < v2[i - 1];
                }
            }
            return v1.size() < v2.size();
        }

        // for basic commutative transformation
        inline bool operator<(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            Id<int32_t> ii1, ii2, ii3, ii4;
            Id<double> id1, id2;
            Id<std::complex<double>> ic1, ic2;
            Id<std::string> isl, isr;
            Id<std::shared_ptr<Expr>> iEl1, iEl2, iEr1, iEr2;
            Id<Product> iP1, iP2;
            Id<Sum> iS1, iS2;
            return match(*lhs, *rhs)
            ( 
                pattern | ds(as<int>(ii1), as<int>(ii2))   = [&] { return *ii1 < *ii2; },
                pattern | ds(as<int>(_), _)                = [&] { return true; },
                pattern | ds(_, as<int>(_))                = [&] { return false; },
                pattern | ds(as<Fraction>(ds(ii1, ii2)), as<Fraction>(ds(ii3, ii4)))   = [&] { return *ii1 * *ii4 < *ii2 * *ii3; },
                pattern | ds(as<Fraction>(_), _)                = [&] { return true; },
                pattern | ds(_, as<Fraction>(_))                = [&] { return false; },
                pattern | ds(as<Constant>(as<double>(id1)), as<Constant>(as<double>(id2)))   = [&] { return *id1 < *id2; },
                pattern | ds(as<Constant>(as<double>(_)), _)   = [&] { return true; },
                pattern | ds(_, as<Constant>(as<double>(_)))   = [&] { return false; },
                pattern | ds(as<Constant>(as<std::complex<double>>(ic1)), as<Constant>(as<std::complex<double>>(ic2)))   = [&]
                {
                    if ((*ic1).real() == (*ic2).real())
                    {
                        return (*ic1).imag() < (*ic2).imag();
                    }
                    return (*ic1).real() < (*ic2).real();
                },
                pattern | ds(as<Constant>(as<std::complex<double>>(_)), _)   = [&] { return true; },
                pattern | ds(_, as<Constant>(as<std::complex<double>>(_)))   = [&] { return false; },
                pattern | ds(as<Symbol>(ds(isl)), as<Symbol>(ds(isr))) = [&] { return *isl < *isr; },
                pattern | ds(as<Symbol>(_), _) = [&] { return true; },
                pattern | ds(_, as<Symbol>(_)) = [&] { return false; },
                pattern | ds(as<Product>(iP1), as<Product>(iP2)) = [&] { return less(*iP1, *iP2); },
                pattern | ds(as<Product>(_), _) = [&] { return true; },
                pattern | ds(_, as<Product>(_)) = [&] { return false; },
                pattern | ds(as<Power>(ds(iEl1, iEl2)), as<Power>(ds(iEr1, iEr2)))   = [&] {
                    if (*iEl1 == *iEr1)
                    {
                        return *iEl2 < *iEr2;
                    }
                    return *iEl1 < *iEl1;
                },
                pattern | ds(as<Power>(_), _) = [&] { return true; },
                pattern | ds(_, as<Power>(_)) = [&] { return false; },
                pattern | ds(as<Sum>(iS1), as<Sum>(iS2)) = [&] { return less(*iS1, *iS2); },
                pattern | ds(as<Sum>(_), _) = [&] { return true; },
                pattern | ds(_, as<Sum>(_)) = [&] { return false; },
                pattern | _ = [&] { return false; }
            );
        }

        template <typename C, typename T>
        auto insertSorted(C const& c, T const& t)
        {
            C copy = c;
            copy.insert(
                std::upper_bound(copy.begin(), copy.end(), t),
                t);
            return copy;
        }

        template <typename C>
        auto merge(C const& c1, C const& c2)
        {
            C result;
            result.reserve(c1.size() + c2.size());
            std::merge(c1.begin(), c1.end(), c2.begin(), c2.end(), std::back_inserter(result));
            return result;
        }

        inline std::shared_ptr<Expr> simplifyRational(std::shared_ptr<Expr> const &r)
        {
            Id<int32_t> ii1, ii2;
            return match(*r)
            (
                pattern | as<Fraction>(ds(ii1, ii2)) = [&]{
                    if (*ii1 / *ii2 * *ii2 == *ii1)
                    {
                        return constant(*ii1 / *ii2);
                    }
                    auto const gcd = std::gcd(*ii1, *ii2);
                    return fraction(*ii1 / gcd, *ii2 / gcd);
                },
                pattern | _ = expr(r)
            )
            ;
        }

        inline std::complex<double> ceval(const std::shared_ptr<Expr> &ex);

        inline std::shared_ptr<Expr> operator+(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            Id<Sum> iSl, iSr;
            Id<int32_t> iil, iir;
            Id<int32_t> ii1, ii2;
            Id<double> id1, id2;
            return match(*lhs, *rhs)(
                // clang-format off
                // basic commutative transformation
                pattern | _ | when ([&]{ return rhs < lhs; }) = [&] {
                    return rhs + lhs;
                },
                // basic associative transformation
                pattern | ds(as<Sum>(iSl), as<Sum>(iSr)) = [&] {
                    return std::make_shared<Expr>(Sum{{ merge(*iSl, *iSr) }});
                },
                pattern | ds(_, as<Sum>(iSr)) = [&] {
                    return std::make_shared<Expr>(Sum{{ insertSorted(*iSr, lhs) }});
                },
                // basic distributive transformation
                pattern | ds(as<int32_t>(iil), as<int32_t>(iir))   = [&] { return constant(*iil + *iir); },
                pattern | ds(as<int32_t>(iil), as<Fraction>(ds(ii1, ii2)))   = [&] { return simplifyRational(fraction(*iil * *ii2 + *ii1, *ii2)); },
                pattern | ds(as<Fraction>(_), as<int32_t>(_))   = [&] { return rhs + lhs; },
                pattern | ds(as<Constant>(as<double>(id1)), as<Constant>(as<double>(id2)))   = [&] { return constant(*id1 + *id2);; },
                pattern | ds(as<Constant>(_), as<Constant>(_)) = [&] { return constant(ceval(lhs) + ceval(rhs)); },
                pattern | _                            = [&] { return std::make_shared<Expr>(Sum{{lhs, rhs}}); }
                // clang-format on
            );
        }

        inline std::shared_ptr<Expr> operator*(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            Id<Product> iSl, iSr;
            Id<int32_t> iil, iir;
            Id<int32_t> ii1, ii2;
            Id<double> id1, id2;
            return match(*lhs, *rhs)(
                // clang-format off
                // basic commutative transformation
                pattern | _ | when ([&]{ return rhs < lhs; }) = [&] {
                    return rhs * lhs;
                },
                // basic associative transformation
                pattern | ds(as<Product>(iSl), as<Product>(iSr)) = [&] {
                    return std::make_shared<Expr>(Product{{ merge(*iSl, *iSr) }});
                },
                pattern | ds(_, as<Product>(iSr)) = [&] {
                    return std::make_shared<Expr>(Product{{ insertSorted(*iSr, lhs) }});
                },
                // basic distributive transformation
                pattern | ds(as<int32_t>(iil), as<int32_t>(iir))   = [&] { return constant(*iil * *iir); },
                pattern | ds(as<int32_t>(iil), as<Fraction>(ds(ii1, ii2)))   = [&] { return simplifyRational(fraction(*iil * *ii1, *ii2)); },
                pattern | ds(as<Constant>(as<double>(id1)), as<Constant>(as<double>(id2)))   = [&] { return constant(*id1 * *id2);; },
                pattern | ds(as<Constant>(_), as<Constant>(_)) = [&] { return constant(ceval(lhs) * ceval(rhs)); },
                pattern | _                            = [&] { return std::make_shared<Expr>(Product{{lhs, rhs}}); }
                // clang-format on
            );
        }

        inline std::shared_ptr<Expr> operator^(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            return std::make_shared<Expr>(Power{{lhs, rhs}});
        }

        inline std::shared_ptr<Expr> operator-(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            return lhs + constant(-1) * rhs;
        }

        inline std::shared_ptr<Expr> operator-(std::shared_ptr<Expr> const &rhs)
        {
            return constant(-1) * rhs;
        }

        inline std::shared_ptr<Expr> operator/(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            using namespace matchit;
            Id<int32_t> il, ir;
            return match(*lhs, *rhs)(
                // clang-format off
                pattern | ds(as<int32_t>(il), as<int32_t>(ir)) = [&] { return simplifyRational(fraction(*il, *ir)); },
                pattern | _                                    = expr(lhs * (rhs ^ constant(-1)))
                // clang-format on
            );
        }

        inline std::shared_ptr<Expr> log(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            return std::make_shared<Expr>(Log{{lhs, rhs}});
        }

        inline double eval(const std::shared_ptr<Expr> &ex)
        {
            assert(ex);
            Id<int32_t> i, il, ir;
            Id<double> d;
            Id<Sum> iS;
            Id<Product> iP;
            Id<std::complex<double>> c;
            Id<std::shared_ptr<Expr> > e, l, r;
            return match(*ex)(
                // clang-format off
                pattern | as<int32_t>(i)                                = expr(i),
                pattern | as<Symbol>(_)                             = [&]{ throw std::runtime_error("Symbol should be replaced before calling eval."); return 0; },
                pattern | as<Constant>(as<double>(d))               = expr(d),
                pattern | as<Constant>(as<std::complex<double>>(c)) = [&]{ assert((*c).imag() == 0); return (*c).real(); },
                pattern | as<Fraction>(ds(il, ir))                  = [&]{ return double(*il) / * ir; },
                pattern | as<Sum>(iS)                                = [&]{
                    return std::accumulate((*iS).begin(), (*iS).end(), 0., [](auto&& sum, auto&& e){ return sum + eval(e); }); 
                },
                pattern | as<Product>(iP)                            = [&]{
                    return std::accumulate((*iP).begin(), (*iP).end(), 1., [](auto&& product, auto&& e){ return product * eval(e); }); 
                },
                pattern | as<Power>(ds(l, r))                       = [&]{ return std::pow(eval(*l), eval(*r)); },
                pattern | as<Sin>(ds(e))                            = [&]{ return std::sin(eval(*e)); },
                pattern | as<Log>(ds(l, r))                         = [&]{ return std::log2(eval(*r)) / std::log2(eval(*l)); }
                // clang-format on
            );
        }

        inline std::complex<double> ceval(const std::shared_ptr<Expr> &ex)
        {
            assert(ex);
            Id<int32_t> i, il, ir;
            Id<double> d;
            Id<Sum> iS;
            Id<Product> iP;
            Id<std::complex<double>> c;
            Id<std::shared_ptr<Expr> > e, l, r;
            return match(*ex)(
                // clang-format off
                pattern | as<int32_t>(i)                                = expr(i),
                pattern | as<Symbol>(_)                             = [&]{ throw std::runtime_error("Symbol should be replaced before calling ceval."); return 0; },
                pattern | as<Constant>(as<double>(d))               = expr(d),
                pattern | as<Constant>(as<std::complex<double> >(c)) = expr(c),
                pattern | as<Fraction>(ds(il, ir))                  = [&]{ return double(*il) / * ir; },
                pattern | as<Sum>(iS)                                = [&]{
                    return std::accumulate((*iS).begin(), (*iS).end(), std::complex<double>(0), [](auto&& sum, auto&& e){ return sum + ceval(e); }); 
                },
                pattern | as<Product>(iP)                            = [&]{
                    return std::accumulate((*iP).begin(), (*iP).end(), std::complex<double>(1), [](auto&& product, auto&& e){ return product * ceval(e); }); 
                },
                pattern | as<Power>(ds(l, r))                       = [&]{ return std::pow(ceval(*l), ceval(*r)); },
                pattern | as<Sin>(ds(e))                            = [&]{ return std::sin(ceval(*e)); },
                pattern | as<Log>(ds(l, r))                         = [&]{ return std::log(ceval(*r)) / std::log(ceval(*l)); }
                // clang-format on
            );
        }

        inline std::string toString(const std::shared_ptr<Expr> &ex)
        {
            assert(ex);
            Id<int32_t> ii, iil, iir;
            Id<double> id;
            Id<std::string> is;
            Id<std::complex<double>> ic;
            Id<std::shared_ptr<Expr> > ie, il, ir;
            Id<Sum> iS;
            Id<Product> iP;
            return match(*ex)(
                // clang-format off
                pattern | as<int32_t>(ii)                                = [&]{ return std::to_string(*ii); },
                pattern | as<Constant>(*pi)                          = expr("pi"),
                pattern | as<Constant>(*e)                           = expr("e"),
                pattern | as<Constant>(*i)                           = expr("i"),
                pattern | as<Constant>(as<double>(id))               = [&]{ return std::to_string(*id); },
                pattern | as<Constant>(as<std::complex<double> >(ic)) = [&]{ return std::to_string((*ic).real()) + " + " + std::to_string((*ic).imag()) + "i"; },
                pattern | as<Fraction>(ds(iil, iir))                 = [&]{ return std::to_string(*iil) + "/" + std::to_string(*iir); },
                pattern | as<Symbol>(ds(is))                         = expr(is),
                pattern | as<Sum>(iS)                                = [&]{
                    std::string result = "(+ ";
                    for (auto e : *iS)
                    {
                        result += toString(e) + " ";
                    }
                    return result.substr(0, result.size()-1) + ")"; 
                },
                pattern | as<Product>(iP)                            = [&]{
                    std::string result = "(* ";
                    for (auto e : *iP)
                    {
                        result += toString(e) + " ";
                    }
                    return result.substr(0, result.size()-1) + ")"; 
                },
                pattern | as<Power>(ds(il, ir))                      = [&]{ return "(^ " + toString(*il) + " " + toString(*ir) + ")"; },
                pattern | as<Log>(ds(il, ir))                        = [&]{ return "(Log " + toString(*il) + " " + toString(*ir) + ")"; },
                pattern | as<Sin>(ds(ie))                            = [&]{ return "(Sin " + toString(*ie) + ")"; }
                // clang-format on
            );
        }


    } // namespace impl
    using impl::constant;
    using impl::symbol;
    using impl::operator+;
    using impl::operator-;
    using impl::operator*;
    using impl::operator/;
    using impl::operator^;
    using impl::eval;
    using impl::pi;
    using impl::i;
    using impl::e;
    using impl::sin;
    using impl::toString;
} // namespace evalit

#endif // EVALIT_H