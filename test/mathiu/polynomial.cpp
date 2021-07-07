#include "mathiu.h"
#include "polynomial.h"
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