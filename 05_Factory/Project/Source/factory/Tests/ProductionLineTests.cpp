// Unreal Automation Tests for the Factory module.
// Run them in the editor (Tools > Session Frontend > Automation, filter "Factory") or headless:
//   UnrealEditor-Cmd.exe Factory.uproject -ExecCmds="Automation RunTests Factory; Quit" -unattended -nullrhi
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "ProductionLine.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

namespace FactoryTests
{
/** Creates a minimal game world and spawns a production line in it. The caller destroys the world. */
static UWorld* CreateTestWorld()
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("FactoryTestWorld"));
    FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
    Context.SetCurrentWorld(World);
    return World;
}

static void DestroyTestWorld(UWorld* World)
{
    GEngine->DestroyWorldContext(World);
    World->DestroyWorld(false);
}
} // namespace FactoryTests


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFactoryComputeStateTest, "Factory.ProductionLine.ComputeState",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFactoryComputeStateTest::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("100/100 is Operational"), AProductionLine::ComputeState(100.f, 100.f, 75.f, 50.f), EProductionState::Operational);
    TestEqual(TEXT("75/75 is Operational (inclusive)"), AProductionLine::ComputeState(75.f, 75.f, 75.f, 50.f), EProductionState::Operational);
    TestEqual(TEXT("one value below 75 is Warning"), AProductionLine::ComputeState(100.f, 60.f, 75.f, 50.f), EProductionState::Warning);
    TestEqual(TEXT("one value below 50 is Critical"), AProductionLine::ComputeState(49.f, 100.f, 75.f, 50.f), EProductionState::Critical);
    TestEqual(TEXT("custom thresholds"), AProductionLine::ComputeState(85.f, 85.f, 90.f, 80.f), EProductionState::Warning);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFactoryRestoreLineTest, "Factory.ProductionLine.RestoreLine",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFactoryRestoreLineTest::RunTest(const FString& Parameters)
{
    UWorld* World = FactoryTests::CreateTestWorld();
    AProductionLine* Line = World->SpawnActor<AProductionLine>();
    if (!TestNotNull(TEXT("line spawned"), Line))
    {
        FactoryTests::DestroyTestWorld(World);
        return false;
    }

    TestEqual(TEXT("starts Operational"), Line->CurrentState, EProductionState::Operational);

    Line->RestoreLine(30.f);
    TestEqual(TEXT("30% is Critical"), Line->CurrentState, EProductionState::Critical);
    TestEqual(TEXT("efficiency set"), Line->Efficiency, 30.f);
    TestEqual(TEXT("resources set"), Line->ResourceLevel, 30.f);

    Line->RestoreLine(250.f);
    TestEqual(TEXT("values are clamped to 100"), Line->Efficiency, 100.f);
    TestEqual(TEXT("back to Operational"), Line->CurrentState, EProductionState::Operational);

    Line->RestoreLine(60.f);
    Line->ResetLine();
    TestEqual(TEXT("ResetLine restores 100%"), Line->ResourceLevel, 100.f);
    TestEqual(TEXT("ResetLine restores Operational"), Line->CurrentState, EProductionState::Operational);

    FactoryTests::DestroyTestWorld(World);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
