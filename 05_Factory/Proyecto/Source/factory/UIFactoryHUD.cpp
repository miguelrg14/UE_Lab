#include "UIFactoryHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UUIFactoryHUD::NativeConstruct()
{
    Super::NativeConstruct();

    for (int32 i = 0; i < 3; ++i)
    {
        FString EffName = FString::Printf(TEXT("EfficiencyBar_Line%d"), i);
        FString ResName = FString::Printf(TEXT("ResourceBar_Line%d"), i);
        FString TxtName = FString::Printf(TEXT("LineStatusText%d"), i); // Optional

        if (UProgressBar* Eff = Cast<UProgressBar>(GetWidgetFromName(FName(*EffName))))
        {
            EfficiencyBars.Add(Eff);
        }

        if (UProgressBar* Res = Cast<UProgressBar>(GetWidgetFromName(FName(*ResName))))
        {
            ResourceBars.Add(Res);
        }

        if (UTextBlock* Txt = Cast<UTextBlock>(GetWidgetFromName(FName(*TxtName))))
        {
            LineStatusTexts.Add(Txt);
        }
    }

    EmergencyWarning = GetWidgetFromName(TEXT("EmergencyWarning"));
    if (EmergencyWarning)
    {
        EmergencyWarning->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UUIFactoryHUD::AssignLines(TArray<AProductionLine*> lines)
{
    productionLines = lines;
}

float UUIFactoryHUD::ReceiveEfficiency(int32 index)
{
    return productionLines[index]->Efficiency / 100.f;
}

float UUIFactoryHUD::ReceiveProgress(int32 index)
{
    return productionLines[index]->ResourceLevel / 100.f;
}

FText UUIFactoryHUD::ReceiveState(int32 index)
{
    const UEnum* EnumPtr = StaticEnum<EProductionState>();
    return EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(productionLines[index]->CurrentState));
}

ESlateVisibility UUIFactoryHUD::CriticalMessageVisibility()
{
    return bIsAtCriticalState ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}

void UUIFactoryHUD::HideWarningVisibility()
{
    bIsAtCriticalState = false;
}

void UUIFactoryHUD::UpdateWarningVisibility(bool bIsVisible)
{
    bIsAtCriticalState = bIsVisible;

    if (bIsVisible && GetWorld())
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_Warning, this, &UUIFactoryHUD::HideWarningVisibility, 10.f, false);
}

void UUIFactoryHUD::ResetFactory()
{
    bIsAtCriticalState = false;

    for (AProductionLine* line : productionLines)
        line->ResetLine();
}

void UUIFactoryHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
}

void UUIFactoryHUD::UpdateLineDisplay(int32 LineID, float Efficiency, float Resource, EProductionState State)
{
    if (EfficiencyBars.IsValidIndex(LineID))
        EfficiencyBars[LineID]->SetPercent(Efficiency / 100.0f);

    if (ResourceBars.IsValidIndex(LineID))
        ResourceBars[LineID]->SetPercent(Resource / 100.0f);

    if (LineStatusTexts.IsValidIndex(LineID))
    {
        FString StateStr = UEnum::GetDisplayValueAsText(State).ToString();
        LineStatusTexts[LineID]->SetText(FText::FromString(StateStr));
    }
}

void UUIFactoryHUD::ShowEmergencyPanel(int32 LineID)
{
    if (EmergencyWarning)
        EmergencyWarning->SetVisibility(ESlateVisibility::Visible);
}

