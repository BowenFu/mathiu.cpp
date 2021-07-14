#ifndef CORE_H
#define CORE_H

#include "matchit.h"
#include <variant>
#include <memory>
#include <string>
#include <cmath>
#include <complex>
#include <vector>
#include <numeric>
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

    inline bool equal(ExprPtr const &lhs, ExprPtr const &rhs);

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

        inline ExprPtr operator^(ExprPtr const &lhs, ExprPtr const &rhs);
        // inline bool operator<(ExprPtr const &lhs, ExprPtr const &rhs) = delete;
        // inline bool operator==(ExprPtr const &lhs, ExprPtr const &rhs) = delete;
        inline ExprPtr operator+(ExprPtr const &lhs, ExprPtr const &rhs);
        inline ExprPtr operator-(ExprPtr const &lhs, ExprPtr const &rhs);
        inline ExprPtr operator-(ExprPtr const &rhs);
        inline ExprPtr operator*(ExprPtr const &lhs, ExprPtr const &rhs);
        inline ExprPtr operator/(ExprPtr const &lhs, ExprPtr const &rhs);

        // Atomic expressions

        using Integer = int32_t;

        struct Symbol : std::array<std::string, 1>
        {
        };

        struct Pi
        {
        };
        inline const auto pi = std::make_shared<Expr const>(Pi{});
        inline constexpr double pi_ = 3.1415926;

        struct E
        {
        };
        inline const auto e = std::make_shared<Expr const>(E{});
        inline const double e_ = std::exp(1);

        struct I
        {
        };
        inline const auto i = std::make_shared<Expr const>(I{});
        inline constexpr std::complex<double> i_ = std::complex<double>(0, 1);

        // Compound expressions

        inline bool less(ExprPtr const &lhs, ExprPtr const &rhs);

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

        struct Union : ExprPtrList
        {
        };

        struct Intersection : ExprPtrList
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

        struct CCInterval : ExprPtrPair
        {
        };

        struct Complexes
        {
        };

        struct True{};
        struct False{};

        using ExprVariant = std::variant<Integer, Fraction, Symbol, Pi, E, I, Sum, Product, Power, Log, Sin, Arctan, Set, List, Relational, PieceWise, SubstitutePair, CCInterval, Complexes, True, False, Logical>;

        struct Expr : ExprVariant
        {
            using variant::variant;
        };

        inline const auto complexes = std::make_shared<Expr const>(Complexes{});
        inline const auto true_ = std::make_shared<Expr const>(True{});
        inline const auto false_ = std::make_shared<Expr const>(False{});

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
            return std::make_shared<Expr const>(v);
        }

        inline ExprPtr operator ""_i(unsigned long long v)
        {
            return integer(static_cast<Integer>(v));
        }

        inline ExprPtr fraction(Integer l, Integer r)
        {
            return std::make_shared<Expr const>(Fraction{{l, r}});
        }

        inline bool equal(ExprPtr const &lhs, ExprPtr const &rhs);

        inline ExprPtr sin(ExprPtr const &ex)
        {
            using namespace matchit;
            return match(ex)(
                pattern | pi = expr(0_i),
                pattern | (pi / 2_i) = expr(1_i),
                pattern | _ = [&]
                { return std::make_shared<Expr const>(Sin{{ex}}); });
        }

        inline ExprPtr arctan(ExprPtr const &ex)
        {
            using namespace matchit;
            return match(ex)(
                pattern | _ = [&]
                { return std::make_shared<Expr const>(Arctan{{ex}}); });
        }

        inline ExprPtr symbol(std::string const &name)
        {
            return std::make_shared<Expr const>(Symbol{{name}});
        }

        inline ExprPtr operator ""_s(const char* str, std::size_t)
        {
            return symbol(str);
        }

        template <typename... Ts>
        inline ExprPtr set(Ts &&... lst)
        {
            return std::make_shared<Expr const>(Set{{lst...}});
        }

        inline ExprPtr operator>>(ExprPtr const& src, ExprPtr const& dst)
        {
            return std::make_shared<Expr const>(SubstitutePair{{src, dst}});
        }

        inline std::string toString(ExprPtr const &ex);

        inline std::string toString(std::string const& s)
        {
            return s;
        }

        template <typename T>
        inline std::string toString(T const& t)
        {
            return std::to_string(t);
        }

        inline bool equal(std::pair<ExprPtr const, ExprPtr > const &lhs, std::pair<ExprPtr const, ExprPtr > const &rhs)
        {
            return equal(lhs.second, rhs.second);
        }

        template <typename T>
        inline bool equal(T const t1, T const t2)
        {
            return t1 == t2;
        }

        inline bool less(double lhs, double rhs)
        {
            return lhs < rhs;
        }

        inline bool less(std::pair<ExprPtr const, ExprPtr>const& lhs, std::pair<ExprPtr const, ExprPtr>const& rhs)
        {
            return less(lhs.second, rhs.second);
        }

        inline bool less(std::string const& lhs, std::string const& rhs)
        {
            return lhs < rhs;
        }

        template <typename T, typename C1 = std::initializer_list<T>, typename C2 = std::initializer_list<T>>
        bool lessC(C1 const& v1, C2 const& v2)
        {
            auto i = std::rbegin(v1);
            auto j = std::rbegin(v2);
            for (; i != std::rend(v1) && j != std::rend(v2); ++i, ++j)
            {
                if (equal((*i), (*j)))
                {
                    continue;
                }
                else
                {
                    return less((*i), (*j));
                }
            }
            return v1.size() < v2.size();
        }

        template <typename T, typename C = std::initializer_list<T>>
        bool equalC(C const& v1, C const& v2)
        {
            return v1.size() == v2.size() && std::equal(std::begin(v1), std::end(v1), std::begin(v2), equalLambda);
        }

        inline double evald(ExprPtr const &ex);

        using namespace matchit;

        inline constexpr auto isRational = or_(as<int>(_), as<Fraction>(_));

        // The <| order relation
        // for basic commutative transformation
        inline bool less(ExprPtr const &lhs, ExprPtr const &rhs)
        {
#if VERBOSE_DEBUG
            std::cout << "less: " << toString(lhs) << "\t" << toString(rhs) << std::endl;
#endif // VERBOSE_DEBUG

            Id<std::string> isl, isr;
            Id<ExprPtr> iEl1, iEl2, iEr1, iEr2;
            Id<Product> iP1, iP2;
            Id<Sum> iS1, iS2;
            Id<SubstitutePair> iPair1, iPair2;
            Id<PieceWise> iPieceWise1, iPieceWise2;
            constexpr auto canBeProduct = or_(as<Power>(_), as<Log>(_), as<Sum>(_), as<Symbol>(_));
            constexpr auto canBePower = or_(as<Sum>(_), as<Log>(_), as<Symbol>(_));
            constexpr auto canBeLog = or_(as<Sum>(_), as<Symbol>(_));
            constexpr auto canBeSum = or_(as<Symbol>(_));
            return match(*lhs, *rhs)
            ( 
                // clang-format off
                pattern | ds(isRational, isRational)   = [&] { return evald(lhs) < evald(rhs); },
                pattern | ds(isRational, _)   = [&] { return true; },
                pattern | ds(_, isRational)   = [&] { return false; },
                pattern | ds(as<Symbol>(ds(isl)), as<Symbol>(ds(isr))) = [&] { return *isl < *isr; },
                pattern | ds(as<Product>(iP1), as<Product>(iP2)) = [&]
                {
                    return lessC<ExprPtr>(*iP1, *iP2);
                },
                pattern | ds(as<Product>(iP1), canBeProduct) = [&] {
                    return lessC<ExprPtr>(*iP1, Product{{{rhs, rhs}}});
                },
                pattern | ds(canBeProduct, as<Product>(iP2)) = [&] {
                    return lessC<ExprPtr>(Product{{{lhs, lhs}}}, *iP2);
                },
                pattern | ds(as<Power>(ds(iEl1, iEl2)), as<Power>(ds(iEr1, iEr2)))   = [&] {
                    return lessC<ExprPtr>({*iEl2, *iEl1}, {*iEr2, *iEr1});
                },
                pattern | ds(as<Power>(ds(iEl1, iEl2)), canBePower)   = [&] {
                    return lessC<ExprPtr>({*iEl2, *iEl1}, {1_i, rhs});
                },
                pattern | ds(canBePower, as<Power>(ds(iEr1, iEr2)))   = [&] {
                    return lessC<ExprPtr>({1_i, lhs}, {*iEr2, *iEr1});
                },
                pattern | ds(as<Log>(ds(iEl1, iEl2)), as<Log>(ds(iEr1, iEr2)))   = [&] {
                    return lessC<ExprPtr>({*iEl2, *iEl1}, {*iEr2, *iEr1});
                },
                pattern | ds(as<Log>(ds(iEl1, iEl2)), canBeLog)   = [&] {
                    return lessC<ExprPtr>({*iEl2, *iEl1}, {1_i, rhs});
                },
                pattern | ds(canBeLog, as<Log>(ds(iEr1, iEr2)))   = [&] {
                    return lessC<ExprPtr>({1_i, lhs}, {*iEr2, *iEr1});
                },
                pattern | ds(as<Sum>(iS1), as<Sum>(iS2)) = [&]
                {
                    return lessC<ExprPtr>(*iS1, *iS2);
                },
                pattern | ds(as<Sum>(iS1), canBeSum) = [&]
                {
                    return lessC<ExprPtr>(*iS1, Sum{{{rhs, rhs}}});
                },
                pattern | ds(canBeSum, as<Sum>(iS2)) = [&]
                {
                    return lessC<ExprPtr>(Sum{{{lhs, lhs}}}, *iS2);
                },
                pattern | ds(as<Sin>(ds(iEl1)), as<Sin>(ds(iEl2))) = [&] { return less(*iEl1, *iEl2); },
                pattern | ds(_, as<Sin>(_)) = expr(true),
                pattern | ds(as<Sin>(_), _) = expr(false),
                pattern | ds(as<I>(_), as<I>(_)) = expr(false),
                pattern | ds(_, as<I>(_)) = expr(true),
                pattern | ds(as<I>(_), _) = expr(false),
                pattern | ds(as<Pi>(_), as<Pi>(_)) = expr(false),
                pattern | ds(_, as<Pi>(_)) = expr(true),
                pattern | ds(as<Pi>(_), _) = expr(false),
                pattern | ds(as<E>(_), as<E>(_)) = expr(false),
                pattern | ds(_, as<E>(_)) = expr(true),
                pattern | ds(as<E>(_), _) = expr(false),
                pattern | ds(as<SubstitutePair>(as<ExprPtrPair>(ds(iEl1, iEl2))), as<SubstitutePair>(as<ExprPtrPair>(ds(iEr1, iEr2))))   = [&] {
                    return lessC<ExprPtr>({*iEl2, *iEl1}, {*iEr2, *iEr1});
                },
                pattern | ds(as<PieceWise>(iPieceWise1), as<PieceWise>(iPieceWise2))   = [&] {
                    return lessC<ExprPtr>(*iPieceWise1, *iPieceWise2);
                },
                pattern | ds(_, as<PieceWise>(_))   = expr(true),
                pattern | ds(as<PieceWise>(_), _)   = expr(false),
                pattern | _ = [&] { throw std::runtime_error{std::string("No match in less: ") + toString(lhs) + " ? " + toString(rhs)}; return false; }
                // clang-format on
            );
        }

        // The equality relation
        // for basic commutative transformation
        inline bool equal(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            if (lhs.get() == rhs.get())
            {
                return true;
            }
            Id<std::string> isl, isr;
            Id<ExprPtr> iEl1, iEl2, iEr1, iEr2;
            Id<Product> iP1, iP2;
            Id<Sum> iS1, iS2;
            return match(*lhs, *rhs)
            ( 
                // clang-format off
                pattern | ds(isRational, isRational)   = [&] { return evald(lhs) == evald(rhs); },
                pattern | ds(as<Symbol>(ds(isl)), as<Symbol>(ds(isr))) = [&] { return *isl == *isr; },
                pattern | ds(as<Product>(iP1), as<Product>(iP2)) = [&]
                {
                    return equalC<ExprPtr>(*iP1, *iP2);
                },
                pattern | ds(as<Power>(ds(iEl1, iEl2)), as<Power>(ds(iEr1, iEr2)))   = [&] {
                    return equalC<ExprPtr>({*iEl2, *iEl1}, {*iEr2, *iEr1});
                },
                pattern | ds(as<Log>(ds(iEl1, iEl2)), as<Log>(ds(iEr1, iEr2)))   = [&] {
                    return equalC<ExprPtr>({*iEl2, *iEl1}, {*iEr2, *iEr1});
                },
                pattern | ds(as<Sum>(iS1), as<Sum>(iS2)) = [&] { return equalC<ExprPtr>(*iS1, *iS2); },
                pattern | ds(as<Sin>(ds(iEl1)), as<Sin>(ds(iEr1))) = [&] { return equal(*iEl1, *iEr1); },
                pattern | ds(as<I>(_), as<I>(_)) = expr(true),
                pattern | ds(as<Pi>(_), as<Pi>(_)) = expr(true),
                pattern | ds(as<E>(_), as<E>(_)) = expr(true),
                pattern | ds(as<SubstitutePair>(as<ExprPtrPair>(ds(iEl1, iEr1))), as<SubstitutePair>(as<ExprPtrPair>(ds(iEl2, iEr2)))) = [&] {
                    return equalC<ExprPtr>({*iEl2, *iEl1}, {*iEr2, *iEr1});
                },
                pattern | _ = [&] { return false; }
                // clang-format on
            );
        }

        template <typename C, typename Op, typename Identity>
        auto merge(C const& c1, C const& c2, Op op, Identity identity) -> C
        {
            if (c1.size() < c2.size())
            {
                return merge(c2, c1, op, identity);
            }
            C result = c1;
            // We assume rational is at the beginning.
            auto const firstIsRational = matched(result.begin()->second, some(isRational));
            for (auto const& e : c2)
            {
                auto const bothRational = firstIsRational && matched(e.second, some(isRational));
                auto const it = bothRational? result.begin(): result.find(e.first);
                if (it == result.end())
                {
                    result.insert(e);
                }
                else
                {
                    auto const opResult = op(it->second, e.second);
                    if (equal(opResult, identity))
                    {
                        result.erase(it);
                    }
                    else
                    {
                        it->second = opResult;
                    }
                }
            }
            return result;
        }

        template <typename C>
        auto mergeSum(C const& c1, C const& c2)
        {
            constexpr auto add = [](auto&& lhs, auto&& rhs) { return lhs + rhs; };
            auto result = merge(c1, c2, add, 0_i);;
            if (result.size() == 1)
            {
                return (*result.begin()).second;
            }
            return std::make_shared<Expr const>(std::move(result));
        }

        template <typename C>
        auto mergeProduct(C const& c1, C const& c2)
        {
            constexpr auto mul = [](auto&& lhs, auto&& rhs) { return lhs * rhs; };
            auto result = merge(c1, c2, mul, 1_i);;
            if (result.size() == 1)
            {
                return (*result.begin()).second;
            }
            return std::make_shared<Expr const>(std::move(result));
        }

        inline ExprPtr simplifyRational(ExprPtr const &r)
        {
            Id<Integer> ii1, ii2;
            return match(*r)
            (
                pattern | as<Fraction>(ds(ii1, ii2)) = [&]{
                    if (*ii1 % *ii2 == 0)
                    {
                        return integer(*ii1 / *ii2);
                    }
                    auto const gcd = std::gcd(*ii1, *ii2);
                    if (*ii2 > 0)
                    {
                        return fraction(*ii1 / gcd, *ii2 / gcd);
                    }
                    return fraction(-*ii1 / gcd, -*ii2 / gcd);
                },
                pattern | _ = expr(r)
            );
        }

        inline std::complex<double> evalc(ExprPtr const &ex);

        inline constexpr auto asCoeff = or_(as<Integer>(_), as<Fraction>(_));
        inline constexpr auto second = [](auto&& p)
        {
            return p.second;
        };
        inline constexpr auto firstIsCoeff = [](auto &&id, auto &&whole)
        {
            // second part of the first pair
            return as<Product>(whole.at(ds(app(second, id.at(some(asCoeff))), ooo)));
        };

        inline std::pair<ExprPtr, ExprPtr> coeffAndTerm(Expr const &e)
        {
            Id<ExprPtr> icoeff;
            Id<Product> ip;
            return match(e)(
                pattern | firstIsCoeff(icoeff, ip) = [&]
                {
                    if ((*ip).size() > 2)
                    {
                        auto pCopy = *ip;
                        pCopy.erase(pCopy.begin());
                        return std::make_pair(*icoeff, std::make_shared<Expr const>(Product{pCopy}));
                    }
                    // single value left.
                    // the basic unary transformation.
                    return std::make_pair(*icoeff, (*(*ip).rbegin()).second);
                },
                pattern | _ = [&]
                { return std::make_pair(1_i, std::make_shared<Expr const>(e)); });
        }

        template <typename C, typename T>
        auto insertSum(C const& c, T const& t)
        {
            return mergeSum(c, C{{{coeffAndTerm(*t).second, t}}});
        }

        inline auto constexpr asCoeffAndRest = [](auto&& coeff, auto&& rest) { return app(coeffAndTerm, ds(coeff, rest)); };

        inline ExprPtr operator+(ExprPtr const &lhs, ExprPtr const &rhs)
        {
#if DEBUG
            std::cout << "operator+: " << toString(lhs) << "\t" << toString(rhs) << std::endl;
#endif // DEBUG
            Id<Sum> iSl, iSr;
            Id<Integer> iil, iir;
            Id<Integer> ii1, ii2, ii3, ii4;
            Id<ExprPtr> coeff1, coeff2, rest;
            return match(*lhs, *rhs)(
                // clang-format off
                // basic commutative transformation
                pattern | _ | when ([&]{ return less(rhs, lhs); }) = [&] {
                    return rhs + lhs;
                },
                // basic identity transformation
                pattern | ds(as<Integer>(0), _) = expr(rhs),
                pattern | ds(_, as<Integer>(0)) = expr(lhs),
                // basic associative transformation
                pattern | ds(as<Sum>(iSl), as<Sum>(iSr)) = [&] {
                    return mergeSum(*iSl, *iSr);
                },
                pattern | ds(as<Sum>(iSl), _) = [&] {
                    return insertSum(*iSl, rhs);
                },
                pattern | ds(_, as<Sum>(iSr)) = [&] {
                    return insertSum(*iSr, lhs);
                },
                // basic distributive transformation
                pattern | ds(as<Integer>(iil), as<Integer>(iir))   = [&] { return integer(*iil + *iir); },
                pattern | ds(as<Integer>(iil), as<Fraction>(ds(ii1, ii2)))   = [&] { return simplifyRational(fraction(*iil * *ii2 + *ii1, *ii2)); },
                pattern | ds(as<Fraction>(ds(ii1, ii2)), as<Integer>(iir))   = [&] { return simplifyRational(fraction(*iir * *ii2 + *ii1, *ii2)); },
                pattern | ds(as<Fraction>(ds(ii1, ii2)), as<Fraction>(ds(ii3, ii4)))   = [&] { return simplifyRational(fraction(*ii1 * *ii4 + *ii2 * *ii3, *ii2 * *ii4)); },
                // basic distributive transformation
                pattern | ds(asCoeffAndRest(coeff1, rest), asCoeffAndRest(coeff2, rest)) = [&]
                {
                    return ((*coeff1) + (*coeff2)) * (*rest);
                },
                pattern | _                            = [&] { return std::make_shared<Expr const>(Sum{{{coeffAndTerm(*lhs).second, lhs}, {coeffAndTerm(*rhs).second, rhs}}}); }
                // clang-format on
            );
        }

        inline std::pair<ExprPtr, ExprPtr> baseAndExp(Expr const &e)
        {
            Id<ExprPtr> iBase;
            Id<ExprPtr> iExp;
            return match(e)(
                pattern | as<Power>(ds(iBase, iExp)) = [&] {
                    return std::make_pair( *iBase, *iExp);
                },
                pattern | _ = [&]{ return std::make_pair(std::make_shared<Expr const>(e), 1_i); }
            );
        }

        template <typename C, typename T>
        auto insertProduct(C const& c, T const& t)
        {
            return mergeProduct(c, C{{{baseAndExp(*t).first, t}}});
        }

        inline auto constexpr asBaseAndExp = [](auto&& base, auto&& exp) { return app(baseAndExp, ds(base, exp)); };

        inline ExprPtr operator*(ExprPtr const &lhs, ExprPtr const &rhs)
        {
#if DEBUG
            std::cout << "operator*: " << toString(lhs) << "\t" << toString(rhs) << std::endl;
#endif // DEBUG

            Id<Product> iSl, iSr;
            Id<Integer> iil, iir;
            Id<Integer> ii1, ii2, ii3, ii4;
            Id<ExprPtr> iu, iv, iw;
            Id<ExprPtr> exp1, exp2, base;
            return match(*lhs, *rhs)(
                // clang-format off
                // basic commutative transformation
                pattern | _ | when ([&]{ return less(rhs, lhs); }) = [&] {
                    return rhs * lhs;
                },
                // basic identity transformation
                pattern | ds(as<Integer>(0), _) = expr(0_i),
                pattern | ds(_, as<Integer>(0)) = expr(0_i),
                pattern | ds(as<Integer>(1), _) = expr(rhs),
                pattern | ds(_, as<Integer>(1)) = expr(lhs),
                // basic associative transformation
                pattern | ds(as<Product>(iSl), as<Product>(iSr)) = [&] {
                    return mergeProduct(*iSl, *iSr);
                },
                pattern | ds(as<Product>(iSl), _) = [&] {
                    return insertProduct(*iSl, rhs);
                },
                pattern | ds(_, as<Product>(iSr)) = [&] {
                    return insertProduct(*iSr, lhs);
                },
                // basic distributive transformation
                pattern | ds(as<Integer>(iil), as<Integer>(iir))   = [&] { return integer(*iil * *iir); },
                pattern | ds(as<Integer>(iil), as<Fraction>(ds(ii1, ii2)))   = [&] { return simplifyRational(fraction(*iil * *ii1, *ii2)); },
                pattern | ds(as<Fraction>(ds(ii1, ii2)), as<Integer>(iir))   = [&] { return simplifyRational(fraction(*iir * *ii1, *ii2)); },
                pattern | ds(as<Fraction>(ds(ii1, ii2)), as<Fraction>(ds(ii3, ii4)))   = [&] { return simplifyRational(fraction(*ii1 * *ii3, *ii2 * *ii4)); },
                // basic power transformation 1
                pattern | ds(as<Power>(ds(iu, iv)), as<Power>(ds(iu, iw))) = iu^(iv+iw),
                // basic distributive transformation
                pattern | ds(asBaseAndExp(base, exp1), asBaseAndExp(base, exp2)) = [&]
                {
                    return (*base)^((*exp1) + (*exp2));
                },
                pattern | _                            = [&] { return std::make_shared<Expr const>(Product{{{baseAndExp(*lhs).first, lhs}, {baseAndExp(*rhs).first, rhs}}}); }
                // clang-format on
            );
        }

        inline ExprPtr pow(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            Id<ExprPtr> iu, iv;
            Id<Integer> ii1, ii2, ii3;
            Id<Product> ip;
            return match(*lhs, *rhs)(
                // clang-format off
                // basic power transformation 2,3
                pattern | ds(as<Power>(ds(iu, iv)), as<int>(_)) = iu^(iv * rhs),
                pattern | ds(as<Product>(ip), as<int>(_)) = [&] {
                    return std::accumulate((*ip).begin(), (*ip).end(), 1_i, [&](auto&& p, auto&& e)
                    {
                        return p * ((e.second)^rhs);
                    });
                },
                // basic identity transformation
                pattern | ds(as<Integer>(0), or_(as<Integer>(_), as<Fraction>(_))) | when([&]{ return evald(rhs) > 0; }) = expr(0_i),
                pattern | ds(as<Integer>(0), _)= [&] { throw std::runtime_error{"undefined!"}; return 0_i;},
                pattern | ds(as<Integer>(1), _)= [&] { return 1_i;},
                pattern | ds(_, as<Integer>(0))= expr(1_i),
                pattern | ds(_, as<Integer>(1))= expr(lhs),
                pattern | ds(as<Integer>(ii1), as<Integer>(ii2.at(_>0))) = [&] {return integer(static_cast<Integer>(std::pow(*ii1, *ii2))); },
                pattern | ds(as<Integer>(ii1), as<Integer>(ii2.at(_<0))) = [&] {return fraction(1, static_cast<Integer>(std::pow(*ii1, -(*ii2)))); },
                pattern | ds(as<Fraction>(ds(ii1, ii2)), as<Integer>(ii3)) = [&] {return simplifyRational(fraction(static_cast<Integer>(std::pow(*ii1, *ii3)), static_cast<Integer>(std::pow(*ii2, *ii3)))); },
                pattern | _ = [&] {
                    return std::make_shared<Expr const>(Power{{lhs, rhs}});
                }
            );
        }

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

        // the basic difference transformation
        inline ExprPtr operator-(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            return match(*lhs, *rhs)
            (
                // basic identity transformation
                pattern | ds(_, as<Integer>(0)) = expr(lhs),
                pattern | ds(as<Integer>(0), _) = [&] { return -rhs; },
                pattern | _ = [&] { return lhs + -1_i * rhs; }
            );
        }

        // the basic quotient transformation
        inline ExprPtr operator/(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            using namespace matchit;
            Id<Integer> il, ir;
            return match(*lhs, *rhs)(
                // clang-format off
                pattern | ds(as<Integer>(il), as<Integer>(ir)) = [&] { return simplifyRational(fraction(*il, *ir)); },
                // basic identity transformation
                pattern | ds(_, as<Integer>(0)) = [&] { throw std::runtime_error{"undefined!"}; return 0_i; },
                pattern | ds(as<Integer>(0), _) = expr(0_i),
                pattern | ds(_, as<Integer>(1)) = expr(lhs),
                pattern | _                                    = expr(lhs * (rhs ^ -1_i))
                // clang-format on
            );
        }

        inline ExprPtr log(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            return std::make_shared<Expr const>(Log{{lhs, rhs}});
        }

        inline ExprPtr operator==(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            if (equal(lhs, rhs))
            {
                return true_;
            }
            return std::make_shared<Expr const>(Relational{RelationalKind::kEQUAL, lhs, rhs});
        }

        inline ExprPtr operator<(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            if (equal(lhs, rhs))
            {
                return false_;
            }
            return match(*lhs, *rhs)
            (
                pattern | ds(isRational, isRational) = [&] { return evald(lhs) < evald(rhs) ? true_ : false_; },
                pattern | _ = [&] {
                    return std::make_shared<Expr const>(Relational{RelationalKind::kLESS, lhs, rhs});
                }
            );
        }

        inline ExprPtr operator<=(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            if (equal(lhs, rhs))
            {
                return true_;
            }
            return match(*lhs, *rhs)
            (
                pattern | ds(isRational, isRational) = [&] { return evald(lhs) <= evald(rhs) ? true_ : false_; },
                pattern | _ = [&] {
                    return std::make_shared<Expr const>(Relational{RelationalKind::kLESS_EQUAL, lhs, rhs});
                }
            );
        }

        inline ExprPtr operator>=(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            if (equal(lhs, rhs))
            {
                return true_;
            }
            return match(*lhs, *rhs)
            (
                pattern | ds(isRational, isRational) = [&] { return evald(lhs) >= evald(rhs) ? true_ : false_; },
                pattern | _ = [&] {
                    return std::make_shared<Expr const>(Relational{RelationalKind::kGREATER_EQUAL, lhs, rhs});
                }
            );
        }

        inline ExprPtr operator>(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            if (equal(lhs, rhs))
            {
                return false_;
            }
            return match(*lhs, *rhs)
            (
                pattern | ds(isRational, isRational) = [&] { return evald(lhs) > evald(rhs) ? true_ : false_; },
                pattern | _ = [&] {
                    return std::make_shared<Expr const>(Relational{RelationalKind::kGREATER, lhs, rhs});
                }
            );
        }

        inline ExprPtr relational(RelationalKind relKind, ExprPtr const &lhs, ExprPtr const &rhs)
        {
            switch (relKind)
            {
            case RelationalKind::kEQUAL:
                return lhs == rhs;
            case RelationalKind::kLESS_EQUAL:
                return lhs <= rhs;
            case RelationalKind::kGREATER_EQUAL:
                return lhs >= rhs;
            case RelationalKind::kLESS:
                return lhs < rhs;
            case RelationalKind::kGREATER:
                return lhs > rhs;
            }
        }

        inline ExprPtr operator&&(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            if (equal(lhs, rhs))
            {
                return lhs;
            }
            return match(lhs, rhs)
            (
                pattern | _ = [&] {
                    return std::make_shared<Expr const>(Logical{And{{{lhs, rhs}}}});
                }
            );
        }

        inline ExprPtr operator||(ExprPtr const &lhs, ExprPtr const &rhs)
        {
            if (equal(lhs, rhs))
            {
                return lhs;
            }
            return match(lhs, rhs)
            (
                pattern | _ = [&] {
                    return std::make_shared<Expr const>(Logical{Or{{{lhs, rhs}}}});
                }
            );
        }

        inline ExprPtr max(ExprPtr const& lhs, ExprPtr const& rhs)
        {
            return match(*lhs, *rhs)
            ( 
                pattern | ds(isRational, isRational) = [&]
                {
                    auto const l = evald(lhs);
                    auto const r = evald(rhs);
                    return l < r ? rhs : lhs;
                },
                pattern | _ = [&]
                {
                    return std::make_shared<Expr const>(PieceWise{{{lhs, lhs >= rhs}, {rhs, lhs < rhs}}});
                }
             );
        }

        inline ExprPtr min(ExprPtr const& lhs, ExprPtr const& rhs)
        {
            return match(*lhs, *rhs)
            ( 
                pattern | ds(isRational, isRational) = [&]
                {
                    auto const l = evald(lhs);
                    auto const r = evald(rhs);
                    return l < r ? lhs : rhs;
                },
                pattern | _ = [&]
                {
                return std::make_shared<Expr const>(PieceWise{{{lhs, lhs <= rhs}, {rhs, lhs > rhs}}});
                }
            );
        }

        inline double evald(ExprPtr const &ex)
        {
            assert(ex);
            Id<Integer> i, il, ir;
            Id<Sum> iS;
            Id<Product> iP;
            Id<ExprPtr > e, l, r;
            return match(*ex)(
                // clang-format off
                pattern | as<Integer>(i)                                = expr(i),
                pattern | as<Fraction>(ds(il, ir))                  = [&]{ return double(*il) / * ir; },
                pattern | as<Symbol>(_)                             = [&]{ throw std::runtime_error("Symbol should be replaced before calling evald."); return 0; },
                pattern | as<Sum>(iS)                                = [&]{
                    return std::accumulate((*iS).begin(), (*iS).end(), 0., [](auto&& sum, auto&& e){ return sum + evald(e.second); }); 
                },
                pattern | as<Product>(iP)                            = [&]{
                    return std::accumulate((*iP).begin(), (*iP).end(), 1., [](auto&& product, auto&& e){ return product * evald(e.second); }); 
                },
                pattern | as<Power>(ds(l, r))                       = [&]{ return std::pow(evald(*l), evald(*r)); },
                pattern | as<Sin>(ds(e))                            = [&]{ return std::sin(evald(*e)); },
                pattern | as<Log>(ds(l, r))                         = [&]{ return std::log2(evald(*r)) / std::log2(evald(*l)); },
                pattern | as<Pi>(_)                                 = expr(pi_),
                pattern | as<E>(_)                                  = expr(e_),
                pattern | _                                         = [&] { throw std::runtime_error{"No match in evald!"}; return 0;}
                // clang-format on
            );
        }

        inline std::complex<double> evalc(ExprPtr const &ex)
        {
            assert(ex);
            Id<Integer> i, il, ir;
            Id<Sum> iS;
            Id<Product> iP;
            Id<ExprPtr > e, l, r;
            return match(*ex)(
                // clang-format off
                pattern | as<Integer>(i)                                = expr(i),
                pattern | as<Fraction>(ds(il, ir))                  = [&]{ return double(*il) / * ir; },
                pattern | as<Symbol>(_)                             = [&]{ throw std::runtime_error("Symbol should be replaced before calling evalc."); return 0; },
                pattern | as<Sum>(iS)                                = [&]{
                    return std::accumulate((*iS).begin(), (*iS).end(), std::complex<double>(0), [](auto&& sum, auto&& e){ return sum + evalc(e.second); }); 
                },
                pattern | as<Product>(iP)                            = [&]{
                    return std::accumulate((*iP).begin(), (*iP).end(), std::complex<double>(1), [](auto&& product, auto&& e){ return product * evalc(e.second); }); 
                },
                pattern | as<Power>(ds(l, r))                       = [&]{ return std::pow(evalc(*l), evalc(*r)); },
                pattern | as<Sin>(ds(e))                            = [&]{ return std::sin(evalc(*e)); },
                pattern | as<Log>(ds(l, r))                         = [&]{ return std::log(evalc(*r)) / std::log(evalc(*l)); },
                pattern | as<Pi>(_)                                 = expr(pi_),
                pattern | as<E>(_)                                  = expr(e_),
                pattern | as<I>(_)                                  = expr(i_),
                pattern | _                                         = [&] { throw std::runtime_error{"No match in evalc!"}; return 0;}
                // clang-format on
            );
        }

        inline auto toString(RelationalKind relKind) -> std::string
        {
            switch (relKind)
            {
            case RelationalKind::kEQUAL:
                return "==";
            case RelationalKind::kLESS:
                return "<";
            case RelationalKind::kLESS_EQUAL:
                return "<=";
            case RelationalKind::kGREATER_EQUAL:
                return ">=";
            case RelationalKind::kGREATER:
                return ">";
            }
            throw std::runtime_error("Missing case!");
        }

        inline auto invertRelational(RelationalKind relKind) -> RelationalKind
        {
            switch (relKind)
            {
            case RelationalKind::kLESS:
                return RelationalKind::kGREATER;
            case RelationalKind::kLESS_EQUAL:
                return RelationalKind::kGREATER_EQUAL;
            case RelationalKind::kGREATER_EQUAL:
                return RelationalKind::kLESS_EQUAL;
            case RelationalKind::kGREATER:
                return RelationalKind::kLESS;
            case RelationalKind::kEQUAL:
                throw std::logic_error{"Unsupported relational : == in invertRelational!"};
            }
            throw std::runtime_error("Missing case!");
        }

        inline std::string toString(ExprPtr const &ex)
        {
            assert(ex);
            Id<Integer> ii, iil, iir;
            Id<std::string> is;
            Id<ExprPtr > ie, il, ir;
            Id<Sum> iS;
            Id<Product> iP;
            Id<Set> iSet;
            Id<List> iList;
            Id<And> iAnd;
            Id<Or> iOr;
            Id<PieceWise> iPieceWise;
            Id<RelationalKind> iRelKind;
            return match(*ex)(
                // clang-format off
                pattern | as<Integer>(ii)                            = [&]{ return std::to_string(*ii); },
                pattern | as<Fraction>(ds(iil, iir))                 = [&]{ return std::to_string(*iil) + "/" + std::to_string(*iir); },
                pattern | as<Symbol>(ds(is))                         = expr(is),
                pattern | as<Sum>(iS)                                = [&]{
                    std::string result = "(+ ";
                    for (auto e : *iS)
                    {
                        result += toString(e.second) + " ";
                    }
                    return result.substr(0, result.size()-1) + ")"; 
                },
                pattern | as<Product>(iP)                            = [&]{
                    std::string result = "(* ";
                    for (auto e : *iP)
                    {
                        result += toString(e.second) + " ";
                    }
                    return result.substr(0, result.size()-1) + ")"; 
                },
                pattern | as<Power>(ds(il, ir))                      = [&]{ return "(^ " + toString(*il) + " " + toString(*ir) + ")"; },
                pattern | as<Log>(ds(il, ir))                        = [&]{ return "(Log " + toString(*il) + " " + toString(*ir) + ")"; },
                pattern | as<Sin>(ds(ie))                            = [&]{ return "(Sin " + toString(*ie) + ")"; },
                pattern | as<Pi>(_)                                  = expr("pi"),
                pattern | as<E>(_)                                   = expr("e"),
                pattern | as<I>(_)                                   = expr("i"),
                pattern | as<Set>(iSet)                              = [&]{
                    if ((*iSet).empty())
                    {
                        return std::string("{}");
                    }
                    std::string result = "{";
                    for (auto e : *iSet)
                    {
                        result += toString(e) + " ";
                    }
                    return result.substr(0, result.size()-1) + "}"; 
                },
                pattern | as<List>(iList)                              = [&]{
                    if ((*iList).empty())
                    {
                        return std::string("[]");
                    }
                    std::string result = "[";
                    for (auto e : *iList)
                    {
                        result += toString(e) + " ";
                    }
                    return result.substr(0, result.size()-1) + "]"; 
                },
                pattern | as<PieceWise>(iPieceWise)                              = [&]{
                    if ((*iPieceWise).empty())
                    {
                        return std::string("(PieceWise)");
                    }
                    std::string result = "(PieceWise ";
                    for (auto e : *iPieceWise)
                    {
                        result += "(" + toString(e.first) + " " + toString(e.second) + ") ";
                    }
                    return result.substr(0, result.size()-1) + ")"; 
                },
                pattern | as<Relational>(ds(iRelKind, il, ir))                      = [&]{
                    return "(" + toString(*iRelKind) + " " + toString(*il) + " " + toString(*ir) + ")";
                },
                pattern | as<SubstitutePair>(as<ExprPtrPair>(ds(il, ir))) = [&] {
                    return "(SubstitutePair " + toString(*il) + " " + toString(*ir) + ")";
                },
                pattern | as<CCInterval>(as<ExprPtrPair>(ds(il, ir))) = [&] {
                    return "(CCInterval " + toString(*il) + " " + toString(*ir) + ")";
                },
                pattern | as<Complexes>(_) = [&] {
                    return "complexes";
                },
                pattern | as<True>(_) = [&] {
                    return "true";
                },
                pattern | as<False>(_) = [&] {
                    return "false";
                },
                pattern | as<Logical>(as<And>(iAnd))                                = [&]{
                    std::string result = "(And ";
                    for (auto e : *iAnd)
                    {
                        result += toString(e) + " ";
                    }
                    return result.substr(0, result.size()-1) + ")"; 
                },
                pattern | as<Logical>(as<Or>(iOr))                                = [&]{
                    std::string result = "(Or ";
                    for (auto e : *iOr)
                    {
                        result += toString(e) + " ";
                    }
                    return result.substr(0, result.size()-1) + ")"; 
                }
                // clang-format on
            );
        }

        inline bool freeOf(ExprPtr const &ex, ExprPtr const& var)
        {
            constexpr auto firstOrSecond = [](auto&& p) { return or_(ds(p, _), ds(_, p)); };
            Id<ExprPtrMap> iMap;
            return match(ex)(
                pattern | var = expr(false),
                pattern | some(or_(as<Sum>(iMap), as<Product>(iMap))) = [&]
                { return std::all_of((*iMap).begin(), (*iMap).end(), [&](auto &&e)
                                     { return freeOf(e.second, var); }); },
                pattern | some(as<Power>(firstOrSecond(var))) = expr(false),
                pattern | some(as<Log>(firstOrSecond(var))) = expr(false),
                pattern | some(as<Sin>(ds(var))) = expr(false),
                pattern | some(as<Arctan>(ds(var))) = expr(false),
                pattern | _ = expr(true)
                );
        }

        inline ExprPtr substituteImpl(ExprPtr const &ex, ExprPtrMap const &srcDstMap)
        {
#if DEBUG
            std::cout << "substituteImpl: " << toString(ex) << std::endl;
#endif // DEBUG
            auto const iter = srcDstMap.find(ex);
            if (iter != srcDstMap.end())
            {
                return iter->second;
            }

            // using ExprVariant = std::variant<Integer, Fraction, Symbol, Pi, E, I, Sum, Product, Power, Log, Sin, Arctan, Set, List, Relational, PieceWise>;
            auto const subs = [&] (auto&& e) { return substituteImpl(e, srcDstMap); };
            Id<Sum> iSum;
            Id<Product> iProduct;
            Id<ExprPtr> iE1, iE2;
            return match(ex)(
                pattern | some(as<Sum>(iSum)) = [&]
                {
                    return std::accumulate((*iSum).begin(), (*iSum).end(), 0_i, [&](auto&& sum, auto &&e)
                                   {
                                       auto elem = subs(e.second);
                                       return sum + elem;
                                   });
                },
                pattern | some(as<Product>(iProduct)) = [&]
                {
                    return std::accumulate((*iProduct).begin(), (*iProduct).end(), 1_i, [&](auto&& product, auto &&e)
                                   {
                                       auto elem = subs(e.second);
                                       return product * elem;
                                   });
                },
                pattern | some(as<Power>(ds(iE1, iE2))) = [&] {
                    return pow(subs(*iE1), subs(*iE2));
                },
                pattern | some(as<Log>(ds(iE1, iE2))) = [&] {
                    return log(subs(*iE1), subs(*iE2));
                },
                pattern | some(as<Sin>(ds(iE1))) = [&] {
                    return sin(subs(*iE1));
                },
                pattern | some(as<Arctan>(ds(iE1))) = [&] {
                    return arctan(subs(*iE1));
                },
                pattern | _ = expr(ex)
            );
        }

        inline ExprPtr substitute(ExprPtr const &ex, ExprPtr const &srcDstPairs)
        {
#if DEBUG
            std::cout << "substitute: " << toString(ex) << " " << toString(srcDstPairs) << std::endl;
#endif // DEBUG
            Id<Set> iSet;
            Id<SubstitutePair> iPair;
            auto const subMap = match(*srcDstPairs)(
                pattern | as<Set>(iSet) = [&]
                {
                    ExprPtrMap result;
                    std::transform((*iSet).begin(), (*iSet).end(), std::inserter(result, result.end()), [&](auto &&e)
                                   {
                                       auto pair = std::get<SubstitutePair>(*e);
                                       return pair;
                                   });
                    return result;
                },
                pattern | as<SubstitutePair>(iPair) = [&]
                { return ExprPtrMap{{{(*iPair).first, (*iPair).second}}}; },
                pattern | _ = [&]
                { throw std::runtime_error("Mismatch in substitute!"); return ExprPtrMap{}; });
            return substituteImpl(ex, subMap);
        }

        template <typename... Pairs>
        inline ExprPtr substitute(ExprPtr const &ex, ExprPtr const &srcDstPairs, Pairs const&... seqPairs)
        {
            return substitute(substitute(ex, srcDstPairs), seqPairs...);
        }

    } // namespace impl
    using impl::Integer;
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