#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProductionLine.h"
#include "UIFactoryHUD.generated.h"

class UProgressBar;
class UTextBlock;
class UWidget;

/**
 * Main HUD: shows efficiency, resources and state for each production line.
 *
 * Values are pushed from the lines through their OnValuesUpdated delegate. The Receive* functions
 * are kept (same names and parameters) because the widget Blueprint binds to them.
 */
UCLASS()
class FACTORY_API UUIFactoryHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Functions")
    float ReceiveEfficiency(int32 index);

    UFUNCTION(BlueprintCallable, Category = "Functions")
    float ReceiveProgress(int32 index);

    UFUNCTION(BlueprintCallable, Category = "Functions")
    FText ReceiveState(int32 index);

    UFUNCTION(BlueprintCallable, Category = "Functions")
    ESlateVisibility CriticalMessageVisibility();

    /** Resets every line to 100% and hides the warning. */
    UFUNCTION(BlueprintCallable, Category = "Functions")
    void ResetFactory();

    /** Shows (or hides) the critical warning. When shown, it hides itself after WarningDuration seconds. */
    void UpdateWarningVisibility(bool bIsVisible);
    void HideWarningVisibility();

    /** Updates bars and text of the slot at Index. */
    void UpdateLineDisplay(int32 Index, float Efficiency, float Resource, EProductionState State);
    void ShowEmergencyPanel(int32 LineID);

    /** Receives the lines (sorted by LineID) and subscribes to their value updates. */
    void AssignLines(const TArray<TObjectPtr<AProductionLine>>& Lines);

    /** Number of line slots in the layout (widgets EfficiencyBar_LineN, ResourceBar_LineN, LineStatusTextN). */
    UPROPERTY(EditDefaultsOnly, Category = "HUD", meta = (ClampMin = "1"))
    int32 NumLines = 3;

    /** Seconds the critical warning stays visible. */
    UPROPERTY(EditDefaultsOnly, Category = "HUD", meta = (ClampMin = "0"))
    float WarningDuration = 10.f;

    UPROPERTY(Transient, BlueprintReadWrite, Category = "HUD")
    TArray<TObjectPtr<UProgressBar>> EfficiencyBars;

    UPROPERTY(Transient, BlueprintReadWrite, Category = "HUD")
    TArray<TObjectPtr<UProgressBar>> ResourceBars;

    UPROPERTY(Transient, BlueprintReadWrite, Category = "HUD")
    TArray<TObjectPtr<UTextBlock>> LineStatusTexts;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    /** Bound automatically to the widget named "EmergencyWarning" in the Blueprint, if it exists. */
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UWidget> EmergencyWarning;

private:
    UFUNCTION()
    void HandleLineValuesUpdated(AProductionLine* Line);

    /** Returns the line at Index, or nullptr if the index is out of range or the line is gone. */
    AProductionLine* GetLine(int32 Index) const;

    bool bIsAtCriticalState = false;
    FTimerHandle TimerHandle_Warning;

    UPROPERTY(Transient)
    TArray<TObjectPtr<AProductionLine>> ProductionLines;
};
