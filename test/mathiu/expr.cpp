#include "mathiu.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(Expr, integer)
{
    auto x = integer(5);
    EXPECT_EQ(evald(x), 5);
}

TEST(Expr, pi)
{
    using namespace matchit;
    match(*pi)
    (
        pattern | as<mathiu::impl::Pi>(_) = [&]{}
    );
}

TEST(Expr, pi2)
{
    auto expr = sin(pi);
    EXPECT_EQ(toString(expr), "0");
}

TEST(Expr, pi3)
{
    auto expr = sin(pi / integer(2));
    EXPECT_EQ(toString(expr), "1");
}

TEST(Expr, e)
{
    auto expr = log(e, e);
    EXPECT_EQ(toString(expr), "(Log e e)");
    EXPECT_NEAR(evald(expr), 1, 1e-5);
}

TEST(Expr, i)
{
    auto expr = i * i;
    EXPECT_EQ(toString(expr), "(^ i 2)");
    auto result = evalc(expr);
    EXPECT_NEAR(result.real(), -1, 1e-5);
    EXPECT_NEAR(result.imag(), 0, 1e-5);
}

TEST(Expr, fraction)
{
    auto expr = integer(5) / integer(8);
    EXPECT_EQ(toString(expr), "5/8");
}

// TEST(Expr, evalc)
// {
//     auto expr = e ^ (-i * pi);
//     auto exprStr = toString(expr);
//     EXPECT_EQ(exprStr, "(^ e (* (* -1 i) pi))");
//     auto result = evalc(expr);
//     EXPECT_NEAR(result.real(), -1, 1e-5);
//     EXPECT_NEAR(result.imag(), 0, 1e-5);
// }

TEST(Expr, ceval1)
{
    auto expr = (-i * pi);
    auto exprStr = toString(expr);
    EXPECT_EQ(exprStr, "(* -1 pi i)");
    auto result = evalc(expr);
    EXPECT_NEAR(result.real(), 0, 1e-5);
    EXPECT_NEAR(result.imag(), -3.1415926, 1e-5);
}

TEST(Expr, product)
{
    auto x = integer(5);
    auto y = integer(5);
    auto expr = x * y;
    EXPECT_EQ(toString(expr), "25");
    EXPECT_EQ(evald(expr), 25);
}

TEST(Expr, quotient)
{
    auto x = integer(5);
    auto y = integer(5);
    auto expr = x / y;
    EXPECT_EQ(toString(expr), "1");
    EXPECT_EQ(evald(expr), 1);
}

TEST(Expr, sum)
{
    auto x = integer(5);
    auto y = integer(5);
    auto expr = x + y;
    EXPECT_EQ(toString(expr), "10");
    EXPECT_EQ(evald(expr), 10);
}

TEST(Expr, sum2)
{
    auto x = integer(5);
    auto y = integer(5);
    auto expr = x + y / x;
    EXPECT_EQ(toString(expr), "6");
    EXPECT_EQ(evald(expr), 6);
}

TEST(Expr, difference)
{
    auto x = integer(5);
    auto y = integer(5);
    auto expr = x - y;
    EXPECT_EQ(toString(expr), "0");
    EXPECT_EQ(evald(expr), 0);
}

TEST(Expr, compound)
{
    auto x = integer(5);
    auto y = integer(2);
    auto expr = x / y - y * y;
    EXPECT_EQ(toString(expr), "-3/2");
    EXPECT_EQ(evald(expr), -1.5);
}

TEST(Expr, symbol)
{
    auto y = symbol("y");
    auto expr = y * y;
    auto result = toString(expr);
    EXPECT_EQ(result, std::string("(^ y 2)"));
}

TEST(Expr, symbol2)
{
    auto x = integer(5);
    auto y = symbol("y");
    auto expr = x / y - y * y;
    auto result = toString(expr);
    EXPECT_EQ(result, std::string("(+ (* 5 (^ y -1)) (* -1 (^ y 2)))"));
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

