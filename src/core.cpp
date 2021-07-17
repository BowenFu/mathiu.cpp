#include "mathiu/core.h"
#include "matchit.h"

namespace mathiu::impl
{
    ExprPtr sin(ExprPtr const &ex)
    {
        using namespace matchit;
        return match(ex)(
            pattern | pi = expr(0_i),
            pattern | (pi / 2_i) = expr(1_i),
            pattern | _ = [&]
            { return makeSharedExprPtr(Sin{{ex}}); });
    }

    ExprPtr arctan(ExprPtr const &ex)
    {
        using namespace matchit;
        return match(ex)(
            pattern | _ = [&]
            { return makeSharedExprPtr(Arctan{{ex}}); });
    }

    // The <| order relation
    // for basic commutative transformation
    bool less(ExprPtr const &lhs, ExprPtr const &rhs)
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
        Id<Union> iUnion1, iUnion2;
        Id<IntervalEnd> iIntervalEnd1, iIntervalEnd2, iIntervalEnd3, iIntervalEnd4;
        Id<Set> iSet1, iSet2;
        constexpr auto canBeProduct = or_(as<Power>(_), as<Log>(_), as<Sum>(_), as<Symbol>(_));
        constexpr auto canBePower = or_(as<Sum>(_), as<Log>(_), as<Symbol>(_));
        constexpr auto canBeLog = or_(as<Sum>(_), as<Symbol>(_));
        constexpr auto canBeSum = or_(as<Symbol>(_));
        return match(*lhs, *rhs)(
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
                pattern | ds(as<Infinity>(_), as<Infinity>(_)) = expr(false),
                pattern | ds(_, as<Infinity>(_)) = expr(true),
                pattern | ds(as<Infinity>(_), _) = expr(false),
                pattern | ds(as<SubstitutePair>(as<ExprPtrPair>(ds(iEl1, iEl2))), as<SubstitutePair>(as<ExprPtrPair>(ds(iEr1, iEr2))))   = [&] {
                    return lessC<ExprPtr>({*iEl2, *iEl1}, {*iEr2, *iEr1});
                },
                pattern | ds(as<PieceWise>(iPieceWise1), as<PieceWise>(iPieceWise2))   = [&] {
                    return lessC<ExprPtr>(*iPieceWise1, *iPieceWise2);
                },
                pattern | ds(_, as<PieceWise>(_))   = expr(true),
                pattern | ds(as<PieceWise>(_), _)   = expr(false),
                pattern | ds(as<SetOp>(as<Union>(iUnion1)), as<SetOp>(as<Union>(iUnion2)))   = [&] {
                    return lessC<ExprPtr>(*iUnion1, *iUnion2);
                },
                pattern | ds(as<Interval>(ds(iIntervalEnd1, iIntervalEnd2)), as<Interval>(ds(iIntervalEnd3, iIntervalEnd4)))   = [&] {
                    return lessC<IntervalEnd>({*iIntervalEnd2, *iIntervalEnd1}, {*iIntervalEnd4, *iIntervalEnd3});
                },
                pattern | ds(as<Interval>(_), as<SetOp>(as<Union>(_))) = expr(true),
                pattern | ds(as<SetOp>(as<Union>(_)), as<Interval>(_)) = expr(false),
                pattern | ds(as<Set>(iSet1), as<Set>(iSet2)) = [&]
                {
                    return lessC<ExprPtr>(*iSet1, *iSet2);
                },
                pattern | _ = [&] { throw std::runtime_error{std::string("No match in less: ") + toString(lhs) + " ? " + toString(rhs)}; return false; }
            // clang-format on
        );
    }

    // The equality relation
    // for basic commutative transformation
    bool equal(ExprPtr const &lhs, ExprPtr const &rhs)
    {
        if (lhs.get() == rhs.get())
        {
            return true;
        }
        Id<std::string> isl, isr;
        Id<ExprPtr> iEl1, iEl2, iEr1, iEr2;
        Id<Product> iP1, iP2;
        Id<Sum> iS1, iS2;
        Id<Union> iUnion1, iUnion2;
        Id<IntervalEnd> iIntervalEnd1, iIntervalEnd2, iIntervalEnd3, iIntervalEnd4;
        return match(*lhs, *rhs)(
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
                pattern | ds(as<Infinity>(_), as<Infinity>(_)) = expr(true),
                pattern | ds(as<SubstitutePair>(as<ExprPtrPair>(ds(iEl1, iEr1))), as<SubstitutePair>(as<ExprPtrPair>(ds(iEl2, iEr2)))) = [&] {
                    return equalC<ExprPtr>({*iEl2, *iEl1}, {*iEr2, *iEr1});
                },
                pattern | ds(as<SetOp>(as<Union>(iUnion1)), as<SetOp>(as<Union>(iUnion2)))   = [&] {
                    return equalC<ExprPtr>(*iUnion1, *iUnion2);
                },
                pattern | ds(as<Interval>(ds(iIntervalEnd1, iIntervalEnd2)), as<Interval>(ds(iIntervalEnd3, iIntervalEnd4)))   = [&] {
                    return equalC<IntervalEnd>({*iIntervalEnd2, *iIntervalEnd1}, {*iIntervalEnd4, *iIntervalEnd3});
                },
                pattern | _ = [&] { return false; }
            // clang-format on
        );
    }

    ExprPtr simplifyRational(ExprPtr const &r)
    {
        Id<Integer> ii1, ii2;
        return match(*r)(
            pattern | as<Fraction>(ds(ii1, ii2)) = [&]
            {
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
            pattern | _ = expr(r));
    }

    std::pair<ExprPtr, ExprPtr> coeffAndTerm(Expr const &e)
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
                    return std::make_pair(*icoeff, makeSharedExprPtr(Product{pCopy}));
                }
                // single value left.
                // the basic unary transformation.
                return std::make_pair(*icoeff, (*(*ip).rbegin()).second);
            },
            pattern | _ = [&]
            { return std::make_pair(1_i, makeSharedExprPtr(e)); });
    }

    template <typename Op>
    ExprPtr transformPieceWise(PieceWise p, Op op)
    {
        for (auto &e : p)
        {
            e.first = op(e.first);
        }
        return makeSharedExprPtr(std::move(p));
    }

    ExprPtr operator+(ExprPtr const &lhs, ExprPtr const &rhs)
    {
#if VERBOSE_DEBUG
        std::cout << "operator+: " << toString(lhs) << "\t" << toString(rhs) << std::endl;
#endif // VERBOSE_DEBUG
        Id<Sum> iSl, iSr;
        Id<Integer> iil, iir;
        Id<Integer> ii1, ii2, ii3, ii4;
        Id<ExprPtr> coeff1, coeff2, rest;
        Id<PieceWise> iPieceWise;
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
                // piecewise
                pattern | ds(as<PieceWise>(iPieceWise), _) = [&]
                {
                    return transformPieceWise(*iPieceWise, [&](auto&& e) { return e + rhs; });
                },
                pattern | ds(_, as<PieceWise>(iPieceWise)) = [&]
                {
                    return transformPieceWise(*iPieceWise, [&](auto&& e) { return lhs + e; });
                },
                // basic distributive transformation
                pattern | ds(asCoeffAndRest(coeff1, rest), asCoeffAndRest(coeff2, rest)) = [&]
                {
                    return ((*coeff1) + (*coeff2)) * (*rest);
                },
                pattern | _                            = [&] { return makeSharedExprPtr(Sum{{{coeffAndTerm(*lhs).second, lhs}, {coeffAndTerm(*rhs).second, rhs}}}); }
            // clang-format on
        );
    }

    std::pair<ExprPtr, ExprPtr> baseAndExp(Expr const &e)
    {
        Id<ExprPtr> iBase;
        Id<ExprPtr> iExp;
        return match(e)(
            pattern | as<Power>(ds(iBase, iExp)) = [&]
            { return std::make_pair(*iBase, *iExp); },
            pattern | _ = [&]
            { return std::make_pair(makeSharedExprPtr(e), 1_i); });
    }

    ExprPtr operator*(ExprPtr const &lhs, ExprPtr const &rhs)
    {
#if VERBOSE_DEBUG
        std::cout << "operator*: " << toString(lhs) << "\t" << toString(rhs) << std::endl;
#endif // VERBOSE_DEBUG

        Id<Product> iSl, iSr;
        Id<Integer> iil, iir;
        Id<Integer> ii1, ii2, ii3, ii4;
        Id<ExprPtr> iu, iv, iw;
        Id<ExprPtr> exp1, exp2, base;
        Id<PieceWise> iPieceWise;
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
                // piecewise
                pattern | ds(as<PieceWise>(iPieceWise), _) = [&]
                {
                    return transformPieceWise(*iPieceWise, [&](auto&& e) { return e * rhs; });
                },
                pattern | ds(_, as<PieceWise>(iPieceWise)) = [&]
                {
                    return transformPieceWise(*iPieceWise, [&](auto&& e) { return lhs * e; });
                },
                // basic distributive transformation
                pattern | ds(asBaseAndExp(base, exp1), asBaseAndExp(base, exp2)) = [&]
                {
                    return (*base)^((*exp1) + (*exp2));
                },
                pattern | _                            = [&] { return makeSharedExprPtr(Product{{{baseAndExp(*lhs).first, lhs}, {baseAndExp(*rhs).first, rhs}}}); }
            // clang-format on
        );
    }

    ExprPtr pow(ExprPtr const &lhs, ExprPtr const &rhs)
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
                    return makeSharedExprPtr(Power{{lhs, rhs}});
                }
            );
        }

        // the basic difference transformation
        ExprPtr operator-(ExprPtr const &lhs, ExprPtr const &rhs)
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
        ExprPtr operator/(ExprPtr const &lhs, ExprPtr const &rhs)
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

    ExprPtr operator==(ExprPtr const &lhs, ExprPtr const &rhs)
    {
        if (equal(lhs, rhs))
        {
            return true_;
        }
        return makeSharedExprPtr(Relational{RelationalKind::kEQUAL, lhs, rhs});
    }

    ExprPtr operator<(ExprPtr const &lhs, ExprPtr const &rhs)
    {
        if (equal(lhs, rhs))
        {
            return false_;
        }
        return match(lhs, rhs)(
            pattern | ds(asDouble, asDouble) = [&]
            { return evald(lhs) < evald(rhs) ? true_ : false_; },
            pattern | _ = [&]
            { return makeSharedExprPtr(Relational{RelationalKind::kLESS, lhs, rhs}); });
    }

    ExprPtr operator<=(ExprPtr const &lhs, ExprPtr const &rhs)
    {
        if (equal(lhs, rhs))
        {
            return true_;
        }
        return match(lhs, rhs)(
            pattern | ds(asDouble, asDouble) = [&]
            { return evald(lhs) <= evald(rhs) ? true_ : false_; },
            pattern | _ = [&]
            { return makeSharedExprPtr(Relational{RelationalKind::kLESS_EQUAL, lhs, rhs}); });
    }

    ExprPtr operator>=(ExprPtr const &lhs, ExprPtr const &rhs)
    {
        if (equal(lhs, rhs))
        {
            return true_;
        }
        return match(lhs, rhs)(
            pattern | ds(asDouble, asDouble) = [&]
            { return evald(lhs) >= evald(rhs) ? true_ : false_; },
            pattern | _ = [&]
            { return makeSharedExprPtr(Relational{RelationalKind::kGREATER_EQUAL, lhs, rhs}); });
    }

    ExprPtr operator>(ExprPtr const &lhs, ExprPtr const &rhs)
    {
        if (equal(lhs, rhs))
        {
            return false_;
        }
        return match(lhs, rhs)(
            pattern | ds(asDouble, asDouble) = [&]
            { return evald(lhs) > evald(rhs) ? true_ : false_; },
            pattern | _ = [&]
            { return makeSharedExprPtr(Relational{RelationalKind::kGREATER, lhs, rhs}); });
    }

    ExprPtr relational(RelationalKind relKind, ExprPtr const &lhs, ExprPtr const &rhs)
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
        throw std::logic_error{"Unreachable"};
    }

    ExprPtr operator&&(ExprPtr const &lhs, ExprPtr const &rhs)
    {
        if (equal(lhs, rhs))
        {
            return lhs;
        }
        return match(lhs, rhs)(
            pattern | _ = [&]
            { return makeSharedExprPtr(Logical{And{{{lhs, rhs}}}}); });
    }

    ExprPtr operator||(ExprPtr const &lhs, ExprPtr const &rhs)
    {
        if (equal(lhs, rhs))
        {
            return lhs;
        }
        return match(lhs, rhs)(
            pattern | _ = [&]
            { return makeSharedExprPtr(Logical{Or{{{lhs, rhs}}}}); });
    }

    ExprPtr max(ExprPtr const &lhs, ExprPtr const &rhs)
    {
        return match(lhs, rhs)(
            pattern | ds(asDouble, asDouble) = [&]
            {
                auto const l = evald(lhs);
                auto const r = evald(rhs);
                return l < r ? rhs : lhs;
            },
            pattern | _ = [&]
            { return makeSharedExprPtr(PieceWise{{{lhs, lhs >= rhs}, {rhs, lhs < rhs}}}); });
    }

    ExprPtr min(ExprPtr const &lhs, ExprPtr const &rhs)
    {
        return match(lhs, rhs)(
            pattern | ds(asDouble, asDouble) = [&]
            {
                auto const l = evald(lhs);
                auto const r = evald(rhs);
                return l < r ? lhs : rhs;
            },
            pattern | _ = [&]
            { return makeSharedExprPtr(PieceWise{{{lhs, lhs <= rhs}, {rhs, lhs > rhs}}}); });
    }

    double evald(ExprPtr const &ex)
    {
        assert(ex);
        Id<Integer> i, il, ir;
        Id<Sum> iS;
        Id<Product> iP;
        Id<ExprPtr> e, l, r;
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
                pattern | as<Infinity>(_)                           = expr(std::numeric_limits<double>::infinity()),
                pattern | _                                         = [&] { throw std::runtime_error{"No match in evald!"}; return 0;}
            // clang-format on
        );
    }

    std::complex<double> evalc(ExprPtr const &ex)
    {
        assert(ex);
        Id<Integer> i, il, ir;
        Id<Sum> iS;
        Id<Product> iP;
        Id<ExprPtr> e, l, r;
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
                pattern | as<Infinity>(_)                           = expr(std::numeric_limits<double>::infinity()),
                pattern | as<I>(_)                                  = expr(i_),
                pattern | _                                         = [&] { throw std::runtime_error{"No match in evalc!"}; return 0;}
            // clang-format on
        );
    }

    auto toString(RelationalKind relKind) -> std::string
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

    auto invertRelational(RelationalKind relKind) -> RelationalKind
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

    std::string toString(ExprPtr const &ex)
    {
        assert(ex);
        Id<Integer> ii, iil, iir;
        Id<std::string> is;
        Id<ExprPtr> ie, il, ir;
        Id<Sum> iS;
        Id<Product> iP;
        Id<Set> iSet;
        Id<List> iList;
        Id<And> iAnd;
        Id<Or> iOr;
        Id<PieceWise> iPieceWise;
        Id<RelationalKind> iRelKind;
        Id<bool> iB1, iB2;
        Id<Union> iUnion;
        Id<Intersection> iIntersection;
        Id<Difference> iDifference;
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
                pattern | as<Infinity>(_)                            = expr("inf"),
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
                pattern | as<Interval>(ds(ds(il, iB1), ds(ir, iB2))) = [&] {
                    return std::string("(") + (*iB1? "C" : "O") + (*iB2? "C" : "O") + "Interval " +
                    toString(*il) + " " + toString(*ir) + ")";
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
                },
                pattern | as<SetOp>(as<Union>(iUnion))                                = [&]{
                    std::string result = "(Union ";
                    for (auto e : *iUnion)
                    {
                        result += toString(e) + " ";
                    }
                    return result.substr(0, result.size()-1) + ")"; 
                },
                pattern | as<SetOp>(as<Intersection>(iIntersection))                                = [&]{
                    std::string result = "(Intersection ";
                    for (auto e : *iUnion)
                    {
                        result += toString(e) + " ";
                    }
                    return result.substr(0, result.size()-1) + ")"; 
                },
                pattern | as<SetOp>(as<Difference>(iDifference))                                = [&]{
                    std::string result = "(Difference ";
                    for (auto e : *iDifference)
                    {
                        result += toString(e) + " ";
                    }
                    return result.substr(0, result.size()-1) + ")"; 
                },
                pattern | _ = [&]
                {
                    throw std::logic_error{"Mismatch in toString!"};
                    return "";
                }
            // clang-format on
        );
    }

    bool freeOf(ExprPtr const &ex, ExprPtr const &var)
    {
        constexpr auto firstOrSecond = [](auto &&p)
        { return or_(ds(p, _), ds(_, p)); };
        Id<ExprPtrMap> iMap;
        Id<PieceWise> iPieceWise;
        return match(ex)(
            pattern | var = expr(false),
            pattern | some(or_(as<Sum>(iMap), as<Product>(iMap))) = [&]
            { return std::all_of((*iMap).begin(), (*iMap).end(), [&](auto &&e)
                                 { return freeOf(e.second, var); }); },
            pattern | some(as<Power>(firstOrSecond(var))) = expr(false),
            pattern | some(as<Log>(firstOrSecond(var))) = expr(false),
            pattern | some(as<Sin>(ds(var))) = expr(false),
            pattern | some(as<Arctan>(ds(var))) = expr(false),
            pattern | some(as<PieceWise>(iPieceWise)) = [&]
            {
                for (auto const& e : *iPieceWise)
                {
                    if (!freeOf(e.first, var))
                    {
                        return false;
                    }
                }
                return true;
            },
            pattern | _ = expr(true));
    }

    ExprPtr substituteImpl(ExprPtr const &ex, ExprPtrMap const &srcDstMap)
    {
#if VERBOSE_DEBUG
        std::cout << "substituteImpl: " << toString(ex) << std::endl;
#endif // VERBOSE_DEBUG
        auto const iter = srcDstMap.find(ex);
        if (iter != srcDstMap.end())
        {
            return iter->second;
        }

        // using ExprVariant = std::variant<Integer, Fraction, Symbol, Pi, E, I, Sum, Product, Power, Log, Sin, Arctan, Set, List, Relational, PieceWise>;
        auto const subs = [&](auto &&e)
        { return substituteImpl(e, srcDstMap); };
        Id<Sum> iSum;
        Id<Product> iProduct;
        Id<ExprPtr> iE1, iE2;
        return match(ex)(
            pattern | some(as<Sum>(iSum)) = [&]
            {
                return std::accumulate((*iSum).begin(), (*iSum).end(), 0_i, [&](auto &&sum, auto &&e)
                                       {
                                           auto elem = subs(e.second);
                                           return sum + elem;
                                       });
            },
            pattern | some(as<Product>(iProduct)) = [&]
            {
                return std::accumulate((*iProduct).begin(), (*iProduct).end(), 1_i, [&](auto &&product, auto &&e)
                                       {
                                           auto elem = subs(e.second);
                                           return product * elem;
                                       });
            },
            pattern | some(as<Power>(ds(iE1, iE2))) = [&]
            { return pow(subs(*iE1), subs(*iE2)); },
            pattern | some(as<Log>(ds(iE1, iE2))) = [&]
            { return log(subs(*iE1), subs(*iE2)); },
            pattern | some(as<Sin>(ds(iE1))) = [&]
            { return sin(subs(*iE1)); },
            pattern | some(as<Arctan>(ds(iE1))) = [&]
            { return arctan(subs(*iE1)); },
            pattern | _ = expr(ex));
    }

    ExprPtr substitute(ExprPtr const &ex, ExprPtr const &srcDstPairs)
    {
#if VERBOSE_DEBUG
        std::cout << "substitute: " << toString(ex) << " " << toString(srcDstPairs) << std::endl;
#endif // VERBOSE_DEBUG
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
            {
                throw std::runtime_error("Mismatch in substitute!");
                return ExprPtrMap{};
            });
        return substituteImpl(ex, subMap);
    }

} // namespace mathiu::impl
