#include "mathiu.h"
#include "gtest/gtest.h"
#include <iostream>
using namespace mathiu;

TEST(Simplification, power)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const e = (x*y)*((x*y)^n2);
    EXPECT_EQ(toString(e), "(* (^ x 3) (^ y 3))");
}

TEST(Simplification, quotient)
{
    auto const a = symbol("a");
    auto const x = symbol("x");
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const e = (a * (x ^ n3)) / x;
    EXPECT_EQ(toString(e), "(* a (^ x 2))");
}

TEST(Simplification, distributive)
{
    auto const x = symbol("x");
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const e1 = x + n2 * x;
    auto const e2 = n3 * x;
    EXPECT_EQ(toString(e1), "(* 3 x)");
    EXPECT_EQ(toString(e2), "(* 3 x)");
    EXPECT_TRUE(mathiu::impl::equal(e1, e2));
}

TEST(asCoeffAndRest, 1)
{
    using namespace matchit;
    Id<mathiu::impl::ExprPtr> coeff;
    auto result = match(*(symbol("x")))
    (
        pattern | mathiu::impl::asCoeffAndRest(coeff, _) = [&] { return mathiu::impl::equal((*coeff), 1_i); },
        pattern | _ = expr(false)
    );
    EXPECT_TRUE(result);
}

TEST(asCoeffAndRest, 2)
{
    auto const e = symbol("x")^2_i;

    using namespace matchit;
    Id<mathiu::impl::ExprPtr> coeff;
    auto result = match(*e)
    (
        pattern | mathiu::impl::asCoeffAndRest(coeff, _) = [&] { return mathiu::impl::equal((*coeff), 1_i); },
        pattern | _ = expr(false)
    );
    EXPECT_TRUE(result);
}

TEST(asCoeffAndRest, 3)
{
    using namespace matchit;
    Id<mathiu::impl::ExprPtr> coeff;
    auto result = match(*(5_i * symbol("x")))
    (
        pattern | mathiu::impl::asCoeffAndRest(coeff, _) = [&] { return mathiu::impl::equal((*coeff), 5_i); },
        pattern | _ = expr(false)
    );
    EXPECT_TRUE(result);
}

TEST(asCoeffAndRest, 4)
{
    using namespace matchit;
    Id<mathiu::impl::ExprPtr> rest;
    auto result = match(*(5_i * symbol("x")))
    (
        pattern | mathiu::impl::asCoeffAndRest(_, rest) = [&] { return mathiu::impl::equal((*rest), symbol("x")); },
        pattern | _ = expr(false)
    );
    EXPECT_TRUE(result);
}

TEST(asCoeffAndRest, multiple)
{
    using namespace matchit;
    Id<mathiu::impl::ExprPtr> coeff1, coeff2, rest;
    auto result = match(*(5_i * symbol("x")), *symbol("x"))
    (
        pattern | ds(mathiu::impl::asCoeffAndRest(coeff1, rest), mathiu::impl::asCoeffAndRest(coeff2, rest)) = [&] { return mathiu::impl::equal((*rest), symbol("x")); },
        pattern | _ = expr(false)
    );
    EXPECT_TRUE(result);
}

TEST(asBaseAndExp, 1)
{
    using namespace matchit;
    Id<mathiu::impl::ExprPtr> base;
    Id<mathiu::impl::ExprPtr> exp;

    auto result = match(*symbol("x"))
    (
        pattern | mathiu::impl::asBaseAndExp(base, exp) = [&] { return mathiu::impl::equal((*base), symbol("x")) && mathiu::impl::equal((*exp), 1_i); },
        pattern | _ = expr(false)
    );
    EXPECT_TRUE(result);
}

