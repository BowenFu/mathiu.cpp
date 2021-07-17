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

TEST(functionRange, 3_)
{
    auto const range = functionRange(-2_i * (("x"_s - 1_i) ^ 2_i), "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{2_i, true}, {3_i, true}}));
    EXPECT_EQ(toString(range), "(CCInterval -8 -2)");
}

TEST(functionRange, 4)
{
    auto const e = max("x"_s - 1_i, 3_i - "x"_s);
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-1_i, true}, {3_i, true}}));
    EXPECT_EQ(toString(range), "(CCInterval 1 4)");
}

TEST(functionRange, 5)
{
    auto const e = max("x"_s - 1_i, 3_i - "x"_s);
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-1_i, false}, {3_i, false}}));
    EXPECT_EQ(toString(range), "(COInterval 1 4)");
}

TEST(functionRange, 6)
{
    auto const e = ("x"_s - 1_i)^2_i;
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-2_i, false}, {2_i, false}}));
    EXPECT_EQ(toString(range), "(COInterval 0 9)");
}

TEST(functionRange, 7)
{
    auto const e = (("x"_s + 1_i)^2_i) - 1_i;
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-2_i, false}, {2_i, false}}));
    EXPECT_EQ(toString(range), "(COInterval -1 8)");
}

TEST(functionRange, 8)
{
    auto const e = max(("x"_s - 1_i)^2_i, (("x"_s + 1_i)^2_i) - 1_i);
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-2_i, false}, {2_i, false}}));
    EXPECT_EQ(toString(range), "(COInterval 9/16 9)");
}

TEST(functionRange, 9)
{
    auto const e = max(("x"_s - 1_i)^2_i, -(("x"_s + 1_i)^2_i) - 1_i);
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-2_i, false}, {2_i, false}}));
    EXPECT_EQ(toString(range), "(COInterval 0 9)");
}

TEST(functionRange, 10)
{
    auto const e = max(("x"_s - 1_i)^2_i, -(("x"_s + 1_i)^2_i) + 1_i);
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-2_i, false}, {2_i, false}}));
    EXPECT_EQ(toString(range), "(COInterval 0 9)");
}

TEST(functionRange, 11)
{
    auto const e = min(("x"_s - 1_i)^2_i, -(("x"_s + 1_i)^2_i) + 1_i);
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-2_i, false}, {2_i, false}}));
    EXPECT_EQ(toString(range), "(OCInterval -8 1)");
}

TEST(functionRange, 12)
{
    auto const e = min(("x"_s - 1_i)^2_i, -(("x"_s + 1_i)^2_i) + 6_i);
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-2_i, false}, {2_i, false}}));
    // SIMPLIFY ME, i.e. (-3, 3+2sqrt(2)]
    EXPECT_EQ(toString(range), "(Union (CCInterval 0 (^ (+ -1 (* -1/4 (^ 32 1/2))) 2)) (Union (OOInterval 5 (+ 6 (* -1 (^ (+ 1 (* -1/4 (^ 32 1/2))) 2)))) (OOInterval -3 (+ 6 (* -1 (^ (+ 1 (* 1/4 (^ 32 1/2))) 2))))))");
}

TEST(functionRange, 13)
{
    auto const e = max(("x"_s^2_i)-1_i, 1_i-("x"_s^2_i));
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-2_i, false}, {2_i, false}}));
    EXPECT_EQ(toString(range), "(COInterval 0 3)");
}

TEST(functionRange, 14)
{
    auto const e = min(("x"_s^2_i)-1_i, 1_i-("x"_s^2_i));
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-2_i, false}, {2_i, false}}));
    EXPECT_EQ(toString(range), "(OCInterval -3 0)");
}

TEST(functionRange, 15)
{
    auto const e = max(("x"_s ^ 2_i) - 1_i, 3_i - "x"_s);
    auto const range = functionRange(e, "x"_s, std::make_shared<impl::Expr const>(impl::Interval{{-1_i, true}, {3_i, true}}));
    EXPECT_EQ(toString(range), "(CCInterval (+ -1 (* 1/4 (^ (+ -1 (^ 17 1/2)) 2))) 8)");
}

TEST(functionRange, 15_1)
{
    auto const e = ("x"_s ^ 2_i) - 1_i;
    auto const lPoint = (1_i / 2_i) * (-1_i + sqrt(17_i));
    auto const domain = std::make_shared<impl::Expr const>(impl::Interval{{lPoint, true}, {3_i, true}});
    auto const range = functionRange(e, "x"_s, domain);
    EXPECT_EQ(toString(range), "(CCInterval (+ -1 (* 1/4 (^ (+ -1 (^ 17 1/2)) 2))) 8)");
}
