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
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(Union (COInterval (* 1/2 (^ 8 1/2)) inf) (OCInterval (* -1 inf) (* -1/2 (^ 8 1/2))))"); // simplify me
}

TEST(inequation, Square2)
{
    auto const e = 2_i > -("x"_s ^ 2_i) ;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "complexes");
}

TEST(inequation, Square3)
{
    auto const e = 2_i <= ("x"_s ^ 2_i) ;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(Union (COInterval (* 1/-2 (^ 8 1/2)) inf) (OCInterval (* -1 inf) (* 1/2 (^ 8 1/2))))"); // simplify me
}

TEST(inequation, Square4)
{
    auto const e1 = ("x"_s ^ 2_i) - 1_i;
    auto const e2 = -e1;
    auto const e = e1 <= e2;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(CCInterval (* -1/4 (^ 16 1/2)) (* 1/4 (^ 16 1/2)))");
}

TEST(inequation, Square5)
{
    auto const e1 = ("x"_s ^ 2_i) - 1_i;
    auto const e2 = -e1;
    auto const e = e1 > e2;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(Union (OOInterval (* 1/4 (^ 16 1/2)) inf) (OOInterval (* -1 inf) (* -1/4 (^ 16 1/2))))");
}

TEST(inequation, Square6)
{
    auto const e1 = ("x"_s ^ 2_i) - 1_i;
    auto const e2 = (("x"_s - 3_i) ^ 2_i) - 3_i;;
    auto const e = e1 > e2;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(OOInterval 7/6 inf)");
}

TEST(inequation, Square7)
{
    auto const e1 = ("x"_s ^ 2_i) - 1_i;
    auto const e2 = (2_i*("x"_s - 3_i) ^ 2_i) - 3_i;;
    auto const e = e1 > e2;
    EXPECT_EQ(toString(solveInequation(e, "x"_s)), "(OOInterval (* 1/-6 (+ -24 (* -1 (^ 168 1/2)))) (* 1/-6 (+ -24 (^ 168 1/2))))");
}