TEST(asBaseAndExp, 2)
{
    using namespace matchit;
    Id<mathiu::impl::ExprPtr> base;
    Id<mathiu::impl::ExprPtr> exp;

    auto result = match(*(symbol("x")^2_i))
    (
        pattern | mathiu::impl::asBaseAndExp(base, exp) = [&] { return mathiu::impl::equal((*base), symbol("x")) && mathiu::impl::equal((*exp), 2_i); },
        pattern | _ = expr(false)
    );
    EXPECT_TRUE(result);
}

TEST(Simplification, mul2Pow)
{
    auto const x = symbol("x");
    EXPECT_EQ(toString(x * x), "(^ x 2)");
    EXPECT_EQ(toString(x * x * x), "(^ x 3)");
}

TEST(Simplification, distributive2_1)
{
    auto const n2 = 2_i;
    auto const x = symbol("x");
    auto const n3 = 3_i;
    auto const e = n2 * x + n3 * x;
    EXPECT_EQ(toString(e), "(* 5 x)");
}

TEST(Simplification, distributive2_2)
{
    auto const n2 = 2_i;
    auto const x = symbol("x");
    auto const f3o2 = fraction(3, 2);
    auto const e = n2 * x + f3o2 * x;
    EXPECT_EQ(toString(e), "(* 7/2 x)");
}

TEST(simplifyRational, int)
{
    auto const n2 = 2_i;
    auto const n3 = 3_i;

    EXPECT_EQ(toString(n2 + n3), "5");
}

TEST(simplifyRational, rational_)
{
    auto const f7o2 = fraction(14, 4);

    EXPECT_EQ(toString(mathiu::impl::simplifyRational(f7o2)), "7/2");
}

TEST(simplifyRational, rational)
{
    auto const n2 = 2_i;
    auto const f3o2 = fraction(3, 2);

    EXPECT_EQ(toString(n2 + f3o2), "7/2");
}

TEST(simplifyRational, rational2)
{
    auto const f3o2 = fraction(3, 2);

    EXPECT_EQ(toString(f3o2 + f3o2), "3");
}


TEST(asCoeffAndRest, rational)
{
    auto const n2 = 2_i;
    auto const x = symbol("x");
    auto const f3o2 = fraction(3, 2);

    using namespace matchit;
    Id<mathiu::impl::ExprPtr> coeff1, coeff2, rest;
    auto result = match(*(n2 * x), *(f3o2 * x))
    (
        pattern | ds(mathiu::impl::asCoeffAndRest(coeff1, rest), mathiu::impl::asCoeffAndRest(coeff2, rest))= [&]
        {
            ((*coeff1) + (*coeff2)); // * (*rest);
            return mathiu::impl::equal((*coeff1), n2) && mathiu::impl::equal((*coeff2), f3o2) && mathiu::impl::equal((*rest), symbol("x")); },
        pattern | _ = expr(false)
    );
    EXPECT_TRUE(result);
}


TEST(Simplification, distributive2)
{
    auto const n2 = 2_i;
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const f3o2 = fraction(3, 2);
    auto const e1 = n2 * x;
    auto const e = n2 * x + y + f3o2 * x;
    EXPECT_EQ(toString(e), "(+ (* 7/2 x) y)");
}

TEST(asCoeffAndRest, product)
{
    auto const x = symbol("x");
    auto const f3o2 = fraction(3, 2);

    using namespace matchit;
    Id<mathiu::impl::ExprPtr> coeff, rest;
    auto result = match(*(f3o2 * x))
    (
        pattern | mathiu::impl::asCoeffAndRest(coeff, rest) = [&] { return mathiu::impl::equal((*coeff), f3o2) && mathiu::impl::equal((*rest), symbol("x")); },
        pattern | _ = expr(false)
    );
    EXPECT_TRUE(result);
}

TEST(asCoeffAndRest, product2)
{
    auto const x = symbol("x");
    auto const f3o2 = fraction(3, 2);

    using namespace matchit;
    auto result = match(*(f3o2 * x))
    (
        pattern | as<mathiu::impl::Product>(ds(ds(f3o2, f3o2), ds(x, x))) = [&] { return true; },
        pattern | _ = expr(false)
    );
    EXPECT_TRUE(result);
}

