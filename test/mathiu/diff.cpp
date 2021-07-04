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

