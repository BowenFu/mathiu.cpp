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

TEST(seqSubstitute, 1)
{
    auto const e = substitute("x"_s + "y"_s, "x"_s >> ("a"_s + 1_i), "y"_s >> ("b"_s + 2_i));
    EXPECT_EQ(toString(e), "(+ 3 a b)");
}

TEST(conSubstitute, 1)
{
    auto const e = substitute("x"_s + "y"_s, set("x"_s >> ("a"_s + 1_i), "y"_s >>("b"_s + 2_i)));
    EXPECT_EQ(toString(e), "(+ 3 a b)"); // FIXME
}

TEST(seqSubstitute, 2)
{
    auto const e = substitute(("a"_s + "b"_s) * "x"_s, ("a"_s + "b"_s) >> ("x"_s + "c"_s), ("x"_s >> "d"_s));
    EXPECT_EQ(toString(e), "(* d (+ c d))");
}

TEST(conSubstitute, 2)
{
    auto const e = substitute(("a"_s + "b"_s) * "x"_s, set(("a"_s + "b"_s) >> ("x"_s + "c"_s), ("x"_s >> "d"_s)));
    EXPECT_EQ(toString(e), "(* d (+ c x))");
}

TEST(seqSubstitute, 3)
{
    auto const e = substitute("x"_s * ("x"_s + "y"_s), "x"_s >> 2_i, ("x"_s + "y"_s) >> 3_i);
    EXPECT_EQ(toString(e), "(* 2 (+ 2 y))");
}

TEST(conSubstitute, 3)
{
    auto const e = substitute("x"_s * ("x"_s + "y"_s), set("x"_s >> 2_i, ("x"_s + "y"_s) >> 3_i));
    EXPECT_EQ(toString(e), "6");
}
