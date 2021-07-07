#include "mathiu.h"
#include "polynomial.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(degree, product)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    EXPECT_EQ(toString(degree(y * x, x)), "1");
}

TEST(degree, power)
{
    auto const x = symbol("x");
    EXPECT_EQ(toString(degree(x * x, x)), "2");
}

TEST(degree, sum)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    EXPECT_EQ(toString(degree(x + y, x)), "1");
}

TEST(degree, multivar)
{
    auto const x = symbol("x");
    auto const y = symbol("y");
    EXPECT_EQ(toString(degree(x * y, set({x, y}))), "2");
}