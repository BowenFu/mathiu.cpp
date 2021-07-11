#include "mathiu.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(functionRange, 1)
{
    auto const range = functionRange("x"_s + 1_i, "x"_s, impl::CCInterval{{1_i, 2_i}});
    EXPECT_EQ(toString(range.first), "2");
    EXPECT_EQ(toString(range.second), "3");
}

TEST(functionRange, 2)
{
    auto const range = functionRange(-2_i * "x"_s + 1_i, "x"_s, impl::CCInterval{{1_i, 2_i}});
    EXPECT_EQ(toString(range.first), "-3");
    EXPECT_EQ(toString(range.second), "-1");
}
