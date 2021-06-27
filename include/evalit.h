#ifndef EVALIT_H
#define EVALIT_H

#include "matchit.h"
#include <optional>
#include <variant>
#include <memory>
#include <string>
#include <cmath>

namespace evalit
{
    namespace impl
    {
        using namespace matchit;

        struct Symbol
        {
            std::string name;
            std::optional<int32_t> value;
        };

        struct Expr;

        struct Sum
        {
            std::shared_ptr<Expr> lhs, rhs;
        };

        struct Product
        {
            std::shared_ptr<Expr> lhs, rhs;
        };

        struct Power
        {
            std::shared_ptr<Expr> lhs, rhs;
        };

        using ExprVariant = std::variant<int32_t, Symbol, Sum, Product, Power>;

        struct Expr : ExprVariant
        {
            using variant::variant;
        };

        inline std::shared_ptr<Expr> value(int32_t v)
        {
            return std::make_shared<Expr>(v);
        }

        inline std::shared_ptr<Expr> symbol(std::string const &name)
        {
            return std::make_shared<Expr>(Symbol{name, {}});
        }

        inline std::shared_ptr<Expr> operator+(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            return std::make_shared<Expr>(Sum{lhs, rhs});
        }

        inline std::shared_ptr<Expr> operator*(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            return std::make_shared<Expr>(Product{lhs, rhs});
        }

        inline std::shared_ptr<Expr> operator^(std::shared_ptr<Expr> const &lhs, std::shared_ptr<Expr> const &rhs)
        {
            return std::make_shared<Expr>(Power{lhs, rhs});
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
        const auto asSumDs = asDsVia<Sum>(&Sum::lhs, &Sum::rhs);
        const auto asProductDs = asDsVia<Product>(&Product::lhs, &Product::rhs);
        const auto asPowerDs = asDsVia<Power>(&Power::lhs, &Power::rhs);

        inline double eval(const std::shared_ptr<Expr> &ex)
        {
            Id<int> i;
            Id<std::optional<int> > v;
            Id<std::shared_ptr<Expr> > e, l, r;
            return match(*ex)(
                // clang-format off
        pattern | as<int>(i)                       = expr(i),
        pattern | asSymbolDs(v)                    = [&]{ assert(*v); return **v; },
        pattern | asSumDs(l, r)                    = [&]{ return eval(*l) + eval(*r); },
        // Optimize multiplication by 0.
        pattern | asProductDs(some(as<int>(0)), _) = expr(0),
        pattern | asProductDs(_, some(as<int>(0))) = expr(0),
        pattern | asProductDs(l, r)                = [&]{ return eval(*l) * eval(*r); },
        pattern | asPowerDs(l, r)                  = [&]{ return std::pow(eval(*l), eval(*r)); }
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