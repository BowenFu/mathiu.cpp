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
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(COInterval -2 inf)");
}

TEST(inequation, Linear1)
{
    auto const e = "x"_s * -1_i > 2_i;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(OOInterval (* -1 inf) -2)");
}

TEST(inequation, Linear2)
{
    auto const e = 2_i >= "x"_s * -1_i;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(COInterval -2 inf)");
}

TEST(inequation, Linear3)
{
    auto const e = 2_i < "x"_s * -1_i;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(OOInterval (* -1 inf) -2)");
}

TEST(inequation, Square)
{
    auto const e = ("x"_s ^ 2_i) < 2_i;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(OOInterval (* -1/2 (^ 8 1/2)) (* 1/2 (^ 8 1/2)))"); // simplify me
}

TEST(inequation, Square1)
{
    auto const e = ("x"_s ^ 2_i) >= 2_i;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(Union (OCInterval (* -1 inf) (* -1/2 (^ 8 1/2))) (COInterval (* 1/2 (^ 8 1/2)) inf))"); // simplify me
}

TEST(inequation, Square2)
{
    auto const e = 2_i > -("x"_s ^ 2_i) ;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(Union (OOInterval (* -1 inf) (* (^ -8 1/2) -1 1/2)) (OOInterval (* (^ -8 1/2) 1/2) inf))"); // simplify me
}

TEST(inequation, Square3)
{
    auto const e = 2_i <= ("x"_s ^ 2_i) ;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(Union (OCInterval (* -1 inf) (* 1/2 (^ 8 1/2))) (COInterval (* 1/-2 (^ 8 1/2)) inf))"); // simplify me
}