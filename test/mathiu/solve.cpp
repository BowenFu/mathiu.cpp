#include "mathiu/solve.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(solvePoly, deg0)
{
    auto const x = symbol("x");
    EXPECT_EQ(toString(solvePoly(0_i, x)), "{x}");
    EXPECT_EQ(toString(solvePoly("y"_s, x)), "{}");
}

TEST(solvePoly, deg1)
{
    auto const x = symbol("x");
    auto const c = "c"_s;
    auto const b = "b"_s;
    EXPECT_EQ(toString(solvePoly(b * x + c, x)), "{(* -1 (^ b -1) c)}");
}

TEST(solvePoly, deg2)
{
    auto const x = symbol("x");
    auto const c = "c"_s;
    auto const b = "b"_s;
    auto const a = "a"_s;
    EXPECT_EQ(toString(solvePoly(a * (x ^ 2_i) + b * x + c, x)),
              "{(* 1/2 (^ a -1) (+ (* -1 b) (^ (+ (^ b 2) (* -4 a c)) 1/2))) "
              "(* 1/2 (^ a -1) (+ (* -1 b) (* -1 (^ (+ (^ b 2) (* -4 a c)) 1/2))))}");
}

TEST(solvePoly, freeOf)
{
    auto const x = symbol("x");
    auto const e = 2_i^symbol("a");
    EXPECT_EQ(toString(solvePoly(e - x, x)), "{(^ 2 a)}");
    EXPECT_EQ(toString(solvePoly(x - e, x)), "{(^ 2 a)}");
}

TEST(solve, alwaysHold)
{
    auto const x = symbol("x");
    auto const e = 2_i^symbol("a");
    EXPECT_EQ(toString(solve(e - e, x)), "{x}");
}

TEST(solve, alwaysFail)
{
    auto const x = symbol("x");
    auto const e1 = 2_i^symbol("a");
    auto const e2 = 3_i^symbol("a");
    EXPECT_EQ(toString(solve(e1 - e2, x)), "{}");
}

// --- solve single

TEST(solve, zero)
{
    auto const x = symbol("x");
    auto const e = 0_i;
    EXPECT_EQ(toString(solve(e, x)), "{x}");
}

TEST(solve, nonZero)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    EXPECT_EQ(toString(solve(y, x)), "{}");
}

TEST(solve, product)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    EXPECT_EQ(toString(solve(y * x, x)), "{0}");
}

TEST(solveEq, ax2_bx_c)
{
    auto const x = "x"_s;
    EXPECT_EQ(toString(solve(2_i * x * x - 3_i * x + 1_i == 0_i, x)), "{1/2 1}");
}

TEST(solveEq, 2)
{
    auto const x = "x"_s;
    auto const e1 = x * 2_i;
    auto const e2 = 5_i - x;
    EXPECT_EQ(toString(solve(e1 == e2, x)), "{5/3}");
}

#if 0
TEST(solveEq, 3)
{
    auto const x = "x"_s;
    auto const e1 = x * 2_i;
    auto const e2 = 5_i - x;
    auto const e = min(e1, e2);
    // TODO simplify conditions
    EXPECT_EQ(toString(e), "(PieceWise ((* 2 x) (<= (* 2 x) (+ 5 (* -1 x)))) ((+ 5 (* -1 x)) (> (* 2 x) (+ 5 (* -1 x)))))");
    EXPECT_EQ(toString(solve(e, x)), "{5/3}");
}
#endif

TEST(solveEq, 4)
{
    auto const e = makeSharedExprPtr(impl::PieceWise{{{-1_i, -"x"_s >= "x"_s}, {1_i, -"x"_s < "x"_s}}});
    auto solution = solve(e, "x"_s, interval(-3_i, true, -2_i, true));
    EXPECT_EQ(toString(solution), "{}");
}

TEST(solveEq, 5)
{
    auto const e = makeSharedExprPtr(impl::PieceWise{{{"x"_s + 2_i, -"x"_s >= "x"_s}, {1_i, -"x"_s < "x"_s}}});
    auto solution = solve(e, "x"_s, interval(-3_i, true, -2_i, true));
    EXPECT_EQ(toString(solution), "{-2}");
}

TEST(solveEq, 5_)
{
    auto const e = "x"_s + 2_i;
    auto solution = solve(e, "x"_s, interval(-3_i, true, -2_i, true));
    EXPECT_EQ(toString(solution), "{-2}");
}