TEST(asCoeffAndRest, sum)
{
    auto const x = symbol("x");
    auto const f3o2 = fraction(3, 2);
    auto const y = symbol("y");

    using namespace matchit;
    Id<mathiu::impl::ExprPtr> e;
    auto result = match(*(y + f3o2 * x))(
        pattern | as<mathiu::impl::Sum>(
                      ds(
                          ds(
                              _,
                              some(
                                  as<mathiu::impl::Product>(ds(ds(f3o2, f3o2), ds(x, x))))),
                          ds(y, y))) = [&]
        { return true; },
        pattern | _ = expr(false));
    EXPECT_TRUE(result);
}

TEST(asCoeffAndRest, sumCompare)
{
    auto const n2 = 2_i;
    auto const x = symbol("x");
    auto const y = symbol("y");

    auto const e = n2 * x + y;
    auto const sum = std::get<mathiu::impl::Sum>(*e);

    EXPECT_NE(sum.find(x), sum.end());
}


TEST(asCoeffAndRest, sum2)
{
    auto const n2 = 2_i;
    auto const x = symbol("x");
    auto const y = symbol("y");

    auto const e = n2 * x + y;

    EXPECT_EQ(toString(e), "(+ (* 2 x) y)");

    using namespace matchit;
    Id<mathiu::impl::ExprPtr> ie;
    auto result = match(*(e))(
        pattern | as<mathiu::impl::Sum>(
                      ds(
                          ds(
                              ie,
                              some(
                                  as<mathiu::impl::Product>(ds(ds(n2, n2), ds(x, x))))),
                          ds(y, y))) = [&]
        {
            EXPECT_EQ(toString(*ie), "x");
            return true;
        },
        pattern | _ = expr(false));
    EXPECT_TRUE(result);
}

TEST(asCoeffAndRest, sum3)
{
    auto const n2 = 2_i;
    auto const x = symbol("x");
    auto const f3o2 = fraction(3, 2);
    auto const y = symbol("y");

    auto const e = n2 * x + y + f3o2 * x;

    EXPECT_EQ(toString(e), "(+ (* 7/2 x) y)");

    using namespace matchit;
    Id<mathiu::impl::ExprPtr> ie;
    auto result = match(*(e))(
        pattern | as<mathiu::impl::Sum>(
                      ds(
                          ds(
                              ie,
                              some(
                                  as<mathiu::impl::Product>(ds(ds(fraction(7, 2), fraction(7, 2)), ds(x, x))))),
                          ds(y, y))) = [&]
        {
            EXPECT_EQ(toString(*ie), "x");
            return true;
        },
        pattern | _ = expr(false));
    EXPECT_TRUE(result);
}

TEST(autoSimplification, nonRationalCoeff)
{
    auto const x = symbol("x");
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const sqrt2 = n2 ^ fraction(1, 2);
    auto const e = n2 * x + n3 * x + sqrt2 * x;
    EXPECT_EQ(toString(e), "(+ (* 5 x) (* (^ 2 1/2) x))");
}

TEST(autoSimplification, sumSin)
{
    auto const x = symbol("x");
    auto const sinX = sin(x);
    auto const n2 = 2_i;
    auto const e1 = sinX;
    auto const e2 = n2 * sinX;
    EXPECT_EQ(toString(coeffAndTerm(*e1).second), toString(sinX));
    EXPECT_EQ(toString(coeffAndTerm(*e2).second), toString(sinX));
    EXPECT_TRUE(mathiu::impl::equal(coeffAndTerm(*e1).second, sinX));
    EXPECT_TRUE(mathiu::impl::equal(coeffAndTerm(*e2).second, sinX));
    auto const e = e1 + e2;
    EXPECT_EQ(toString(e), "(* 3 (Sin x))");
}

