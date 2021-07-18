#include "mathiu.h"
#include "gtest/gtest.h"
#include <string>
using namespace mathiu;

TEST(IntervalComp, plus)
{
    auto const e = "x"_s + "y"_s;
    auto const range1 = functionRange(e, "x"_s, interval(1_i, true, 2_i, false));
    EXPECT_EQ(toString(range1), "(COInterval (+ 1 y) (+ 2 y))");
}

TEST(IntervalComp, plus2)
{
    auto const e = "x"_s + "y"_s;
    auto const range1 = functionRange(e, "x"_s, interval(1_i, true, 2_i, false));
    EXPECT_EQ(toString(range1), "(COInterval (+ 1 y) (+ 2 y))");
    auto const range1Interval = std::get<impl::Interval>(*range1);

    auto const leftRange = functionRange(range1Interval.first.first, "y"_s, interval(-2_i, false, -1_i, true));
    EXPECT_EQ(toString(leftRange), "(OCInterval -1 0)");

    auto const rightRange = functionRange(range1Interval.second.first, "y"_s, interval(-2_i, false, -1_i, true));
    EXPECT_EQ(toString(rightRange), "(OCInterval 0 1)");
}

TEST(IntervalComp, plus3)
{
    auto const e = "x"_s + "y"_s;
    auto const range1 = functionRange(e, "x"_s, interval(1_i, true, 2_i, false));
    EXPECT_EQ(toString(range1), "(COInterval (+ 1 y) (+ 2 y))");
    auto const range1Interval = std::get<impl::Interval>(*range1);
    
    auto const leftRange = functionRange(range1Interval.first.first, "y"_s, interval(-2_i, false, -1_i, true));
    EXPECT_EQ(toString(leftRange), "(OCInterval -1 0)");
    auto const leftRangeInterval = std::get<impl::Interval>(*leftRange);

    auto const rightRange = functionRange(range1Interval.second.first, "y"_s, interval(-2_i, false, -1_i, true));
    EXPECT_EQ(toString(rightRange), "(OCInterval 0 1)");
    auto const rightRangeInterval = std::get<impl::Interval>(*rightRange);

    auto const left = leftRangeInterval.first.first;
    auto const leftClose = leftRangeInterval.first.second && range1Interval.first.second;

    auto const right = rightRangeInterval.second.first;
    auto const rightClose = rightRangeInterval.second.second && range1Interval.second.second;
    auto const finalRange = interval(left, leftClose, right, rightClose);
    EXPECT_EQ(toString(finalRange), "(OOInterval -1 1)");
}

TEST(IntervalComp, plus4)
{
    auto const e = "x"_s + "y"_s;
    auto const finalRange = functionRange(e, set("x"_s >> interval(1_i, true, 2_i, false), "y"_s >> interval(-2_i, false, -1_i, true)));
    EXPECT_EQ(toString(finalRange), "(OOInterval -1 1)");
}

#if 0 // fix this with multivar calculus
TEST(IntervalComp, times)
{
    auto const e = "x"_s * "y"_s;
    auto const finalRange = functionRange(e, set("x"_s >> interval(1_i, true, 2_i, false), "y"_s >> interval(-2_i, false, -1_i, true)));
    EXPECT_EQ(toString(finalRange), "(OOInterval -4 -1)");
}
#endif
