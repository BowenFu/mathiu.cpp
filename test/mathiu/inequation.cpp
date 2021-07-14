#include "mathiu.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(inequation, True)
{
    auto const e = 2_i < 3_i;
    EXPECT_EQ(toString(e), "true");
}

TEST(inequation, False)
{
    auto const e = 2_i > 3_i;
    EXPECT_EQ(toString(e), "false");
}

TEST(inequation, True1)
{
    auto const e = "x"_s == "x"_s;
    EXPECT_EQ(toString(e), "true");
}

TEST(inequation, True2)
{
    auto const e = "x"_s >= "x"_s;
    EXPECT_EQ(toString(e), "true");
}

TEST(inequation, False1)
{
    auto const e = "x"_s > "x"_s;
    EXPECT_EQ(toString(e), "false");
}

TEST(inequation, False2)
{
    auto const e = "x"_s < "x"_s;
    EXPECT_EQ(toString(e), "false");
}

TEST(inequation, Linear)
{
    auto const e = "x"_s * -1_i <= 2_i;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(>= x -2)");
}

TEST(inequation, Linear1)
{
    auto const e = "x"_s * -1_i > 2_i;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(< x -2)");
}

TEST(inequation, Square)
{
    auto const e = ("x"_s ^ 2_i) < 2_i;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(And (> x (* -1/2 (^ 8 1/2))) (< x (* 1/2 (^ 8 1/2))))"); // simplify me
}

TEST(inequation, Square1)
{
    auto const e = ("x"_s ^ 2_i) >= 2_i;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(Or (<= x (* -1/2 (^ 8 1/2))) (>= x (* 1/2 (^ 8 1/2))))"); // simplify me
}