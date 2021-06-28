#ifndef mathia_H
#define mathia_H

#include "matchit.h"
#include <variant>
#include <memory>
#include <string>
#include <cmath>
#include <complex>
#include <iostream>
#include <vector>
#include <numeric>

namespace mathia
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

        inline bool less(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs);

        template <typename T>
        inline bool less(T const t1, T const t2)
        {
            return t1 < t2;
        }

        auto lessLambda = [](auto&& x, auto&& y) { return less(x, y); };

        inline std::string toString(const std::shared_ptr<Expr> &ex);

        inline std::string toString(std::string const& s)
        {
            return s;
        }

        template <typename T>
        inline std::string toString(T const& t)
        {
            return std::to_string((t));
        }

        template <typename T, typename C = std::initializer_list<T>>
        bool lessC(C const& v1, C const& v2)
        {
            std::cerr << "lessC: v1 size " << v1.size() << ", v2 size" << v2.size() << std::endl;
            for (auto i = std::rbegin(v1), j = std::rbegin(v2); i != std::rend(v1) && j != std::rend(v2); ++i, ++j)
            {
                if (*i == *j)
                {
                    std::cerr << "lessC : " << toString(*i) << " == " << toString(*j) << std::endl;
                    continue;
                }
                else
                {
                    std::cerr << "lessC : " << toString(*i) << (less(*i, *j) ? " < " : " > ") << toString(*j) << std::endl;
                    return less(*i, *j);
                }
            }
            std::cerr << "lessC : size " << v1.size() << (v1.size() < v2.size() ? " < " : " >= ") << v2.size() << std::endl;
            return v1.size() < v2.size();
        }

        inline double eval(const std::shared_ptr<Expr> &ex);

        // The <| order relation
        // for basic commutative transformation
        inline bool less(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            std::cerr << "comparing " << toString(lhs) << " " << toString(rhs) << std::endl;
            Id<std::complex<double>> ic1, ic2;
            Id<std::string> isl, isr;
            Id<std::shared_ptr<Expr>> iEl1, iEl2, iEr1, iEr2;
            Id<Product> iP1, iP2;
            Id<Sum> iS1, iS2;
            constexpr auto isReal = or_(as<int>(_), as<Fraction>(_), as<Constant>(as<double>(_)));
            return match(*lhs, *rhs)
            ( 
                // clang-format off
                pattern | ds(isReal, isReal)   = [&] { return eval(lhs) < eval(rhs); },
                pattern | ds(isReal, _)   = [&] { return true; },
                pattern | ds(_, isReal)   = [&] { return false; },
                pattern | ds(as<Constant>(as<std::complex<double>>(ic1)), as<Constant>(as<std::complex<double>>(ic2)))   = [&]
                {
                    return lessC<double>({(*ic1).imag(), (*ic1).real()}, {(*ic2).imag(), (*ic2).real()});
                },
                pattern | ds(as<Constant>(as<std::complex<double>>(_)), _)   = [&] { return true; },
                pattern | ds(_, as<Constant>(as<std::complex<double>>(_)))   = [&] { return false; },
                pattern | ds(as<Symbol>(ds(isl)), as<Symbol>(ds(isr))) = [&] { return *isl < *isr; },
                pattern | ds(as<Symbol>(_), _) = [&] { return true; },
                pattern | ds(_, as<Symbol>(_)) = [&] { return false; },
                pattern | ds(as<Product>(iP1), as<Product>(iP2)) = [&]
                {
                    std::cerr << "in product iP1 size " << (*iP1).size() << " iP2 size " << (*iP2).size() << std::endl;
                    return lessC<std::shared_ptr<Expr>>(*iP1, *iP2);
                },
                pattern | ds(as<Product>(_), _) = [&] { return true; },
                pattern | ds(_, as<Product>(_)) = [&] { return false; },
                pattern | ds(as<Power>(ds(iEl1, iEl2)), as<Power>(ds(iEr1, iEr2)))   = [&] {
                    return lessC<std::shared_ptr<Expr>>({*iEl2, *iEl1}, {*iEr2, *iEr1});
                },
                pattern | ds(as<Power>(_), _) = [&] { return true; },
                pattern | ds(_, as<Power>(_)) = [&] { return false; },
                pattern | ds(as<Sum>(iS1), as<Sum>(iS2)) = [&] { return lessC<std::shared_ptr<Expr>>(*iS1, *iS2); },
                pattern | ds(as<Sum>(_), _) = [&] { return true; },
                pattern | ds(_, as<Sum>(_)) = [&] { return false; },
                pattern | _ = [&] { return false; }
                // clang-format on
            );
        }

        template <typename C, typename T>
        auto insertSorted(C const& c, T const& t)
        {
            C copy = c;
            copy.insert(
                std::upper_bound(copy.begin(), copy.end(), t, lessLambda),
                t);
            return copy;
        }

        template <typename C>
        auto merge(C const& c1, C const& c2)
        {
            C result;
            result.reserve(c1.size() + c2.size());
            std::merge(c1.begin(), c1.end(), c2.begin(), c2.end(), std::back_inserter(result), lessLambda);
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

        inline std::shared_ptr<Expr> operator^(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs);
        
        inline std::shared_ptr<Expr> operator*(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            Id<Product> iSl, iSr;
            Id<int32_t> iil, iir;
            Id<int32_t> ii1, ii2;
            Id<double> id1, id2;
            Id<std::shared_ptr<Expr>> iu, iv, iw;
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
                // basic power transformation 1
                pattern | ds(as<Power>(ds(iu, iv)), as<Power>(ds(iu, iw))) = iu^(iv+iw),
                pattern | _                            = [&] { return std::make_shared<Expr>(Product{{lhs, rhs}}); }
                // clang-format on
            );
        }

        inline std::shared_ptr<Expr> operator^(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            Id<std::shared_ptr<Expr>> iu, iv;
            Id<Product> ip;
            return match(*lhs, *rhs)(
                // clang-format off
                // basic power transformation 2,3
                pattern | ds(as<Power>(ds(iu, iv)), as<int>(_)) = (iu^rhs) * (iv^rhs),
                pattern | ds(as<Product>(ip), as<int>(_)) = [&] {
                    return std::accumulate((*ip).begin(), (*ip).end(), constant(1), [&](auto&& p, auto&& e)
                    {
                        return p * e^rhs;
                    });
                },
                pattern | _ = [&] {
                    return std::make_shared<Expr>(Power{{lhs, rhs}});
                }
            );
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
    using impl::fraction;
} // namespace mathia

#endif // mathia_H