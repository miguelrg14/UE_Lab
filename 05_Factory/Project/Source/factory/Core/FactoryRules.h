// Engine-independent rules of the factory simulation.
//
// This header has no Unreal dependencies on purpose: AProductionLine uses it inside the engine, and the
// same code is compiled by CMake for the unit tests and the console simulation (see 05_Factory/Tests).
#pragma once

#include <algorithm>
#include <cstdint>

namespace FactoryRules
{
/** Health state of a production line. Same order as EProductionState (checked with a static_assert). */
enum class ELineState : std::uint8_t
{
    Operational = 0,
    Warning = 1,
    Critical = 2
};

/** Efficiency and resources are percentages. */
inline constexpr float MinValue = 0.f;
inline constexpr float MaxValue = 100.f;

/** State thresholds: both values >= Operational -> Operational, both >= Warning -> Warning, else Critical. */
struct FThresholds
{
    float Operational = 75.f;
    float Warning = 50.f;
};

/** Values of one line. */
struct FLineValues
{
    int Id = 0;
    float Efficiency = MaxValue;
    float Resources = MaxValue;
    ELineState State = ELineState::Operational;
};

/** Clamps a value to [MinValue, MaxValue]. NaN is treated as MinValue so a bad input can never stay "healthy". */
inline float ClampValue(float Value)
{
    if (!(Value == Value)) // NaN
    {
        return MinValue;
    }
    return std::clamp(Value, MinValue, MaxValue);
}

/** Evaluates the state of a line. The worst of the two values decides. */
inline ELineState Evaluate(float Efficiency, float Resources, const FThresholds& Thresholds = {})
{
    const float Worst = std::min(ClampValue(Efficiency), ClampValue(Resources));
    if (Worst >= Thresholds.Operational)
    {
        return ELineState::Operational;
    }
    if (Worst >= Thresholds.Warning)
    {
        return ELineState::Warning;
    }
    return ELineState::Critical;
}

/**
 * Random-walk step: moves Current by Unit * MaxDelta and clamps the result.
 * Unit is a random number in [-1, 1] supplied by the caller, so the rule itself stays deterministic.
 */
inline float Step(float Current, float Unit, float MaxDelta)
{
    const float SafeUnit = std::clamp(Unit, -1.f, 1.f);
    return ClampValue(Current + SafeUnit * std::max(MaxDelta, 0.f));
}

/** Stores new values in Line and re-evaluates its state. Returns true if the state changed. */
inline bool Apply(FLineValues& Line, float NewEfficiency, float NewResources, const FThresholds& Thresholds = {})
{
    Line.Efficiency = ClampValue(NewEfficiency);
    Line.Resources = ClampValue(NewResources);

    const ELineState NewState = Evaluate(Line.Efficiency, Line.Resources, Thresholds);
    const bool bChanged = NewState != Line.State;
    Line.State = NewState;
    return bChanged;
}

/** Human-readable state name (matches the UMETA DisplayName of EProductionState). */
inline const char* ToString(ELineState State)
{
    switch (State)
    {
    case ELineState::Operational: return "Operational";
    case ELineState::Warning:     return "Warning";
    case ELineState::Critical:    return "Critical";
    }
    return "Unknown";
}
} // namespace FactoryRules
