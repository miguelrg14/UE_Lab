#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProductionLine.h"
#include "UIFactoryHUD.generated.h"

class UProgressBar;
class UTextBlock;
class UPanelWidget;
class UWidget;

UCLASS()
class FACTORY_API UUIFactoryHUD : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    UFUNCTION(BlueprintCallable, Category = "Functions")
    float ReceiveEfficiency(int32 index);

    UFUNCTION(BlueprintCallable, Category = "Functions")
    float ReceiveProgress(int32 index);

    UFUNCTION(BlueprintCallable, Category = "Functions")
    FText ReceiveState(int32 index);

    UFUNCTION(BlueprintCallable, Category = "Functions")
    ESlateVisibility CriticalMessageVisibility();

    UFUNCTION(BlueprintCallable, Category = "Functions")
    void ResetFactory();

    void UpdateWarningVisibility(bool bIsVisible);
    void HideWarningVisibility();

    void UpdateLineDisplay(int32 LineID, float Efficiency, float Resource, EProductionState State);
    void ShowEmergencyPanel(int32 LineID);
    void AssignLines(TArray<AProductionLine*> lines);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UProgressBar*> EfficiencyBars;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UProgressBar*> ResourceBars;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UTextBlock*> LineStatusTexts;

private:
    bool bIsAtCriticalState = false;
    FTimerHandle TimerHandle_Warning;

    TArray<AProductionLine*> productionLines;
    UWidget* EmergencyWarning;
};