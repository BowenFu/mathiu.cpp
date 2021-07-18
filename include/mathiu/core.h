#ifndef CORE_H
#define CORE_H

#include "matchit.h"
#include <variant>
#include <memory>
#include <string>
#include <complex>
#include <vector>
#include <map>
#include <set>
#include <list>

#define VERBOSE_DEBUG 0
#define DEBUG 1

#if DEBUG
#include <iostream>
#endif // DEBUG

namespace mathiu::impl
{
    struct Expr;
    using ExprPtr = std::shared_ptr<Expr const>;

    bool equal(ExprPtr const &lhs, ExprPtr const &rhs);

} // namespace mathiu::impl

namespace matchit::impl
{
    template <>
    class PatternTraits<mathiu::impl::ExprPtr>
    {
        using Pattern = mathiu::impl::ExprPtr;
    public:
        template <typename Value>
        using AppResultTuple = std::tuple<>;

        constexpr static auto nbIdV = 0;

        template <typename Value, typename ContextT>
        constexpr static auto matchPatternImpl(Value &&value, Pattern const &pattern,
                                               int32_t /* depth */,
                                               ContextT & /*context*/)
        {
            return mathiu::impl::equal(pattern, std::forward<Value>(value));
        }
        constexpr static void processIdImpl(Pattern const &, int32_t /*depth*/,
                                            IdProcess) {}
    };

    template <>
    class IdTraits<mathiu::impl::ExprPtr>
    {
        using Type = mathiu::impl::ExprPtr;
    public:
        static auto
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
            __attribute__((no_sanitize_address))
#endif
#endif
            equal(Type const &lhs, Type const &rhs)
        {
            return mathiu::impl::equal(lhs, rhs);
        }
    };

} // namespace matchit::impl

namespace mathiu
{
    namespace impl
    {
        struct Expr;

        using ExprPtr = std::shared_ptr<Expr const>;

        ExprPtr operator^(ExprPtr const &lhs, ExprPtr const &rhs);
        // bool operator<(ExprPtr const &lhs, ExprPtr const &rhs) = delete;
        // bool operator==(ExprPtr const &lhs, ExprPtr const &rhs) = delete;
        ExprPtr operator+(ExprPtr const &lhs, ExprPtr const &rhs);
        ExprPtr operator-(ExprPtr const &lhs, ExprPtr const &rhs);
        ExprPtr operator-(ExprPtr const &rhs);
        ExprPtr operator*(ExprPtr const &lhs, ExprPtr const &rhs);
        ExprPtr operator/(ExprPtr const &lhs, ExprPtr const &rhs);

        template <typename T>
        inline ExprPtr makeSharedExprPtr(T&& t)
        {
            return std::make_shared<Expr const>(std::forward<T>(t));
        }

        // Atomic expressions

        using Integer = int32_t;

        struct Symbol : std::array<std::string, 1>
        {
        };

        struct Pi
        {
        };
        inline const auto pi = makeSharedExprPtr(Pi{});
        inline constexpr double pi_ = 3.1415926;

        struct E
        {
        };
        inline const auto e = makeSharedExprPtr(E{});
        inline const double e_ = std::exp(1);

        struct I
        {
        };
        inline const auto i = makeSharedExprPtr(I{});
        inline constexpr std::complex<double> i_ = std::complex<double>(0, 1);

        struct Infinity
        {
        };

        inline const auto infinity = makeSharedExprPtr(Infinity{});

        // Compound expressions

        bool less(ExprPtr const &lhs, ExprPtr const &rhs);

        struct ExprPtrLess
        {
            inline bool operator()(ExprPtr const &lhs, ExprPtr const &rhs) const
            {
                return less(lhs, rhs);
            }
        };

        using ExprPtrPair = std::pair<ExprPtr, ExprPtr>;
        using ExprPtrMap = std::map<ExprPtr, ExprPtr, ExprPtrLess>;
        using ExprPtrSet = std::set<ExprPtr, ExprPtrLess>;
        using ExprPtrList = std::vector<ExprPtr>;
        template <size_t N>
        using ExprPtrArray = std::array<ExprPtr, N>;

        // algebraic

        struct Sum : ExprPtrMap
        {
        };

        struct Product : ExprPtrMap
        {
        };

