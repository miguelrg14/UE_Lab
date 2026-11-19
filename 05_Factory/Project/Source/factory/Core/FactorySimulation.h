// Engine-independent model of the whole factory: a set of lines updated by the same rules as
// AProductionLine, plus the emergency countdown of UEmergencyMiniGame. Used by the unit tests
// and by the console simulation that CI runs as a smoke test.
#pragma once

#include "FactoryRules.h"

#include <random>
#include <vector>

namespace FactoryRules
{
/** A state change produced by one simulation step. */
struct FStateChange
{
    int LineId = 0;
    ELineState From = ELineState::Operational;
    ELineState To = ELineState::Operational;
};

/** Emergency countdown: the player has TimeToReact seconds to restore the line. */
class FEmergency
{
public:
    void Start(int InLineId, float TimeToReact)
    {
        LineId = InLineId;
        Remaining = std::max(TimeToReact, 0.f);
        bActive = true;
    }

    /** Advances the countdown. Returns true exactly once, when the time runs out. */
    bool Tick(float DeltaSeconds)
    {
        if (!bActive)
        {
            return false;
        }
        Remaining -= std::max(DeltaSeconds, 0.f);
        if (Remaining <= 0.f)
        {
            Remaining = 0.f;
            bActive = false;
            return true;
        }
        return false;
    }

    void Resolve() { bActive = false; }

    bool IsActive() const { return bActive; }
    int GetLineId() const { return LineId; }
    float GetRemaining() const { return Remaining; }

private:
    int LineId = 0;
    float Remaining = 0.f;
    bool bActive = false;
};

/** The factory: N lines that drift randomly every step. */
class FFactorySimulation
{
public:
    FFactorySimulation(int NumLines, unsigned Seed, float InMaxDelta = 5.f, FThresholds InThresholds = {})
        : Rng(Seed), MaxDelta(InMaxDelta), Thresholds(InThresholds)
    {
        for (int i = 0; i < NumLines; ++i)
        {
            FLineValues Line;
            Line.Id = i;
            Lines.push_back(Line);
        }
    }

    /** One update (AProductionLine::UpdateData for every line). Returns the lines that changed state. */
    std::vector<FStateChange> StepAll()
    {
        std::uniform_real_distribution<float> Unit(-1.f, 1.f);
        std::vector<FStateChange> Changes;
        for (FLineValues& Line : Lines)
        {
            const ELineState Before = Line.State;
            const float NewEfficiency = Step(Line.Efficiency, Unit(Rng), MaxDelta);
            const float NewResources = Step(Line.Resources, Unit(Rng), MaxDelta);
            if (Apply(Line, NewEfficiency, NewResources, Thresholds))
            {
                Changes.push_back({Line.Id, Before, Line.State});
            }
        }
        return Changes;
    }

    /** Sets both values of a line to Level (UEmergencyMiniGame success path). Returns false if the ID does not exist. */
    bool Restore(int LineId, float Level)
    {
        for (FLineValues& Line : Lines)
        {
            if (Line.Id == LineId)
            {
                Apply(Line, Level, Level, Thresholds);
                return true;
            }
        }
        return false;
    }

    /** Resets every line to 100% (HUD "Reset factory" button). */
    void ResetAll()
    {
        for (FLineValues& Line : Lines)
        {
            Apply(Line, MaxValue, MaxValue, Thresholds);
        }
    }

    bool AnyCritical() const
    {
        return std::any_of(Lines.begin(), Lines.end(), [](const FLineValues& Line) { return Line.State == ELineState::Critical; });
    }

    const std::vector<FLineValues>& GetLines() const { return Lines; }

private:
    std::vector<FLineValues> Lines;
    std::mt19937 Rng;
    float MaxDelta;
    FThresholds Thresholds;
};
} // namespace FactoryRules
