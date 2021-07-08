#include "mathiu.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(degree, product)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    EXPECT_EQ(toString(degree(y * x, x)), "1");
}

TEST(degree, power)
{
    auto const x = symbol("x");
    EXPECT_EQ(toString(degree(x * x, x)), "2");
}

TEST(degree, sum)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    EXPECT_EQ(toString(degree(x + y, x)), "1");
}

TEST(degree, multivar)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    EXPECT_EQ(toString(degree(x * y, set({x, y}))), "2");
}

TEST(degree, multivar2)
{
    auto const i2 = 2_i;
    auto const i3 = 3_i;
    auto const i4 = 4_i;
    auto const w = "w"_s;
    auto const x = "x"_s;
    auto const y = "y"_s;
    auto const z = "z"_s;
    EXPECT_EQ(toString(degree(i3 * w * (x^i2) * (y^i3) * (z^i4), set({x, z}))), "6");
}

TEST(degree, multivar3)
{
    auto const i2 = 2_i;
    auto const a = "a"_s;
    auto const b = "b"_s;
    auto const c = "c"_s;
    auto const x = "x"_s;
    EXPECT_EQ(toString(degree(a * (x^i2) + (b^i2) + c, x)), "2");
}

TEST(degree, multivar4)
{
    auto const i2 = 2_i;
    auto const a = "a"_s;
    auto const b = "b"_s;
    auto const c = "c"_s;
    auto const x = "x"_s;
    auto const sinx = sin(x);
    EXPECT_EQ(toString(degree(a * (sinx^i2) + (b*sinx) + c, sinx)), "2");
}

TEST(degree, multivarr)
{
    auto const i2 = 2_i;
    auto const i3 = 3_i;
    auto const i6 = 6_i;
    auto const w = "w"_s;
    auto const x = "x"_s;
    auto const y = "y"_s;
    auto const z = "z"_s;
    EXPECT_EQ(toString(degree(i2 * (x^i2) * y * (z^i3) + w * x * (z^i6), set({x, z}))), "7");
}

TEST(coefficient, 1)
{
    auto const i2 = 2_i;
    auto const a = "a"_s;
    auto const b = "b"_s;
    auto const c = "c"_s;
    auto const x = "x"_s;
    EXPECT_EQ(toString(coefficient(a * (x ^ i2) + b * x + c, x, 2)), "a");
}

TEST(coefficient, 2)
{
    auto const i2 = 2_i;
    auto const i3 = 3_i;
    auto const i5 = 5_i;
    auto const i7 = 7_i;
    auto const i9 = 9_i;
    auto const x = "x"_s;
    auto const y = "y"_s;
    auto const z = "z"_s;
    EXPECT_EQ(toString(coefficient(i3 * x * (y ^ i2) + i5 * (x^i2) * y + i7*x + i9, x, 1)), "(+ 7 (* 3 (^ y 2)))");
}

TEST(coefficient, 2_1)
{
    auto const i2 = 2_i;
    auto const i3 = 3_i;
    auto const i5 = 5_i;
    auto const i7 = 7_i;
    auto const i9 = 9_i;
    auto const x = "x"_s;
    auto const y = "y"_s;
    auto const z = "z"_s;
    EXPECT_EQ(toString(coefficient(x * i2, x, 1)), "2");
    EXPECT_EQ(toString(coefficient(x * y, x, 1)), "y");
    EXPECT_EQ(toString(coefficient(x * (y ^ i2), x, 1)), "(^ y 2)");
    EXPECT_EQ(toString(coefficient(i3 * x * (y ^ i2), x, 1)), "(* 3 (^ y 2))");
    EXPECT_EQ(toString(i3 * x * (y ^ i2) / x), "(* 3 (^ y 2))");
}

TEST(coefficient, 3)
{
    auto const i2 = 2_i;
    auto const i3 = 3_i;
    auto const i5 = 5_i;
    auto const i7 = 7_i;
    auto const i9 = 9_i;
    auto const x = "x"_s;
    auto const y = "y"_s;
    auto const z = "z"_s;
    EXPECT_EQ(toString(coefficient(i3 * x * (y ^ i2) + i5 * (x^i2) * y + i7*x + i9, x, 3)), "0");
}

TEST(coefficient, 4)
{
    auto const i2 = 2_i;
    auto const i3 = 3_i;
    auto const i5 = 5_i;
    auto const i7 = 7_i;
    auto const i9 = 9_i;
    auto const x = "x"_s;
    auto const y = "y"_s;
    auto const z = "z"_s;
    auto const u = i3 * x * (y ^ i2) + i5 * (x^i2) * y + i7*x + i9;
    EXPECT_EQ(toString(coefficient(coefficient(u, x, 1), y, 2)), "3");
}

TEST(coefficient, 5)
{
    auto const i2 = 2_i;
    auto const i3 = 3_i;
    auto const i4 = 4_i;
    auto const x = "x"_s;
    auto const sinx = sin(x);
    auto const lnx = log(e, x);
    auto const u = i3 * sinx * (x ^ i2) + i2 * lnx * x + i4;
    // FIXME, should throw exception.
    EXPECT_THROW(toString(coefficient(u, x, 2)), std::exception);
}

TEST(coefficient, 6)
{
    auto const i2 = 2_i;
    auto const i3 = 3_i;
    auto const i4 = 4_i;
    auto const x = "x"_s;
    auto const sinx = sin(x);
    auto const lnx = log(e, x);
    auto const u = i3 * sinx * (x ^ i2) + i2 * lnx * x + i4;
    // EXPECT_EQ(toString(coefficient(u, lnx, 1)), "(* 2 x)");
    EXPECT_EQ(toString(coefficient(coefficient(u, lnx, 1), x, 1)), "2");
}

TEST(coefficient, 7)
{
    auto const i2 = 2_i;
    auto const i3 = 3_i;
    auto const i5 = 5_i;
    auto const i7 = 7_i;
    auto const i9 = 9_i;
    auto const x = "x"_s;
    auto const y = "y"_s;
    auto const z = "z"_s;
    auto const u = i3 * x * (y ^ i2) + i5 * (x^i2) * y + i7* (x^i2) * (y^i3) + i9;
    EXPECT_EQ(toString(coefficient(u, x, 2)), "(+ (* 5 y) (* 7 (^ y 3)))");
}

TEST(coefficientList, 1)
{
    auto const a = "a"_s;
    auto const b = "b"_s;
    auto const c = "c"_s;
    auto const x = "x"_s;
    auto const u = a * (x ^ 2_i) + b * x + c;
    EXPECT_EQ(toString(coefficientList(u, x)), "[c b a]");
}