        struct Power : ExprPtrArray<2>
        {
        };

        struct Log : ExprPtrArray<2>
        {
        };

        // TODO: a uniform Func type, std::tuple<std::string, ExprPtrMap>
        struct Sin : ExprPtrArray<1>
        {
        };

        struct Arctan : ExprPtrArray<1>
        {
        };

        struct Fraction : std::array<Integer, 2>
        {
        };

        struct Set : ExprPtrSet
        {
        };

        struct List : ExprPtrList
        {
        };

        enum class RelationalKind
        {
            kEQUAL,
            kLESS,
            kLESS_EQUAL,
            kGREATER,
            kGREATER_EQUAL
        };

        using Relational = std::tuple<RelationalKind, ExprPtr, ExprPtr>;

        // logical

        struct And : ExprPtrList
        {
        };

        struct Or : ExprPtrList
        {
        };

        struct Not : ExprPtrArray<1>
        {
        };

        using LogicalVariant = std::variant<And, Or, Not>;

        struct Logical : LogicalVariant
        {
        };

        // set

        struct Union : ExprPtrSet
        {
        };

        struct Intersection : ExprPtrSet
        {
        };

        struct Difference : ExprPtrArray<2>
        {
        };

        using SetOpVariant = std::variant<Union, Intersection, Difference>;

        struct SetOp : SetOpVariant
        {
        };

        struct PieceWise : std::vector<ExprPtrPair>
        {
        };

        struct SubstitutePair : ExprPtrPair
        {
        };

        using IntervalEnd = std::pair<ExprPtr, bool>; // true for close, false for open

        using Interval = std::pair<IntervalEnd, IntervalEnd>;

        struct True{};
        struct False{};

        using ExprVariant = std::variant<Integer, Fraction, Symbol, Pi, E, I, Infinity, Sum, Product, Power, Log, Sin, Arctan, Set, List, Relational, PieceWise, SubstitutePair, Interval, True, False, Logical, SetOp>;

        struct Expr : ExprVariant
        {
            using variant::variant;
        };

        inline const auto true_ = makeSharedExprPtr(True{});
        inline const auto false_ = makeSharedExprPtr(False{});

        inline bool operator==(ExprPtrPair const &l, ExprPtrPair const &r)
        {
            return equal(l.first, r.first) && equal(l.second, r.second);
        }

        inline bool operator==(ExprPtrMap const &l, ExprPtrMap const &r)
        {
            constexpr auto equalPair = [](auto&& x, auto&& y) { return equal(x.first, y.first) && equal(x.second, y.second); };
            return l.size() == r.size() && std::equal(l.begin(), l.end(), r.begin(), equalPair);
        }

        inline constexpr auto equalLambda = [](auto&& x, auto&& y) { return equal(x, y); };
        
        inline bool operator==(ExprPtrSet const &l, ExprPtrSet const &r)
        {
            return l.size() == r.size() && std::equal(l.begin(), l.end(), r.begin(), equalLambda);
        }

        inline bool operator==(ExprPtrList const &l, ExprPtrList const &r)
        {
            return l.size() == r.size() && std::equal(l.begin(), l.end(), r.begin(), equalLambda);
        }

        template <size_t N>
        inline bool operator==(ExprPtrArray<N> const &l, ExprPtrArray<N> const &r)
        {
            return std::equal(l.begin(), l.end(), r.begin(), equalLambda);
        }

        inline ExprPtr integer(Integer v)
        {
            return makeSharedExprPtr(v);
        }

        inline ExprPtr operator ""_i(unsigned long long v)
        {
            return integer(static_cast<Integer>(v));
        }

        inline ExprPtr fraction(Integer l, Integer r)
        {
            return makeSharedExprPtr(Fraction{{l, r}});
        }

        bool equal(ExprPtr const &lhs, ExprPtr const &rhs);

        ExprPtr sin(ExprPtr const &ex);

        ExprPtr arctan(ExprPtr const &ex);

        inline ExprPtr symbol(std::string const &name)
        {
            return makeSharedExprPtr(Symbol{{name}});
        }

        inline ExprPtr operator ""_s(const char* str, std::size_t)
        {
            return symbol(str);
        }

        template <typename... Ts>
        inline ExprPtr set(Ts &&... lst)
        {
            return makeSharedExprPtr(Set{{lst...}});
        }

