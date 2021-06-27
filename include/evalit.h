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

        struct Rational : std::array<int32_t, 2>
        {
        };

        using ExprVariant = std::variant<int32_t, Rational, Symbol, Constant, Sum, Product, Power, Log, Sin, Arctan>;

        struct Expr : ExprVariant
        {
            using variant::variant;
        };

        inline std::shared_ptr<Expr> constant(int32_t v)
        {
            return std::make_shared<Expr>(v);
        }

        inline std::shared_ptr<Expr> rational(int32_t l, int32_t r)
        {
            return std::make_shared<Expr>(Rational{{l, r}});
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

        inline std::shared_ptr<Expr> operator+(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            return std::make_shared<Expr>(Sum{{lhs, rhs}});
        }

        inline std::shared_ptr<Expr> operator*(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            return std::make_shared<Expr>(Product{{lhs, rhs}});
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
                pattern | ds(as<int32_t>(il), as<int32_t>(ir)) = [&] { return rational(*il, *ir); },
                pattern | _                                    = expr(lhs * (rhs ^ constant(-1)))
                // clang-format on
            );
        }

        inline std::shared_ptr<Expr> log(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            return std::make_shared<Expr>(Log{{lhs, rhs}});
        }

        inline bool operator==(Expr const &l, Expr const &r)
        {
            return static_cast<ExprVariant const &>(l) ==
                   static_cast<ExprVariant const &>(r);
        }

        inline double eval(const std::shared_ptr<Expr> &ex)
        {
            assert(ex);
            Id<int32_t> i, il, ir;
            Id<double> d;
            Id<std::complex<double>> c;
            Id<std::shared_ptr<Expr> > e, l, r;
            return match(*ex)(
                // clang-format off
                pattern | as<int32_t>(i)                                = expr(i),
                pattern | as<Symbol>(_)                             = [&]{ throw std::runtime_error("Symbol should be replaced before calling eval."); return 0; },
                pattern | as<Constant>(as<double>(d))               = expr(d),
                pattern | as<Constant>(as<std::complex<double>>(c)) = [&]{ assert((*c).imag() == 0); return (*c).real(); },
                pattern | as<Rational>(ds(il, ir))                  = [&]{ return double(*il) / * ir; },
                pattern | as<Sum>(ds(l, r))                         = [&]{ return eval(*l) + eval(*r); },
                // Optimize multiplication by 0.
                pattern | as<Product>(ds(some(as<int32_t>(0)), _))      = expr(0),
                pattern | as<Product>(ds(_, some(as<int32_t>(0))))      = expr(0),
                pattern | as<Product>(ds(l, r))                     = [&]{ return eval(*l) * eval(*r); },
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
            Id<std::complex<double>> c;
            Id<std::shared_ptr<Expr> > e, l, r;
            return match(*ex)(
                // clang-format off
                pattern | as<int32_t>(i)                                = expr(i),
                pattern | as<Symbol>(_)                             = [&]{ throw std::runtime_error("Symbol should be replaced before calling eval."); return 0; },
                pattern | as<Constant>(as<double>(d))               = expr(d),
                pattern | as<Constant>(as<std::complex<double>>(c)) = expr(c),
                pattern | as<Rational>(ds(il, ir))                  = [&]{ return double(*il) / * ir; },
                pattern | as<Sum>(ds(l, r))                         = [&]{ return ceval(*l) + ceval(*r); },
                // Optimize multiplication by 0.
                pattern | as<Product>(ds(some(as<int32_t>(0)), _))      = expr(0),
                pattern | as<Product>(ds(_, some(as<int32_t>(0))))      = expr(0),
                pattern | as<Product>(ds(l, r))                     = [&]{ return ceval(*l) * ceval(*r); },
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
            return match(*ex)(
                // clang-format off
                pattern | as<int32_t>(ii)                                = [&]{ return std::to_string(*ii); },
                pattern | as<Constant>(*pi)                          = expr("pi"),
                pattern | as<Constant>(*e)                           = expr("e"),
                pattern | as<Constant>(*i)                           = expr("i"),
                pattern | as<Constant>(as<double>(id))               = [&]{ return std::to_string(*id); },
                pattern | as<Constant>(as<std::complex<double>>(ic)) = [&]{ return std::to_string((*ic).real()) + " + " + std::to_string((*ic).imag()) + "i"; },
                pattern | as<Rational>(ds(iil, iir))                 = [&]{ return std::to_string(*iil) + "/" + std::to_string(*iir); },
                pattern | as<Symbol>(ds(is))                         = expr(is),
                pattern | as<Sum>(ds(il, ir))                        = [&]{ return "(+ " + toString(*il) + " " + toString(*ir) + ")"; },
                pattern | as<Product>(ds(il, ir))                    = [&]{ return "(* " + toString(*il) + " " + toString(*ir) + ")"; },
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