#include "solve.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

#if 0
TEST(solve, integer)
{
    auto const x = symbol("x");
    auto const e = integer(2);
    EXPECT_EQ(toString(solve(x - e, x)), "{2}");
    EXPECT_EQ(toString(solve(e - x, x)), "{2}");
}

TEST(solve, freeOf)
{
    auto const x = symbol("x");
    auto const e = integer(2)^symbol("a");
    EXPECT_EQ(toString(solve(e - x, x)), "{(^ 2 a)}");
    EXPECT_EQ(toString(solve(x - e, x)), "{(^ 2 a)}");
}
#endif // 0

TEST(solve, alwaysHold)
{
    auto const x = symbol("x");
    auto const e = integer(2)^symbol("a");
    EXPECT_EQ(toString(solve(e - e, x)), "{x}");
}

TEST(solve, alwaysFail)
{
    auto const x = symbol("x");
    auto const e1 = integer(2)^symbol("a");
    auto const e2 = integer(3)^symbol("a");
    EXPECT_EQ(toString(solve(e1 - e2, x)), "{}");
}

// --- solve single

TEST(solve, zero)
{
    auto const x = symbol("x");
    auto const e = integer(0);
    EXPECT_EQ(toString(solve(e, x)), "{x}");
}

TEST(solve, nonZero)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    EXPECT_EQ(toString(solve(y, x)), "{}");
}

TEST(solve, product)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    EXPECT_EQ(toString(solve(y * x, x)), "{0}");
}
