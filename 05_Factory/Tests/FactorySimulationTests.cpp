#include "FactorySimulation.h"

#include <gtest/gtest.h>

#include <cmath>

using namespace FactoryRules;

TEST(FactorySimulation, StartsWithHealthyLines)
{
    const FFactorySimulation Sim(3, 1u);
    ASSERT_EQ(Sim.GetLines().size(), 3u);
    for (const FLineValues& Line : Sim.GetLines())
    {
        EXPECT_FLOAT_EQ(Line.Efficiency, 100.f);
        EXPECT_FLOAT_EQ(Line.Resources, 100.f);
        EXPECT_EQ(Line.State, ELineState::Operational);
    }
    EXPECT_FALSE(Sim.AnyCritical());
}

TEST(FactorySimulation, ValuesStayInRangeAndMoveAtMostMaxDelta)
{
    FFactorySimulation Sim(3, 123u, 5.f);
    for (int StepIndex = 0; StepIndex < 1000; ++StepIndex)
    {
        const std::vector<FLineValues> Before = Sim.GetLines();
        Sim.StepAll();
        for (size_t i = 0; i < Before.size(); ++i)
        {
            const FLineValues& Line = Sim.GetLines()[i];
            ASSERT_GE(Line.Efficiency, 0.f);
            ASSERT_LE(Line.Efficiency, 100.f);
            ASSERT_LE(std::abs(Line.Efficiency - Before[i].Efficiency), 5.0001f);
            ASSERT_LE(std::abs(Line.Resources - Before[i].Resources), 5.0001f);
            ASSERT_EQ(Line.State, Evaluate(Line.Efficiency, Line.Resources));
        }
    }
}

TEST(FactorySimulation, SameSeedIsDeterministic)
{
    FFactorySimulation A(3, 7u);
    FFactorySimulation B(3, 7u);
    for (int i = 0; i < 100; ++i)
    {
        A.StepAll();
        B.StepAll();
    }
    for (size_t i = 0; i < 3; ++i)
    {
        EXPECT_FLOAT_EQ(A.GetLines()[i].Efficiency, B.GetLines()[i].Efficiency);
        EXPECT_FLOAT_EQ(A.GetLines()[i].Resources, B.GetLines()[i].Resources);
    }
}

TEST(FactorySimulation, ReportedChangesMatchTransitions)
{
    FFactorySimulation Sim(3, 99u, 20.f);
    for (int i = 0; i < 500; ++i)
    {
        const std::vector<FLineValues> Before = Sim.GetLines();
        for (const FStateChange& Change : Sim.StepAll())
        {
            EXPECT_EQ(Change.From, Before[static_cast<size_t>(Change.LineId)].State);
            EXPECT_EQ(Change.To, Sim.GetLines()[static_cast<size_t>(Change.LineId)].State);
            EXPECT_NE(Change.From, Change.To);
        }
    }
}

TEST(FactorySimulation, LargeDeltasEventuallyReachCritical)
{
    FFactorySimulation Sim(3, 5u, 25.f);
    bool bSawCritical = false;
    for (int i = 0; i < 2000 && !bSawCritical; ++i)
    {
        Sim.StepAll();
        bSawCritical = Sim.AnyCritical();
    }
    EXPECT_TRUE(bSawCritical);
}

TEST(FactorySimulation, RestoreAndResetBringLinesBack)
{
    FFactorySimulation Sim(2, 1u);
    ASSERT_TRUE(Sim.Restore(1, 10.f));
    EXPECT_EQ(Sim.GetLines()[1].State, ELineState::Critical);
    EXPECT_TRUE(Sim.AnyCritical());

    ASSERT_TRUE(Sim.Restore(1, 80.f));
    EXPECT_EQ(Sim.GetLines()[1].State, ELineState::Operational);

    Sim.Restore(0, 0.f);
    Sim.ResetAll();
    EXPECT_FALSE(Sim.AnyCritical());
    EXPECT_FLOAT_EQ(Sim.GetLines()[0].Efficiency, 100.f);
}

TEST(FactorySimulation, RestoreUnknownLineFails)
{
    FFactorySimulation Sim(3, 1u);
    EXPECT_FALSE(Sim.Restore(42, 100.f));
}

TEST(Emergency, FailsOnceWhenTimeRunsOut)
{
    FEmergency Emergency;
    EXPECT_FALSE(Emergency.IsActive());
    Emergency.Start(2, 10.f);
    EXPECT_TRUE(Emergency.IsActive());
    EXPECT_EQ(Emergency.GetLineId(), 2);
    EXPECT_FALSE(Emergency.Tick(4.f));
    EXPECT_FLOAT_EQ(Emergency.GetRemaining(), 6.f);
    EXPECT_TRUE(Emergency.Tick(6.f));
    EXPECT_FALSE(Emergency.IsActive());
    EXPECT_FALSE(Emergency.Tick(1.f)); // only reported once
}

TEST(Emergency, ResolvedEmergencyNeverFails)
{
    FEmergency Emergency;
    Emergency.Start(0, 1.f);
    Emergency.Resolve();
    EXPECT_FALSE(Emergency.Tick(5.f));
}
