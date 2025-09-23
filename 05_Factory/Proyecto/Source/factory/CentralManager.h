#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProductionLine.h"
#include "UIFactoryHUD.h"
#include "CentralManager.generated.h"

UCLASS()
class FACTORY_API ACentralManager : public AActor
{
    GENERATED_BODY()

public:
    ACentralManager();
    virtual void BeginPlay() override;

    // Handles state changes for production lines and performs additional tasks
    UFUNCTION()
    void OnProductionLineStateChanged(EProductionState NewState, int32 LineID);

    // Stores the production states of each line in the factory
    UPROPERTY(BlueprintReadOnly)
    TMap<int32, EProductionState> ProductionLineStates;

    // UI reference exposed to the editor, allowing assignment of custom UI widgets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Interface")
    TSubclassOf<UUIFactoryHUD> UIFactoryHUDClass;

    // A pointer to the HUD widget instance
    UUIFactoryHUD* UIFactoryHUD;

protected:
    // Checks for emergency situations and triggers appropriate actions
    void DetectEmergencies();
};