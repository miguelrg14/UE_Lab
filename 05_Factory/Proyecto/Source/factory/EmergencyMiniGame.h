#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmergencyMiniGame.generated.h"

/**
 * Emergency minigame widget: when a line goes Critical the player has TimeToReact seconds to
 * press the reset button. On success the line is restored to RecoveryLevel; otherwise the
 * failure is logged and OnEmergencyFailed is raised for the Blueprint.
 */
UCLASS()
class FACTORY_API UEmergencyMiniGame : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Starts the emergency countdown for the given line. */
    UFUNCTION(BlueprintCallable, Category = "Emergency")
    void LaunchEmergencySequence(int32 LineID);

    /** Called by the reset button: stops the countdown and restores the assigned line. */
    UFUNCTION(BlueprintCallable, Category = "Emergency")
    void ReinitializeProductionLine();

    /** Seconds the player has to react. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Emergency", meta = (ClampMin = "0.1"))
    float TimeToReact = 10.f;

    /** Efficiency / resource level the line is restored to on success. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Emergency", meta = (ClampMin = "0", ClampMax = "100"))
    float RecoveryLevel = 80.f;

protected:
    virtual void NativeDestruct() override;

    /** Raised when the player did not react in time (e.g. hide the button in the Blueprint). */
    UFUNCTION(BlueprintImplementableEvent, Category = "Emergency")
    void OnEmergencyFailed(int32 LineID);

private:
    void OnFailureEvent();

    /** Line targeted by the current emergency. */
    int32 AssignedLineID = 0;

    FTimerHandle EmergencyFailureTimer;
};
