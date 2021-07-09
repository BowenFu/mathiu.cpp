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