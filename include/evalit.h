#ifndef EVALIT_H
#define EVALIT_H

#include "matchit.h"
#include <variant>
#include <memory>
#include <string>
#include <cmath>
#include <complex>

namespace evalit
{
    namespace impl
    {
        using namespace matchit;

        struct Expr;

        struct Symbol
        {
            std::string name;
            std::shared_ptr<Expr> value;
        };

        struct Constant : std::variant<double, std::complex<double>>
        {
        };

        constexpr double pi_ = 3.1415926;
        inline const auto pi = std::make_shared<Expr>(Constant{{pi_}});

        constexpr std::complex<double> i_ = std::complex<double>(0, 1);
        inline const auto i = std::make_shared<Expr>(Constant{{i_}});

        struct Sum : std::array<std::shared_ptr<Expr>, 2>
        {
        };

        struct Product : std::array<std::shared_ptr<Expr>, 2>
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

        using ExprVariant = std::variant<int32_t, Symbol, Constant, Sum, Product, Power, Log, Sin, Arctan>;

        struct Expr : ExprVariant
        {
            using variant::variant;
        };

        inline std::shared_ptr<Expr> constant(int32_t v)
        {
            return std::make_shared<Expr>(v);
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
            return std::make_shared<Expr>(Symbol{name, {}});
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

        inline std::shared_ptr<Expr> operator/(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            return lhs * (rhs ^ constant(-1));
        }

        inline bool operator==(Expr const &l, Expr const &r)
        {
            return static_cast<ExprVariant const &>(l) ==
                   static_cast<ExprVariant const &>(r);
        }

        const auto asSymbolDs = asDsVia<Symbol>(&Symbol::value);

        inline double eval(const std::shared_ptr<Expr> &ex)
        {
            assert(ex);
            Id<int> i;
            Id<double> d;
            Id<std::complex<double>> c;
            Id<std::shared_ptr<Expr> > e, l, r;
            return match(*ex)(
                // clang-format off
                pattern | as<int>(i)                                = expr(i),
                pattern | asSymbolDs(e)                             = [&]{ return eval(*e); },
                pattern | as<Constant>(as<double>(d))               = expr(d),
                pattern | as<Constant>(as<std::complex<double>>(c)) = [&]{ assert((*c).imag() == 0); return (*c).real(); },
                pattern | as<Sum>(ds(l, r))                         = [&]{ return eval(*l) + eval(*r); },
                // Optimize multiplication by 0.
                pattern | as<Product>(ds(some(as<int>(0)), _))      = expr(0),
                pattern | as<Product>(ds(_, some(as<int>(0))))      = expr(0),
                pattern | as<Product>(ds(l, r))                     = [&]{ return eval(*l) * eval(*r); },
                pattern | as<Power>(ds(l, r))                       = [&]{ return std::pow(eval(*l), eval(*r)); },
                pattern | as<Sin>(ds(e))                            = [&]{ return std::sin(eval(*e)); }
                // clang-format on
            );
        }

        inline std::complex<double> ceval(const std::shared_ptr<Expr> &ex)
        {
            assert(ex);
            Id<int> i;
            Id<double> d;
            Id<std::complex<double>> c;
            Id<std::shared_ptr<Expr> > e, l, r;
            return match(*ex)(
                // clang-format off
                pattern | as<int>(i)                                = expr(i),
                pattern | asSymbolDs(e)                             = [&]{ return ceval(*e); },
                pattern | as<Constant>(as<double>(d))               = expr(d),
                pattern | as<Constant>(as<std::complex<double>>(c)) = expr(c),
                pattern | as<Sum>(ds(l, r))                         = [&]{ return ceval(*l) + ceval(*r); },
                // Optimize multiplication by 0.
                pattern | as<Product>(ds(some(as<int>(0)), _))      = expr(0),
                pattern | as<Product>(ds(_, some(as<int>(0))))      = expr(0),
                pattern | as<Product>(ds(l, r))                     = [&]{ return ceval(*l) * ceval(*r); },
                pattern | as<Power>(ds(l, r))                       = [&]{ return std::pow(ceval(*l), ceval(*r)); },
                pattern | as<Sin>(ds(e))                            = [&]{ return std::sin(ceval(*e)); }
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
    using impl::sin;
} // namespace evalit

#endif // EVALIT_H