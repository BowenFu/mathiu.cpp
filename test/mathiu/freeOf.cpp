#include "mathiu.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(freeOf, 1)
{
    EXPECT_FALSE(freeOf("a"_s + "b"_s, "b"_s));
}

TEST(freeOf, 2)
{
    EXPECT_TRUE(freeOf("a"_s + "b"_s, "c"_s));
}

TEST(freeOf, 3)
{
    EXPECT_FALSE(freeOf(("a"_s + "b"_s) * "c"_s, "a"_s + "b"_s));
}

TEST(freeOf, 4)
{
    EXPECT_FALSE(freeOf(sin("x"_s) + 2_i * "x"_s, sin("x"_s)));
}

TEST(freeOf, 5)
{
    EXPECT_TRUE(freeOf(("a"_s + "b"_s + "c"_s) * "d"_s, "a"_s + "b"_s));
}

TEST(freeOf, 6)
{
    EXPECT_TRUE(freeOf(("y"_s + 2_i * "x"_s - "y"_s) / "x"_s, "x"_s));
}
