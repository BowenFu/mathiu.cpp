#ifndef EVALIT_H
#define EVALIT_H

#include "matchit.h"
#include <variant>
#include <memory>
#include <string>
#include <cmath>

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

        struct Sum : std::array<std::shared_ptr<Expr>, 2>
        {
        };

        struct Product : std::array<std::shared_ptr<Expr>, 2>
        {
        };

        struct Power : std::array<std::shared_ptr<Expr>, 2>
        {
        };

        using ExprVariant = std::variant<int32_t, double, Symbol, Sum, Product, Power>;

        struct Expr : ExprVariant
        {
            using variant::variant;
        };

        inline std::shared_ptr<Expr> value(int32_t v)
        {
            return std::make_shared<Expr>(v);
        }

        inline std::shared_ptr<Expr> value(double v)
        {
            return std::make_shared<Expr>(v);
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
            return lhs + value(-1) * rhs;
        }

        inline std::shared_ptr<Expr> operator/(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            return lhs * (rhs ^ value(-1));
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
            Id<std::shared_ptr<Expr> > e, l, r;
            return match(*ex)(
                // clang-format off
                pattern | as<int>(i)                           = expr(i),
                pattern | as<double>(d)                        = expr(d),
                pattern | asSymbolDs(e)                        = [&]{ return eval(*e); },
                pattern | as<Sum>(ds(l, r))                    = [&]{ return eval(*l) + eval(*r); },
                // Optimize multiplication by 0.
                pattern | as<Product>(ds(some(as<int>(0)), _)) = expr(0),
                pattern | as<Product>(ds(_, some(as<int>(0)))) = expr(0),
                pattern | as<Product>(ds(l, r))                = [&]{ return eval(*l) * eval(*r); },
                pattern | as<Power>(ds(l, r))                  = [&]{ return std::pow(eval(*l), eval(*r)); }
                // clang-format on
            );
        }

    } // namespace impl
    using impl::value;
    using impl::symbol;
    using impl::operator+;
    using impl::operator-;
    using impl::operator*;
    using impl::operator/;
    using impl::operator^;
    using impl::eval;
} // namespace evalit

#endif // EVALIT_H