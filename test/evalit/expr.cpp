#include "evalit.h"
#include "gtest/gtest.h"
#include <string>
using namespace evalit;

TEST(Expr, constant)
{
    auto x = constant(5);
    EXPECT_EQ(eval(x), 5);
}

TEST(Expr, pi)
{
    auto expr = sin(pi);
    EXPECT_EQ(toString(expr), "(Sin pi)");
    EXPECT_NEAR(eval(expr), 0, 1e-5);
    EXPECT_NEAR(eval(sin(pi / constant(2))), 1, 1e-5);
}

TEST(Expr, e)
{
    auto expr = log(e, e);
    EXPECT_EQ(toString(expr), "(Log e e)");
    EXPECT_NEAR(eval(expr), 1, 1e-5);
}

TEST(Expr, i)
{
    auto expr = i * i;
    EXPECT_EQ(toString(expr), "(* i i)");
    auto result = ceval(expr);
    EXPECT_NEAR(result.real(), -1, 1e-5);
    EXPECT_NEAR(result.imag(), 0, 1e-5);
}

TEST(Expr, ceval)
{
    auto expr = e ^ (-i * pi);
    EXPECT_EQ(toString(expr), "(^ e (* (* -1 i) pi))");
    auto result = ceval(expr);
    EXPECT_NEAR(result.real(), -1, 1e-5);
    EXPECT_NEAR(result.imag(), 0, 1e-5);
}

TEST(Expr, product)
{
    auto x = constant(5);
    auto y = constant(5);
    EXPECT_EQ(eval(x * y), 25);
}

TEST(Expr, quotient)
{
    auto x = constant(5);
    auto y = constant(5);
    EXPECT_EQ(eval(x / y), 1);
}

TEST(Expr, sum)
{
    auto x = constant(5);
    auto y = constant(5);
    EXPECT_EQ(eval(x + y), 10);
}

TEST(Expr, difference)
{
    auto x = constant(5);
    auto y = constant(5);
    EXPECT_EQ(eval(x - y), 0);
}

TEST(Expr, compound)
{
    auto x = constant(5);
    auto y = constant(2);
    EXPECT_EQ(eval(x / y - y * y), -1.5);
}

TEST(Expr, symbol)
{
    auto x = constant(5);
    auto y = symbol("y");
    auto expr = x / y - y * y;
    auto result = toString(expr);
    EXPECT_EQ(result, std::string("(+ (* 5 (^ y -1)) (* -1 (* y y)))"));
}
