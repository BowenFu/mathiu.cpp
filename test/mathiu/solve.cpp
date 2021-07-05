#include "solve.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(solve, integer)
{
    auto const x = symbol("x");
    auto const e = integer(2);
    EXPECT_EQ(toString(solve(x, e, x)), "2");
    EXPECT_EQ(toString(solve(e, x, x)), "2");
}

TEST(solve, freeOf)
{
    auto const x = symbol("x");
    auto const e = integer(2)^symbol("a");
    EXPECT_EQ(toString(solve(e, x, x)), "(^ 2 a)");
    EXPECT_EQ(toString(solve(x, e, x)), "(^ 2 a)");
}

