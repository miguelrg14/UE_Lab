// Console version of the factory simulation, built on the same rules as the Unreal project.
//
//   factory_sim [--lines N] [--steps N] [--seed N] [--delta X] [--quiet]
//
// Every step is one second of game time. When a line becomes Critical an emergency starts; the
// simulated operator reacts after a random delay and restores the line to 80% if the delay is
// shorter than the 10 s window (the same values as UEmergencyMiniGame).
#include "FactorySimulation.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>

using namespace FactoryRules;

namespace
{
struct FOptions
{
    int Lines = 3;
    int Steps = 60;
    unsigned Seed = 42;
    float Delta = 5.f;
    bool bQuiet = false;
};

bool ParseOptions(int Argc, char** Argv, FOptions& Out)
{
    for (int i = 1; i < Argc; ++i)
    {
        const std::string Arg = Argv[i];
        const bool bHasValue = i + 1 < Argc;
        if (Arg == "--quiet") { Out.bQuiet = true; }
        else if (Arg == "--lines" && bHasValue) { Out.Lines = std::atoi(Argv[++i]); }
        else if (Arg == "--steps" && bHasValue) { Out.Steps = std::atoi(Argv[++i]); }
        else if (Arg == "--seed" && bHasValue) { Out.Seed = static_cast<unsigned>(std::strtoul(Argv[++i], nullptr, 10)); }
        else if (Arg == "--delta" && bHasValue) { Out.Delta = static_cast<float>(std::atof(Argv[++i])); }
        else
        {
            std::fprintf(stderr, "Usage: factory_sim [--lines N] [--steps N] [--seed N] [--delta X] [--quiet]\n");
            return false;
        }
    }
    if (Out.Lines < 1 || Out.Lines > 16 || Out.Steps < 1 || Out.Delta < 0.f)
    {
        std::fprintf(stderr, "Invalid options: lines must be 1-16, steps >= 1, delta >= 0.\n");
        return false;
    }
    return true;
}

void PrintLines(int Second, const FFactorySimulation& Sim)
{
    std::printf("t=%3ds ", Second);
    for (const FLineValues& Line : Sim.GetLines())
    {
        std::printf(" | L%d eff %5.1f res %5.1f %-11s", Line.Id, static_cast<double>(Line.Efficiency),
                    static_cast<double>(Line.Resources), ToString(Line.State));
    }
    std::printf("\n");
}
} // namespace

int main(int Argc, char** Argv)
{
    FOptions Options;
    if (!ParseOptions(Argc, Argv, Options))
    {
        return 2;
    }

    constexpr float TimeToReact = 10.f;
    constexpr float RecoveryLevel = 80.f;

    FFactorySimulation Sim(Options.Lines, Options.Seed, Options.Delta);
    std::mt19937 OperatorRng(Options.Seed ^ 0x5eedu);
    std::uniform_real_distribution<float> ReactionTime(2.f, 14.f);

    FEmergency Emergency;
    float OperatorDelay = 0.f;
    int StateChanges = 0, Emergencies = 0, Restored = 0, Failed = 0;

    for (int Second = 1; Second <= Options.Steps; ++Second)
    {
        for (const FStateChange& Change : Sim.StepAll())
        {
            ++StateChanges;
            if (!Options.bQuiet)
            {
                std::printf("        Line %d: %s -> %s\n", Change.LineId, ToString(Change.From), ToString(Change.To));
            }
            if (Change.To == ELineState::Critical && !Emergency.IsActive())
            {
                ++Emergencies;
                Emergency.Start(Change.LineId, TimeToReact);
                OperatorDelay = ReactionTime(OperatorRng);
                if (!Options.bQuiet)
                {
                    std::printf("        EMERGENCY on line %d, %.0f s to react\n", Change.LineId, static_cast<double>(TimeToReact));
                }
            }
        }

        if (Emergency.IsActive())
        {
            OperatorDelay -= 1.f;
            if (OperatorDelay <= 0.f)
            {
                Emergency.Resolve();
                Sim.Restore(Emergency.GetLineId(), RecoveryLevel);
                ++Restored;
                if (!Options.bQuiet) { std::printf("        Line %d restored to %.0f%%\n", Emergency.GetLineId(), static_cast<double>(RecoveryLevel)); }
            }
            else if (Emergency.Tick(1.f))
            {
                ++Failed;
                if (!Options.bQuiet) { std::printf("        FAILED to restore line %d in time\n", Emergency.GetLineId()); }
            }
        }

        if (!Options.bQuiet && (Second % 10 == 0 || Second == Options.Steps))
        {
            PrintLines(Second, Sim);
        }
    }

    std::printf("Simulated %d s with %d lines (seed %u): %d state changes, %d emergencies, %d restored, %d failed.\n",
                Options.Steps, Options.Lines, Options.Seed, StateChanges, Emergencies, Restored, Failed);

    // Sanity check used by CI: every line must end in the state its values imply.
    for (const FLineValues& Line : Sim.GetLines())
    {
        if (Line.State != Evaluate(Line.Efficiency, Line.Resources) || Line.Efficiency < 0.f || Line.Efficiency > 100.f)
        {
            std::fprintf(stderr, "Inconsistent state on line %d\n", Line.Id);
            return 1;
        }
    }
    return 0;
}
