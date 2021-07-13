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