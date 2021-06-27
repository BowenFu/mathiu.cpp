#include "evalit.h"
#include "gtest/gtest.h"
#include <string>
using namespace evalit;

TEST(Expr, constant1)
{
    auto x = constant(5);
    EXPECT_EQ(eval(x), 5);
}

TEST(Expr, constant2)
{
    auto expr = sin(pi);
    EXPECT_EQ(toString(expr), "(Sin pi)");
    EXPECT_NEAR(eval(expr), 0, 1e-5);
    EXPECT_NEAR(eval(sin(pi / constant(2))), 1, 1e-5);
}

TEST(Expr, constant3)
{
    EXPECT_NEAR(eval(log(e, e)), 1, 1e-5);
}

TEST(Expr, constant4)
{
    auto result = ceval(i * i);
    EXPECT_NEAR(result.real(), -1, 1e-5);
    EXPECT_NEAR(result.imag(), 0, 1e-5);
}

TEST(Expr, constant5)
{
    auto result = ceval(e ^ (-i * pi));
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
