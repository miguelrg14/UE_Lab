#include "ProductionLine.h"
#include "factory.h"
#include "Core/FactoryRules.h"
#include "Async/Async.h"
#include "Engine/World.h"
#include "Math/RandomStream.h"
#include "TimerManager.h"

AProductionLine::AProductionLine()
{
    PrimaryActorTick.bCanEverTick = false;
    Efficiency = 100.0f;
    ResourceLevel = 100.0f;
    CurrentState = EProductionState::Operational;
    LineID = 0;
}

void AProductionLine::BeginPlay()
{
    Super::BeginPlay();
    InitiateMonitoring();
}

void AProductionLine::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(Timer_Update);
    Super::EndPlay(EndPlayReason);
}

void AProductionLine::InitiateMonitoring()
{
    GetWorldTimerManager().SetTimer(Timer_Update, this, &AProductionLine::UpdateData, UpdateInterval, true);
}

// EProductionState and FactoryRules::ELineState must stay in the same order.
static_assert(static_cast<uint8>(EProductionState::Operational) == static_cast<uint8>(FactoryRules::ELineState::Operational)
    && static_cast<uint8>(EProductionState::Warning) == static_cast<uint8>(FactoryRules::ELineState::Warning)
    && static_cast<uint8>(EProductionState::Critical) == static_cast<uint8>(FactoryRules::ELineState::Critical),
    "EProductionState must match FactoryRules::ELineState");

EProductionState AProductionLine::ComputeState(float InEfficiency, float InResourceLevel, float InOperationalThreshold, float InWarningThreshold)
{
    const FactoryRules::FThresholds Thresholds{InOperationalThreshold, InWarningThreshold};
    return static_cast<EProductionState>(FactoryRules::Evaluate(InEfficiency, InResourceLevel, Thresholds));
}

void AProductionLine::UpdateData()
{
    // Copy everything the worker needs: the background task must never read or write this actor.
    const float StartEfficiency = Efficiency;
    const float StartResourceLevel = ResourceLevel;
    const float MaxDelta = MaxDeltaPerUpdate;
    const int32 Seed = static_cast<int32>(FPlatformTime::Cycles() ^ GetUniqueID());

    // Weak pointer: if the line is destroyed before the task finishes, the result is simply dropped.
    TWeakObjectPtr<AProductionLine> WeakThis(this);

    Async(EAsyncExecution::ThreadPool, [WeakThis, StartEfficiency, StartResourceLevel, MaxDelta, Seed]()
    {
        // Local random stream: each task owns its generator instead of sharing global state across threads.
        FRandomStream Stream(Seed);
        const float NewEfficiency = FactoryRules::Step(StartEfficiency, Stream.FRandRange(-1.f, 1.f), MaxDelta);
        const float NewResourceLevel = FactoryRules::Step(StartResourceLevel, Stream.FRandRange(-1.f, 1.f), MaxDelta);

        // Back to the game thread to touch UObjects and broadcast delegates.
        AsyncTask(ENamedThreads::GameThread, [WeakThis, NewEfficiency, NewResourceLevel]()
        {
            if (AProductionLine* Line = WeakThis.Get())
            {
                Line->ApplyValues(NewEfficiency, NewResourceLevel);
            }
        });
    });
}

void AProductionLine::ApplyValues(float NewEfficiency, float NewResourceLevel)
{
    check(IsInGameThread());

    Efficiency = FactoryRules::ClampValue(NewEfficiency);
    ResourceLevel = FactoryRules::ClampValue(NewResourceLevel);

    const EProductionState NewState = ComputeState(Efficiency, ResourceLevel, OperationalThreshold, WarningThreshold);
    if (NewState != CurrentState)
    {
        CurrentState = NewState;
        UE_LOG(LogFactory, Verbose, TEXT("Line %d -> %s (Efficiency %.1f, Resources %.1f)"),
            LineID, *UEnum::GetDisplayValueAsText(NewState).ToString(), Efficiency, ResourceLevel);
        OnStateChanged.Broadcast(NewState, LineID);
    }

    OnValuesUpdated.Broadcast(this);
}

void AProductionLine::ResetLine()
{
    RestoreLine(100.f);
}

void AProductionLine::RestoreLine(float Level)
{
    ApplyValues(Level, Level);
}
