#include "mathiu.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(substitute, 1)
{
    auto const e = substitute("x"_s * ("x"_s + "y"_s), "x"_s >> 2_i);
    EXPECT_EQ(toString(e), "(* 2 (+ 2 y))");
}

TEST(substitute, 2)
{
    auto const e = substitute("x"_s * ("x"_s + "y"_s), ("x"_s + "y"_s) >> 3_i);
    EXPECT_EQ(toString(e), "(* 3 x)");
}

