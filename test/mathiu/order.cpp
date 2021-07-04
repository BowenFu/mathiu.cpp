#include "mathiu.h"
#include "gtest/gtest.h"
using namespace mathiu;

TEST(Order, integer)
{
    EXPECT_TRUE(less(integer(2), fraction(5, 2)));
    EXPECT_TRUE(less(fraction(5, 2), integer(7)));
}

TEST(Order, constant1)
{
    EXPECT_TRUE(less(pi, i));
    // EXPECT_TRUE(less(pi, -i));
}

TEST(Order, symbol)
{
    EXPECT_TRUE(less(symbol("a"), symbol("b")));
    EXPECT_TRUE(less(symbol("v1"), symbol("v2")));
    EXPECT_TRUE(less(symbol("x1"), symbol("xa")));
}

TEST(Order, less)
{
    EXPECT_TRUE(impl::lessC<double>({}, {1}));
    EXPECT_TRUE(impl::lessC<double>({1}, {1, 2}));
    EXPECT_TRUE(impl::lessC<int32_t>({2}, {1, 2}));
    EXPECT_TRUE(impl::lessC<float>({2.0}, {1.0, 2.0}));
    EXPECT_TRUE(impl::lessC<double>({2.0}, {1.0, 2.0}));
    EXPECT_TRUE(impl::lessC<double>({2}, {1, 2}));
    EXPECT_TRUE(impl::lessC<double>({1, 2}, {1, 1, 2}));
    EXPECT_TRUE(impl::lessC<double>({1, 2, 3}, {1, 1, 2, 3}));
    EXPECT_TRUE(impl::lessC<std::string>({"c", "d"}, {"b", "c", "d"}));
    auto const b = symbol("b");
    auto const c = symbol("c");
    auto const d = symbol("d");
    EXPECT_TRUE(impl::lessC<impl::ExprPtr>({c, d}, {b, c, d}));
}

TEST(Order, product)
{
    auto const a = symbol("a");
    auto const b = symbol("b");
    auto const c = symbol("c");
    auto const d = symbol("d");
    EXPECT_TRUE(less(a * b, a * c));
    EXPECT_TRUE(less(a * c * d, b * c * d));
}

TEST(Order, product1)
{
    auto const b = symbol("b");;
    auto const c = symbol("c");
    auto const d = symbol("d");
    EXPECT_EQ(toString(b * d), "(* b d)");
    EXPECT_EQ(toString(b * c * d), "(* b c d)");
    EXPECT_TRUE(impl::lessC<impl::ExprPtr>({b, d}, {b, c, d}));

    auto const e1 = c * d;
    auto const e2 = b * c * d;
    EXPECT_EQ(toString(e1), "(* c d)");
    EXPECT_EQ(toString(e2), "(* b c d)");
    EXPECT_TRUE(impl::lessC<impl::ExprPtr>({c, d}, {b, c, d}));
    EXPECT_TRUE(less(e1, e2));
}

TEST(Order, sum)
{
    auto const d = symbol("d");
    auto const c = symbol("c");
    auto const b = symbol("b");
    auto const a = symbol("a");

    EXPECT_EQ(toString(a + b), "(+ a b)");
    EXPECT_EQ(toString(a + c), "(+ a c)");
    EXPECT_EQ(toString(a + c + d), "(+ a c d)");
    EXPECT_EQ(toString(b + c + d), "(+ b c d)");
    EXPECT_TRUE(less(a + b, a + c));
    EXPECT_TRUE(less(a + c + d, b + c + d));
}

TEST(Order, sum1)
{
    auto const b = symbol("b");
    auto const c = symbol("c");
    auto const d = symbol("d");
    EXPECT_TRUE(less(b + d, b + c + d));
    EXPECT_TRUE(less(c + d, b + c + d));
}

TEST(Order, power)
{
    auto const n1 = integer(1);
    auto const n2 = integer(2);
    auto const n3 = integer(3);
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const e1 = (n1 + x) ^ n2;
    auto const e2 = (n1 + x) ^ n3;
    EXPECT_TRUE(less(e1, e2));
    EXPECT_EQ(toString(e1), "(^ (+ 1 x) 2)");
    EXPECT_EQ(toString(e2), "(^ (+ 1 x) 3)");
    EXPECT_TRUE(less((n1 + x) ^ n3, (n1 + y) ^ n2));
}

TEST(Order, compond)
{
    auto const a = symbol("a");
    auto const x = symbol("x");
    auto const n2 = integer(2);
    auto const n3 = integer(3);
    EXPECT_EQ(toString(a*(x^n2)), "(* a (^ x 2))");
    EXPECT_EQ(toString(x^n3), "(^ x 3)");
    EXPECT_TRUE(less(a*(x^n2), x^n3));
}

TEST(Order, compond2)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const n1 = integer(1);
    auto const n3 = integer(3);
    EXPECT_TRUE(less((n1 + x)^n3, n1 + y));
}

TEST(Order, compond3)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    auto const n1 = integer(1);
    EXPECT_TRUE(less(n1 + x, y));
}

TEST(Order, compond4)
{
    auto const x = symbol("x");
    auto const n2 = integer(2);
    EXPECT_TRUE(less(x, x^n2));
}

TEST(Order, func)
{
    auto const x = symbol("x");
    auto const n2 = integer(2);
    EXPECT_TRUE(less(sin(x), sin(x^n2)));
}