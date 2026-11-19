#include "FactoryRules.h"

#include <gtest/gtest.h>

#include <limits>

using namespace FactoryRules;

TEST(Evaluate, BothValuesHighIsOperational)
{
    EXPECT_EQ(Evaluate(100.f, 100.f), ELineState::Operational);
    EXPECT_EQ(Evaluate(75.f, 75.f), ELineState::Operational); // thresholds are inclusive
}

TEST(Evaluate, WorstValueDecides)
{
    EXPECT_EQ(Evaluate(100.f, 74.9f), ELineState::Warning);
    EXPECT_EQ(Evaluate(74.9f, 100.f), ELineState::Warning);
    EXPECT_EQ(Evaluate(50.f, 99.f), ELineState::Warning);
    EXPECT_EQ(Evaluate(100.f, 49.9f), ELineState::Critical);
    EXPECT_EQ(Evaluate(0.f, 0.f), ELineState::Critical);
}

TEST(Evaluate, CustomThresholds)
{
    const FThresholds Strict{90.f, 60.f};
    EXPECT_EQ(Evaluate(89.f, 95.f, Strict), ELineState::Warning);
    EXPECT_EQ(Evaluate(59.f, 95.f, Strict), ELineState::Critical);
    EXPECT_EQ(Evaluate(90.f, 90.f, Strict), ELineState::Operational);
}

TEST(ClampValue, KeepsValuesInPercentRange)
{
    EXPECT_FLOAT_EQ(ClampValue(-10.f), 0.f);
    EXPECT_FLOAT_EQ(ClampValue(150.f), 100.f);
    EXPECT_FLOAT_EQ(ClampValue(42.5f), 42.5f);
}

TEST(ClampValue, NaNBecomesMinimum)
{
    EXPECT_FLOAT_EQ(ClampValue(std::numeric_limits<float>::quiet_NaN()), 0.f);
    EXPECT_EQ(Evaluate(std::numeric_limits<float>::quiet_NaN(), 100.f), ELineState::Critical);
}

TEST(Step, MovesByUnitTimesDeltaAndClamps)
{
    EXPECT_FLOAT_EQ(Step(50.f, 1.f, 5.f), 55.f);
    EXPECT_FLOAT_EQ(Step(50.f, -0.5f, 5.f), 47.5f);
    EXPECT_FLOAT_EQ(Step(98.f, 1.f, 5.f), 100.f);
    EXPECT_FLOAT_EQ(Step(2.f, -1.f, 5.f), 0.f);
}

TEST(Step, OutOfRangeInputsAreSanitised)
{
    EXPECT_FLOAT_EQ(Step(50.f, 3.f, 5.f), 55.f);   // unit clamped to [-1, 1]
    EXPECT_FLOAT_EQ(Step(50.f, 1.f, -5.f), 50.f);  // negative delta means no movement
}

TEST(Apply, ReportsStateChangesOnlyWhenTheyHappen)
{
    FLineValues Line;
    EXPECT_FALSE(Apply(Line, 90.f, 90.f));
    EXPECT_TRUE(Apply(Line, 60.f, 90.f));
    EXPECT_EQ(Line.State, ELineState::Warning);
    EXPECT_FALSE(Apply(Line, 55.f, 90.f));
    EXPECT_TRUE(Apply(Line, 10.f, 90.f));
    EXPECT_EQ(Line.State, ELineState::Critical);
}

TEST(Apply, StoresClampedValues)
{
    FLineValues Line;
    Apply(Line, 120.f, -3.f);
    EXPECT_FLOAT_EQ(Line.Efficiency, 100.f);
    EXPECT_FLOAT_EQ(Line.Resources, 0.f);
}

TEST(ToString, MatchesEnumDisplayNames)
{
    EXPECT_STREQ(ToString(ELineState::Operational), "Operational");
    EXPECT_STREQ(ToString(ELineState::Warning), "Warning");
    EXPECT_STREQ(ToString(ELineState::Critical), "Critical");
}
