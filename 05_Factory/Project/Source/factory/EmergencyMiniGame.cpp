#include "EmergencyMiniGame.h"
#include "factory.h"
#include "ProductionLine.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"

void UEmergencyMiniGame::LaunchEmergencySequence(int32 LineID)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    AssignedLineID = LineID;

    UE_LOG(LogFactory, Warning, TEXT("Emergency sequence started for line %d (%.0f s to react)."), LineID, TimeToReact);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
            FString::Printf(TEXT("Emergency sequence initiated for Line %d. Reset within %.0f seconds!"), LineID, TimeToReact));
    }

    World->GetTimerManager().SetTimer(EmergencyFailureTimer, this, &UEmergencyMiniGame::OnFailureEvent, TimeToReact, false);
}

void UEmergencyMiniGame::ReinitializeProductionLine()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    World->GetTimerManager().ClearTimer(EmergencyFailureTimer);

    for (TActorIterator<AProductionLine> It(World); It; ++It)
    {
        if (It->LineID != AssignedLineID)
        {
            continue;
        }

        // Single update path: RestoreLine re-evaluates the state and notifies the manager and the HUD.
        It->RestoreLine(RecoveryLevel);

        UE_LOG(LogFactory, Log, TEXT("Line %d restored to %.0f%%."), AssignedLineID, RecoveryLevel);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
                FString::Printf(TEXT("Line %d successfully reset to Operational."), AssignedLineID));
        }
        return;
    }

    UE_LOG(LogFactory, Warning, TEXT("ReinitializeProductionLine: no production line with ID %d."), AssignedLineID);
}

void UEmergencyMiniGame::OnFailureEvent()
{
    UE_LOG(LogFactory, Error, TEXT("Failed to reset line %d in time. Critical failure logged."), AssignedLineID);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
            FString::Printf(TEXT("FAILED to reset Line %d! Critical failure logged."), AssignedLineID));
    }

    OnEmergencyFailed(AssignedLineID);
}

void UEmergencyMiniGame::NativeDestruct()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(EmergencyFailureTimer);
    }

    Super::NativeDestruct();
}
