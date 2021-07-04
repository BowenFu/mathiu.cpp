#include "diff.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(diff, integer)
{
    auto const x = symbol("x");
    auto const e = integer(2);
    EXPECT_EQ(toString(diff(e, x)), "0");
}

TEST(diff, symbol)
{
    auto const x = symbol("x");
    EXPECT_EQ(toString(diff(x, x)), "1");
}

TEST(diff, symbol2)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    EXPECT_EQ(toString(diff(y, x)), "0");
}

TEST(diff, sum)
{
    auto const x = symbol("x");
    auto const e = symbol("x") + integer(2);
    EXPECT_EQ(toString(diff(e, x)), "1");
}

TEST(diff, sum2)
{
    auto const x = symbol("x");
    auto const e = symbol("x") + symbol("y");
    EXPECT_EQ(toString(diff(e, x)), "1");
}

TEST(diff, product)
{
    auto const x = symbol("x");
    auto const e = symbol("x") * symbol("y");
    EXPECT_EQ(toString(diff(e, x)), "y");
}

TEST(diff, product2)
{
    auto const x = symbol("x");
    auto const e = symbol("x") * integer(2);
    EXPECT_EQ(toString(diff(e, x)), "2");
}

TEST(diff, power)
{
    auto const x = symbol("x");
    auto const e = x ^ integer(2);
    EXPECT_EQ(toString(diff(e, x)), "(* 2 x)");
}

TEST(diff, power2)
{
    auto const x = symbol("x");
    auto const e = x ^ fraction(2, 3);
    EXPECT_EQ(toString(diff(e, x)), "(* 2/3 (^ x -1/3))");
}

TEST(diff, compound)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const e = x * y * (x + integer(3));
    EXPECT_EQ(toString(diff(e, x)), "(+ (* x y) (* (+ 3 x) y))");
}