#include "evalit.h"
#include "gtest/gtest.h"
using namespace evalit;

TEST(Expr, value)
{
    auto x = value(5);
    EXPECT_EQ(eval(x), 5);
}

TEST(Expr, product)
{
    auto x = value(5);
    auto y = value(5);
    EXPECT_EQ(eval(x * y), 25);
}

TEST(Expr, quotient)
{
    auto x = value(5);
    auto y = value(5);
    EXPECT_EQ(eval(x / y), 1);
}

TEST(Expr, sum)
{
    auto x = value(5);
    auto y = value(5);
    EXPECT_EQ(eval(x + y), 10);
}

TEST(Expr, difference)
{
    auto x = value(5);
    auto y = value(5);
    EXPECT_EQ(eval(x - y), 0);
}

TEST(Expr, compound)
{
    auto x = value(5);
    auto y = value(2);
    EXPECT_EQ(eval(x / y - y * y), -1.5);
}

TEST(Expr, symbol)
{
    auto x = value(5);
    auto y = symbol("y");
    auto expr = x / y - y * y;
    std::get<impl::Symbol>(*y).value = value(2);
    EXPECT_EQ(eval(expr), -1.5);
    std::get<impl::Symbol>(*y).value = value(2.5);
    EXPECT_EQ(eval(expr), -4.25);
    std::get<impl::Symbol>(*y).value = x;
    EXPECT_EQ(eval(expr), -24);
}