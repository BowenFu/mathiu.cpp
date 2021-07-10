#include "mathiu.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(relational, 1)
{
    auto const x = "x"_s;
    auto const y = "y"_s;
    EXPECT_EQ(toString(max(x, y)), "(PieceWise (x (>= x y)) (y (< x y)))");
}

TEST(relational, 2)
{
    auto const x = "x"_s;
    auto const e1 = x * 2_i;
    auto const e2 = 5_i - x;
    EXPECT_EQ(toString(max(e1, e2)), "(PieceWise ((* 2 x) (>= (* 2 x) (+ 5 (* -1 x)))) ((+ 5 (* -1 x)) (< (* 2 x) (+ 5 (* -1 x)))))");
}