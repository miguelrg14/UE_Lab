#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProductionLine.h"
#include "CentralManager.generated.h"

class UUIFactoryHUD;

/**
 * Central manager of the factory. Registers every AProductionLine in the level, listens to their
 * state changes through their delegates, creates the HUD and raises the emergency warning when a
 * line becomes Critical.
 */
UCLASS()
class FACTORY_API ACentralManager : public AActor
{
    GENERATED_BODY()

public:
    ACentralManager();

    /** Bound to every line's OnStateChanged delegate. */
    UFUNCTION()
    void OnProductionLineStateChanged(EProductionState NewState, int32 LineID);

    /** Last known state of each line, keyed by LineID. */
    UPROPERTY(BlueprintReadOnly, Category = "Production")
    TMap<int32, EProductionState> ProductionLineStates;

    /** Widget class used for the HUD. Must be set in the editor (BP_FactoryManager). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "User Interface")
    TSubclassOf<UUIFactoryHUD> UIFactoryHUDClass;

    /** Also print state changes on screen (they always go to the LogFactory category). */
    UPROPERTY(EditAnywhere, Category = "Debug")
    bool bShowOnScreenDebug = true;

protected:
    virtual void BeginPlay() override;

    /** Raises the emergency warning on the HUD. */
    void DetectEmergencies();

private:
    /** HUD instance. UPROPERTY so the garbage collector tracks the reference. */
    UPROPERTY(Transient)
    TObjectPtr<UUIFactoryHUD> UIFactoryHUD;

    /** Registered lines, sorted by LineID. */
    UPROPERTY(Transient)
    TArray<TObjectPtr<AProductionLine>> ProductionLines;
};