TEST(autoSimplification, cancel)
{
    auto const x = symbol("x");
    auto const n1 = 1_i;
    auto const nm1 = -1_i;
    auto const e = x + n1 + nm1 * (x + n1);
    EXPECT_EQ(toString(e), "(+ 1 x (* -1 (+ 1 x)))"); // should be 0
}

TEST(autoSimplification, associative)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const e = x + (x + y);
    EXPECT_EQ(toString(e), "(+ (* 2 x) y)");
}

TEST(autoSimplification, associative1)
{
    auto const w = symbol("w");
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const z = symbol("z");
    auto const e = (w + x) + (y + z);
    EXPECT_EQ(toString(e), "(+ w x y z)");
}

TEST(autoSimplification, associative2)
{
    auto const w = symbol("w");
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const z = symbol("z");
    auto const e = (((w * x) * y) * z);
    EXPECT_EQ(toString(e), "(* w x y z)");
}

TEST(autoSimplification, associative3)
{
    auto const n1 = 1_i;
    auto const n2 = 2_i;
    auto const x = symbol("x");
    auto const e = (n1 + x) + n2 * (x + n1);
    EXPECT_EQ(toString(e), "(+ 1 x (* 2 (+ 1 x)))");
}

TEST(autoSimplification, associative4)
{
    auto const u = symbol("u");
    auto const v = symbol("v");
    auto const w = symbol("w");
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const z = symbol("z");
    auto const e = ((((u + v) * w) * x) + y) + z;
    EXPECT_EQ(toString(e), "(+ (* (+ u v) w x) y z)");
}

TEST(autoSimplification, associative5)
{
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const n4 = 4_i;
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const z = symbol("z");
    auto const e = n2 * (x * y * z) + n3 * x * (y * z) + n4 * (x * y) * z;
    EXPECT_EQ(toString(e), "(* 9 x y z)");
}

TEST(autoSimplification, commutative)
{
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const n4 = 4_i;
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const z = symbol("z");
    auto const e = n2 * z * x * y + n3 * y * x * z + n4 * x * z * y;
    EXPECT_EQ(toString(e), "(* 9 x y z)");
}

TEST(autoSimplification, commutative2)
{
    auto const n3 = 3_i;
    auto const x = symbol("x");
    auto const a = symbol("a");
    auto const e = x * n3 * a;
    EXPECT_EQ(toString(e), "(* 3 a x)");
}

TEST(autoSimplification, commutative3)
{
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const n4 = 4_i;
    auto const x = symbol("x");
    auto const e = (x ^ n3) + (x ^ n2) + (x ^ n4);
    EXPECT_EQ(toString(e), "(+ (^ x 2) (^ x 3) (^ x 4))");
}

TEST(autoSimplification, commutative4)
{
    auto const n1 = 1_i;
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const e = n3 * ((x ^ n2) + n2) * y * (x ^ n3) * ((x ^ n2) + n1);
    EXPECT_EQ(toString(e), "(* 3 (^ x 3) (+ 1 (^ x 2)) (+ 2 (^ x 2)) y)");
}

TEST(autoSimplification, power)
{
    auto const n2 = 2_i;
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const e = (x * y) * (( x * y) ^ n2);
    EXPECT_EQ(toString(e), "(* (^ x 3) (^ y 3))");
}

TEST(autoSimplification, power1)
{
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const x = symbol("x");
    auto const e = (x ^ n2) * (x ^ n3);
    EXPECT_EQ(toString(e), "(^ x 5)");
}

TEST(autoSimplification, power2)
{
    auto const f1o2 = fraction(1, 2);
    auto const f1o3 = fraction(1, 3);
    auto const x = symbol("x");
    auto const e = (x ^ f1o2) * (x ^ f1o3);
    EXPECT_EQ(toString(e), "(^ x 5/6)");
}

