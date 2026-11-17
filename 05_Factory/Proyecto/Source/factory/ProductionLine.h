#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProductionLine.generated.h"

class AProductionLine;

/** Health states a production line can be in. */
UENUM(BlueprintType)
enum class EProductionState : uint8
{
    Operational UMETA(DisplayName = "Operational"),
    Warning     UMETA(DisplayName = "Warning"),
    Critical    UMETA(DisplayName = "Critical")
};

/** Broadcast on the game thread when a line changes state. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProductionStateChanged, EProductionState, NewState, int32, LineID);

/** Broadcast on the game thread every time a line refreshes its values (used to push data to the HUD). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProductionValuesUpdated, AProductionLine*, Line);

/**
 * A simulated production line.
 *
 * Every UpdateInterval seconds the new Efficiency / ResourceLevel values are computed on a
 * thread-pool task. The task works only on copies of the data; the results are applied back
 * on the game thread, where the state is evaluated and the delegates are broadcast.
 */
UCLASS()
class FACTORY_API AProductionLine : public AActor
{
    GENERATED_BODY()

public:
    AProductionLine();

    /** Starts the periodic simulation update. Called automatically from BeginPlay. */
    void InitiateMonitoring();

    /** Restores the line to 100% efficiency and resources. */
    UFUNCTION(BlueprintCallable, Category = "Production")
    void ResetLine();

    /** Sets efficiency and resources to Level (0-100), re-evaluates the state and notifies listeners. */
    UFUNCTION(BlueprintCallable, Category = "Production")
    void RestoreLine(float Level);

    /** State rule: both values >= InOperationalThreshold -> Operational, both >= InWarningThreshold -> Warning, else Critical. */
    static EProductionState ComputeState(float InEfficiency, float InResourceLevel, float InOperationalThreshold, float InWarningThreshold);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnProductionStateChanged OnStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnProductionValuesUpdated OnValuesUpdated;

    /** Unique ID of the line. The HUD shows lines sorted by this value. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production")
    int32 LineID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production", meta = (ClampMin = "0", ClampMax = "100"))
    float Efficiency;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production", meta = (ClampMin = "0", ClampMax = "100"))
    float ResourceLevel;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production")
    EProductionState CurrentState;

    /** Both values at or above this keep the line Operational. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production|Tuning", meta = (ClampMin = "0", ClampMax = "100"))
    float OperationalThreshold = 75.f;

    /** Both values at or above this (but below OperationalThreshold) put the line in Warning. Below it: Critical. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production|Tuning", meta = (ClampMin = "0", ClampMax = "100"))
    float WarningThreshold = 50.f;

    /** Seconds between simulation updates. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production|Tuning", meta = (ClampMin = "0.1"))
    float UpdateInterval = 1.f;

    /** Maximum random change applied to each value per update. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production|Tuning", meta = (ClampMin = "0"))
    float MaxDeltaPerUpdate = 5.f;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    /** Timer callback: launches the background computation. */
    void UpdateData();

    /** Game thread only: stores the new values, evaluates the state and broadcasts the delegates. */
    void ApplyValues(float NewEfficiency, float NewResourceLevel);

    FTimerHandle Timer_Update;
};
