#include "ProductionLine.h"
#include "factory.h"
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

EProductionState AProductionLine::ComputeState(float InEfficiency, float InResourceLevel, float InOperationalThreshold, float InWarningThreshold)
{
    if (InEfficiency >= InOperationalThreshold && InResourceLevel >= InOperationalThreshold)
    {
        return EProductionState::Operational;
    }
    if (InEfficiency >= InWarningThreshold && InResourceLevel >= InWarningThreshold)
    {
        return EProductionState::Warning;
    }
    return EProductionState::Critical;
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
        const float NewEfficiency = FMath::Clamp(StartEfficiency + Stream.FRandRange(-MaxDelta, MaxDelta), 0.f, 100.f);
        const float NewResourceLevel = FMath::Clamp(StartResourceLevel + Stream.FRandRange(-MaxDelta, MaxDelta), 0.f, 100.f);

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

    Efficiency = NewEfficiency;
    ResourceLevel = NewResourceLevel;

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
    const float ClampedLevel = FMath::Clamp(Level, 0.f, 100.f);
    ApplyValues(ClampedLevel, ClampedLevel);
}