TEST(autoSimplification, power3)
{
    auto const x = symbol("x");
    auto const a = symbol("a");
    auto const b = symbol("b");
    auto const e = (x ^ a) * (x ^ b);
    EXPECT_EQ(toString(e), "(^ x (+ a b))");
}

TEST(autoSimplification, power4)
{
    auto const x = symbol("x");
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const e = (x ^ n2) ^ n3;
    EXPECT_EQ(toString(e), "(^ x 6)");
}

TEST(autoSimplification, power5)
{
    auto const x = symbol("x");
    auto const a = symbol("a");
    auto const n2 = 2_i;
    auto const e = (x ^ a) ^ n2;
    EXPECT_EQ(toString(e), "(^ x (* 2 a))");
}

TEST(autoSimplification, power6)
{
    auto const x = symbol("x");
    auto const n2 = 2_i;
    auto const f1o2 = fraction(1, 2);
    auto const e = (x ^ n2) ^ f1o2;
    EXPECT_EQ(toString(e), "(^ (^ x 2) 1/2)");
}

TEST(autoSimplification, power7)
{
    auto const x = symbol("x");
    auto const n2 = 2_i;
    auto const f1o2 = fraction(1, 2);
    auto const e1 = (x ^ f1o2);
    EXPECT_EQ(toString(e1), "(^ x 1/2)");
    auto const e = e1 ^ n2;
    EXPECT_EQ(toString(e), "x");
}

TEST(autoSimplification, power8)
{
    auto const x = symbol("x");
    auto const n2 = 2_i;
    auto const a = symbol("a");
    auto const e = (x ^ n2) ^ a;
    EXPECT_EQ(toString(e), "(^ (^ x 2) a)");
}

TEST(autoSimplification, power9)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const n2 = 2_i;
    auto const e = (x * y) ^ n2;
    EXPECT_EQ(toString(e), "(* (^ x 2) (^ y 2))");
}

TEST(autoSimplification, power10)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const f1o3 = fraction(1, 3);
    auto const e = (x * y) ^ f1o3;
    EXPECT_EQ(toString(e), "(^ (* x y) 1/3)");
}

TEST(autoSimplification, power11)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const a = symbol("a");
    auto const e = (x * y) ^ a;
    EXPECT_EQ(toString(e), "(^ (* x y) a)");
}

TEST(autoSimplification, power12)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const f1o2 = fraction(1, 2);
    auto const e = (x * y) * ((x * y) ^ f1o2);
    EXPECT_EQ(toString(e), "(* x y (^ (* x y) 1/2))");
}

TEST(autoSimplification, difference)
{
    auto const u = symbol("u");
    auto const e = -u;
    EXPECT_EQ(toString(e), "(* -1 u)");
}

TEST(autoSimplification, difference1)
{
    auto const u = symbol("u");
    auto const v = symbol("v");
    auto const e = u - v;
    EXPECT_EQ(toString(e), "(+ u (* -1 v))");
}

TEST(autoSimplification, quotient)
{
    auto const u = symbol("u");
    auto const v = symbol("v");
    auto const e = u / v;
    EXPECT_EQ(toString(e), "(* u (^ v -1))");
}

TEST(autoSimplification, quotient1)
{
    auto const a = symbol("a");
    auto const x = symbol("x");
    auto const n3 = 3_i;
    auto const e = (a * (x ^ n3)) / x;
    EXPECT_EQ(toString(e), "(* a (^ x 2))");
}

TEST(autoSimplification, identity)
{
    auto const u = symbol("u");
    auto const n0 = 0_i;
    auto const e = u + n0;
    EXPECT_EQ(toString(e), "u");
}

TEST(autoSimplification, identity1)
{
    auto const u = symbol("u");
    auto const n0 = 0_i;
    auto const e = u * n0;
    EXPECT_EQ(toString(e), "0");
}

