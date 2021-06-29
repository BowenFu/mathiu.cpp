#include "mathia.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathia;

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
    EXPECT_EQ(toString(expr), "-1.000000 + 0.000000i");
    auto result = ceval(expr);
    EXPECT_NEAR(result.real(), -1, 1e-5);
    EXPECT_NEAR(result.imag(), 0, 1e-5);
}

TEST(Expr, fraction)
{
    auto expr = constant(5) / constant(8);
    EXPECT_EQ(toString(expr), "5/8");
}

// TEST(Expr, ceval)
// {
//     auto expr = e ^ (-i * pi);
//     auto exprStr = toString(expr);
//     EXPECT_EQ(exprStr, "(^ e (* (* -1 i) pi))");
//     auto result = ceval(expr);
//     EXPECT_NEAR(result.real(), -1, 1e-5);
//     EXPECT_NEAR(result.imag(), 0, 1e-5);
// }

TEST(Expr, ceval1)
{
    auto expr = (-i * pi);
    auto exprStr = toString(expr);
    EXPECT_EQ(exprStr, "(* -1 pi i)");
    auto result = ceval(expr);
    EXPECT_NEAR(result.real(), 0, 1e-5);
    EXPECT_NEAR(result.imag(), -3.1415926, 1e-5);
}

TEST(Expr, product)
{
    auto x = constant(5);
    auto y = constant(5);
    auto expr = x * y;
    EXPECT_EQ(toString(expr), "25");
    EXPECT_EQ(eval(expr), 25);
}

TEST(Expr, quotient)
{
    auto x = constant(5);
    auto y = constant(5);
    auto expr = x / y;
    EXPECT_EQ(toString(expr), "1");
    EXPECT_EQ(eval(expr), 1);
}

TEST(Expr, sum)
{
    auto x = constant(5);
    auto y = constant(5);
    auto expr = x + y;
    EXPECT_EQ(toString(expr), "10");
    EXPECT_EQ(eval(expr), 10);
}

TEST(Expr, sum2)
{
    auto x = constant(5);
    auto y = constant(5);
    auto expr = x + y / x;
    EXPECT_EQ(toString(expr), "6");
    EXPECT_EQ(eval(expr), 6);
}

TEST(Expr, difference)
{
    auto x = constant(5);
    auto y = constant(5);
    auto expr = x - y;
    EXPECT_EQ(toString(expr), "0");
    EXPECT_EQ(eval(expr), 0);
}

TEST(Expr, compound)
{
    auto x = constant(5);
    auto y = constant(2);
    auto expr = x / y - y * y;
    EXPECT_EQ(toString(expr), "-3/2");
    EXPECT_EQ(eval(expr), -1.5);
}

TEST(Expr, symbol)
{
    auto x = constant(5);
    auto y = symbol("y");
    auto expr = x / y - y * y;
    auto result = toString(expr);
    EXPECT_EQ(result, std::string("(+ (* -1 y y) (* 5 (^ y -1)))"));
}

TEST(Expr, sumMultiple)
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto z = symbol("z");
    auto expr = x + y + z;
    // auto expr = x + z + y;
    auto result = toString(expr);
    EXPECT_EQ(result, std::string("(+ x y z)"));
}

