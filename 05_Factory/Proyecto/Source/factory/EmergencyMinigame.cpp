#include "EmergencyMiniGame.h"
#include "Kismet/GameplayStatics.h"
#include "ProductionLine.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

void UEmergencyMiniGame::LaunchEmergencySequence(int32 LineID)
{
    AssignedLineID = LineID;

    if (GEngine)
    {
        FString Message = FString::Printf(TEXT("Emergency sequence initiated for Line %d. Reset within 10 seconds!"), LineID);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Message);
    }

    // Set a 10-second timer to trigger failure
    GetWorld()->GetTimerManager().SetTimer(EmergencyFailureTimer, this, &UEmergencyMiniGame::OnFailureEvent, 10.0f, false);
}

void UEmergencyMiniGame::ReinitializeProductionLine()
{
    GetWorld()->GetTimerManager().ClearTimer(EmergencyFailureTimer);

    // Find the production line actor
    TArray<AActor*> Lines;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProductionLine::StaticClass(), Lines);

    for (AActor* Actor : Lines)
    {
        AProductionLine* Line = Cast<AProductionLine>(Actor);
        if (Line && Line->LineID == AssignedLineID)
        {
            Line->Efficiency = 80.0f;
            Line->ResourceLevel = 80.0f;
            Line->CurrentState = EProductionState::Operational;

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
                    FString::Printf(TEXT("Line %d successfully reset to Operational."), AssignedLineID));
            }

            // Optionally broadcast state change again manually:
            Line->OnStateChanged.Broadcast(EProductionState::Operational, Line->LineID);
            break;
        }
    }
}


void UEmergencyMiniGame::OnFailureEvent()
{
    if (GEngine)
    {
        FString Message = FString::Printf(TEXT("FAILED to reset Line %d! Critical failure logged."), AssignedLineID);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Message);
    }
}