TEST(autoSimplification, identity2)
{
    auto const u = symbol("u");
    auto const n1 = 1_i;
    auto const e = u * n1;
    EXPECT_EQ(toString(e), "u");
}

TEST(autoSimplification, identity3)
{
    auto const n0 = 0_i;
    auto const f2o3 = fraction(2, 3);
    auto const e = n0 ^ f2o3;
    EXPECT_EQ(toString(e), "0");
}

TEST(autoSimplification, identity4)
{
    auto const n0 = 0_i;
    auto const w = symbol("w");
    EXPECT_THROW(n0 ^ w, std::runtime_error);
}

TEST(autoSimplification, identity5)
{
    auto const n1 = 1_i;
    auto const w = symbol("w");
    auto const e = n1 ^ w;
    EXPECT_EQ(toString(e), "1");
}

TEST(autoSimplification, identity6)
{
    auto const v = symbol("v");
    auto const n0 = 0_i;
    EXPECT_EQ(toString(v ^ n0), "1");
}

TEST(autoSimplification, identity7)
{
    auto const n0 = 0_i;
    EXPECT_THROW(n0 ^ n0, std::runtime_error);
}

TEST(autoSimplification, identity8)
{
    auto const v = symbol("v");
    auto const n1 = 1_i;
    auto const e = v ^ n1;
    EXPECT_EQ(toString(e), "v");
}

TEST(autoSimplification, numerical)
{
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const e = n2 ^ n3;
    EXPECT_EQ(toString(e), "8");
}

TEST(autoSimplification, rational)
{
    auto const n2 = 2_i;
    auto const n3 = 3_i;
    auto const e = n2 / (-n3);
    EXPECT_EQ(toString(e), "-2/3");
}

TEST(autoSimplification, power13)
{
    auto const x = symbol("x");
    auto const f1o2 = fraction(1, 2);
    auto const n8 = 8_i;
    auto const e = (((x^f1o2)^f1o2)^n8);
    EXPECT_EQ(toString(e), "(^ x 2)");
}

TEST(autoSimplification, power14)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const z = symbol("z");
    auto const f1o2 = fraction(1, 2);
    auto const n2 = 2_i;
    auto const e = ((((x*y)^f1o2)*(z^n2))^n2);
    EXPECT_EQ(toString(e), "(* x y (^ z 4))");
}

TEST(autoSimplification, product)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const e = x * y / x;
    // auto const e = x / x * y;
    EXPECT_EQ(toString(e), "y");
}

TEST(autoSimplification, log)
{
    auto const i2 = 2_i;
    auto const i1 = 1_i;
    auto const x = "x"_s;
    auto const lnx = log(e, x);
    // auto const u = i2 * lnx * x / lnx;
    auto const u = lnx / lnx;
    EXPECT_EQ(toString(u), "1");
}

TEST(autoSimplification, 1)
{
    auto const e = 2_i + 3_i/4_i + 5_i/6_i;
    EXPECT_EQ(toString(e), "43/12");
}

TEST(autoSimplification, 2)
{
    auto const e = "x"_s + "y"_s + 2_i * "x"_s;
    EXPECT_EQ(toString(e), "(+ (* 3 x) y)");
}

TEST(autoSimplification, 3)
{
    auto const e = "x"_s * "y"_s * ("x"_s ^ 2_i);
    EXPECT_EQ(toString(e), "(* (^ x 3) y)");
}

TEST(autoSimplification, 4)
{
    auto const e = 1_i * ("x"_s ^ 3_i) + "a"_s * ("x"_s ^ 0_i) + "b"_s * ("x"_s ^ 1_i) + 0_i * ("x"_s ^ 2_i);
    EXPECT_EQ(toString(e), "(+ a (* b x) (^ x 3))");
}

TEST(autoSimplification, 5)
{
    auto const e = "x"_s * "y"_s + 3_i * "y"_s * "x"_s;
    EXPECT_EQ(toString(e), "(* 4 x y)");
}
