#include "mathiu.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(functionRange, 1)
{
    auto const range = functionRange("x"_s + 1_i, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{1_i, true}, {2_i, true}}));
    EXPECT_EQ(toString(range), "(CCInterval 2 3)");
}

TEST(functionRange, 2)
{
    auto const range = functionRange(-2_i * "x"_s + 1_i, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{1_i, true}, {2_i, true}}));
    EXPECT_EQ(toString(range), "(CCInterval -3 -1)");
}

TEST(functionRange, 3)
{
    auto const range = functionRange(-2_i * (("x"_s - 1_i) ^ 2_i), "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-1_i, true}, {2_i, true}}));
    EXPECT_EQ(toString(range), "(CCInterval -8 0)");
}

TEST(functionRange, 4)
{
    auto const e = max("x"_s - 1_i, 3_i - "x"_s);
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-1_i, true}, {3_i, true}}));
    EXPECT_EQ(toString(range), "(CCInterval 1 4)");
}
