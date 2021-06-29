#include "mathia.h"
#include "gtest/gtest.h"
using namespace mathia;

TEST(Simplification, power)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const n2 = constant(2);
    auto const n3 = constant(3);
    auto const e1 = (x*y)*((x*y)^n2);
    auto const e2 = (x^n3)*(y^n3);
    EXPECT_EQ(toString(e1), "(* 1 x (^ x 2) y (^ y 2))"); // need to be simplified with basic distributive transformation
    EXPECT_EQ(toString(e2), "(* (^ x 3) (^ y 3))");
}

TEST(Simplification, quotient)
{
    auto const a = symbol("a");
    auto const x = symbol("x");
    auto const n2 = constant(2);
    auto const n3 = constant(3);
    auto const e1 = (a * (x ^ n3)) / x;
    auto const e2 = a * (x^n2);
    EXPECT_EQ(toString(e1), "(* a (^ x -1) (^ x 3))");
    EXPECT_EQ(toString(e2), "(* a (^ x 2))");
}