        inline ExprPtr operator>>(ExprPtr const& src, ExprPtr const& dst)
        {
            return makeSharedExprPtr(SubstitutePair{{src, dst}});
        }

        std::string toString(ExprPtr const &ex);

        double evald(ExprPtr const &ex);

        inline constexpr auto isRational = matchit::or_(matchit::as<int>(matchit::_), matchit::as<Fraction>(matchit::_));

        // The <| order relation
        // for basic commutative transformation
        bool less(ExprPtr const &lhs, ExprPtr const &rhs);

        // The equality relation
        // for basic commutative transformation
        bool equal(ExprPtr const &lhs, ExprPtr const &rhs);

        std::complex<double> evalc(ExprPtr const &ex);

        std::pair<ExprPtr, ExprPtr> baseAndExp(Expr const &e);
        inline auto constexpr asBaseAndExp = [](auto&& base, auto&& exp) { return app(baseAndExp, ds(base, exp)); };

        ExprPtr pow(ExprPtr const &lhs, ExprPtr const &rhs);

        // note the operator^ precedence is not high, wrap the operands with parentheses.
        inline ExprPtr operator^(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            return pow(lhs, rhs);
        }

        inline ExprPtr sqrt(ExprPtr const &lhs)
        {
            return pow(lhs, 1_i / 2_i);
        }

        // the basic difference transformation
        inline ExprPtr operator-(ExprPtr const &rhs)
        {
            return integer(-1) * rhs;
        }

        inline ExprPtr log(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            return makeSharedExprPtr(Log{{lhs, rhs}});
        }

        ExprPtr operator==(ExprPtr const &lhs, ExprPtr const &rhs);

        ExprPtr operator<(ExprPtr const &lhs, ExprPtr const &rhs);

        ExprPtr operator<=(ExprPtr const &lhs, ExprPtr const &rhs);

        ExprPtr operator>=(ExprPtr const &lhs, ExprPtr const &rhs);

        ExprPtr operator>(ExprPtr const &lhs, ExprPtr const &rhs);

        ExprPtr relational(RelationalKind relKind, ExprPtr const &lhs, ExprPtr const &rhs);

        ExprPtr operator&&(ExprPtr const &lhs, ExprPtr const &rhs);

        ExprPtr operator||(ExprPtr const &lhs, ExprPtr const &rhs);

        ExprPtr max(ExprPtr const& lhs, ExprPtr const& rhs);

        ExprPtr min(ExprPtr const& lhs, ExprPtr const& rhs);

        double evald(ExprPtr const &ex);

        std::complex<double> evalc(ExprPtr const &ex);

        auto toString(RelationalKind relKind) -> std::string;

        auto invertRelational(RelationalKind relKind) -> RelationalKind;
        
        std::string toString(ExprPtr const &ex);

        bool freeOf(ExprPtr const &ex, ExprPtr const& var);

        ExprPtr substituteImpl(ExprPtr const &ex, ExprPtrMap const &srcDstMap);

        ExprPtr substitute(ExprPtr const &ex, ExprPtr const &srcDstPairs);

        template <typename... Pairs>
        inline ExprPtr substitute(ExprPtr const &ex, ExprPtr const &srcDstPairs, Pairs const&... seqPairs)
        {
            return substitute(substitute(ex, srcDstPairs), seqPairs...);
        }

        inline auto const asDouble = matchit::meet([](auto&& e)
        {
            try{
                evald(e);
                return true;
            }
            catch (...)
            {
                return false;
            }
        });

        inline ExprPtr interval(ExprPtr const& l, bool const lClose, ExprPtr const& r, bool const rClose)
        {
            return std::make_shared<impl::Expr const>(impl::Interval{{l, lClose}, {r, rClose}});
        }
    } // namespace impl
    using impl::integer;
    using impl::symbol;
    using impl::operator+;
    using impl::operator-;
    using impl::operator*;
    using impl::operator/;
    using impl::operator^;
    using impl::evald;
    using impl::evalc;
    using impl::pi;
    using impl::i;
    using impl::e;
    using impl::sin;
    using impl::toString;
    using impl::fraction;
    using impl::set;
    using impl::operator""_i;
    using impl::operator""_s;
} // namespace mathiu

#endif // CORE_H