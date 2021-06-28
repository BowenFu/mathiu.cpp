#include "mathia.h"
#include "gtest/gtest.h"
using namespace mathia;

TEST(Order, constant)
{
    EXPECT_TRUE(less(constant(2), fraction(5, 2)));
    EXPECT_TRUE(less(fraction(5, 2), constant(7)));
}

TEST(Order, symbol)
{
    EXPECT_TRUE((symbol("a"), symbol("b")));
    EXPECT_TRUE((symbol("v1"), symbol("v2")));
    EXPECT_TRUE((symbol("x1"), symbol("xa")));
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
    EXPECT_TRUE(impl::lessC<std::shared_ptr<impl::Expr>>({c, d}, {b, c, d}));
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
    EXPECT_TRUE(impl::lessC<std::shared_ptr<impl::Expr>>({b, d}, {b, c, d}));
    auto const e1 = c * d;
    auto const e2 = b * c * d;
    EXPECT_EQ(toString(e1), "(* c d)");
    EXPECT_EQ(toString(e2), "(* b c d)");
    EXPECT_TRUE(impl::lessC<std::shared_ptr<impl::Expr>>({c, d}, {b, c, d}));
    EXPECT_TRUE(impl::less(e1, e2));
}

TEST(Order, sum)
{
    auto const a = symbol("a");
    auto const b = symbol("b");
    auto const c = symbol("c");
    auto const d = symbol("d");
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