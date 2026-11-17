#include "CentralManager.h"
#include "factory.h"
#include "UIFactoryHUD.h"
#include "Algo/Sort.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ACentralManager::ACentralManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ACentralManager::BeginPlay()
{
    Super::BeginPlay();

    // UI-driven simulation: show the cursor and route input to the widgets.
    if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
    {
        PlayerController->bShowMouseCursor = true;
        PlayerController->SetInputMode(FInputModeUIOnly());
    }

    // Register every production line in the level and subscribe to its state changes.
    for (TActorIterator<AProductionLine> It(GetWorld()); It; ++It)
    {
        AProductionLine* Line = *It;
        ProductionLines.Add(Line);
        ProductionLineStates.Add(Line->LineID, Line->CurrentState);
        Line->OnStateChanged.AddUniqueDynamic(this, &ACentralManager::OnProductionLineStateChanged);
    }

    // Actor iteration order is not guaranteed: sort so that HUD slot N always shows the same line.
    Algo::Sort(ProductionLines, [](const TObjectPtr<AProductionLine>& A, const TObjectPtr<AProductionLine>& B)
    {
        return A->LineID < B->LineID;
    });

    UE_LOG(LogFactory, Log, TEXT("%s registered %d production lines."), *GetName(), ProductionLines.Num());

    if (!UIFactoryHUDClass)
    {
        UE_LOG(LogFactory, Warning, TEXT("%s: UIFactoryHUDClass is not set, the HUD will not be created."), *GetName());
        return;
    }

    UIFactoryHUD = CreateWidget<UUIFactoryHUD>(GetWorld(), UIFactoryHUDClass);
    if (UIFactoryHUD)
    {
        UIFactoryHUD->AddToViewport();
        UIFactoryHUD->AssignLines(ProductionLines);
    }
}

void ACentralManager::OnProductionLineStateChanged(EProductionState NewState, int32 LineID)
{
    ProductionLineStates.Add(LineID, NewState);

    const FString StateName = UEnum::GetDisplayValueAsText(NewState).ToString();
    UE_LOG(LogFactory, Log, TEXT("Line %d state changed to %s"), LineID, *StateName);

    if (bShowOnScreenDebug && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Line %d state changed to %s"), LineID, *StateName));
    }

    if (NewState == EProductionState::Critical)
    {
        DetectEmergencies();
    }
}

void ACentralManager::DetectEmergencies()
{
    if (UIFactoryHUD)
    {
        UIFactoryHUD->UpdateWarningVisibility(true);
    }
}
