#include "UIFactoryHUD.h"
#include "factory.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UUIFactoryHUD::NativeConstruct()
{
    Super::NativeConstruct();

    // Slots are found by naming convention. A missing widget is stored as nullptr so that
    // index N always matches line N (the arrays never shift).
    EfficiencyBars.Reset();
    ResourceBars.Reset();
    LineStatusTexts.Reset();

    for (int32 i = 0; i < NumLines; ++i)
    {
        EfficiencyBars.Add(Cast<UProgressBar>(GetWidgetFromName(FName(*FString::Printf(TEXT("EfficiencyBar_Line%d"), i)))));
        ResourceBars.Add(Cast<UProgressBar>(GetWidgetFromName(FName(*FString::Printf(TEXT("ResourceBar_Line%d"), i)))));
        LineStatusTexts.Add(Cast<UTextBlock>(GetWidgetFromName(FName(*FString::Printf(TEXT("LineStatusText%d"), i)))));
    }

    if (EmergencyWarning)
    {
        EmergencyWarning->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UUIFactoryHUD::NativeDestruct()
{
    for (AProductionLine* Line : ProductionLines)
    {
        if (IsValid(Line))
        {
            Line->OnValuesUpdated.RemoveDynamic(this, &UUIFactoryHUD::HandleLineValuesUpdated);
        }
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TimerHandle_Warning);
    }

    Super::NativeDestruct();
}

void UUIFactoryHUD::AssignLines(const TArray<TObjectPtr<AProductionLine>>& Lines)
{
    ProductionLines.Reset();

    for (AProductionLine* Line : Lines)
    {
        if (!IsValid(Line))
        {
            continue;
        }

        const int32 Index = ProductionLines.Add(Line);
        Line->OnValuesUpdated.AddUniqueDynamic(this, &UUIFactoryHUD::HandleLineValuesUpdated);
        UpdateLineDisplay(Index, Line->Efficiency, Line->ResourceLevel, Line->CurrentState);
    }
}

AProductionLine* UUIFactoryHUD::GetLine(int32 Index) const
{
    if (ProductionLines.IsValidIndex(Index))
    {
        AProductionLine* Line = ProductionLines[Index].Get();
        return IsValid(Line) ? Line : nullptr;
    }
    return nullptr;
}

float UUIFactoryHUD::ReceiveEfficiency(int32 index)
{
    const AProductionLine* Line = GetLine(index);
    return Line ? Line->Efficiency / 100.f : 0.f;
}

float UUIFactoryHUD::ReceiveProgress(int32 index)
{
    const AProductionLine* Line = GetLine(index);
    return Line ? Line->ResourceLevel / 100.f : 0.f;
}

FText UUIFactoryHUD::ReceiveState(int32 index)
{
    const AProductionLine* Line = GetLine(index);
    return Line ? UEnum::GetDisplayValueAsText(Line->CurrentState) : FText::GetEmpty();
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

    if (bIsVisible)
    {
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().SetTimer(TimerHandle_Warning, this, &UUIFactoryHUD::HideWarningVisibility, WarningDuration, false);
        }
    }
}

void UUIFactoryHUD::ResetFactory()
{
    bIsAtCriticalState = false;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TimerHandle_Warning);
    }

    // ResetLine goes through the line's single update path, so the manager and the HUD are notified.
    for (AProductionLine* Line : ProductionLines)
    {
        if (IsValid(Line))
        {
            Line->ResetLine();
        }
    }
}

void UUIFactoryHUD::HandleLineValuesUpdated(AProductionLine* Line)
{
    const int32 Index = ProductionLines.IndexOfByKey(Line);
    if (Index != INDEX_NONE)
    {
        UpdateLineDisplay(Index, Line->Efficiency, Line->ResourceLevel, Line->CurrentState);
    }
}

void UUIFactoryHUD::UpdateLineDisplay(int32 Index, float Efficiency, float Resource, EProductionState State)
{
    if (EfficiencyBars.IsValidIndex(Index) && EfficiencyBars[Index])
    {
        EfficiencyBars[Index]->SetPercent(Efficiency / 100.0f);
    }

    if (ResourceBars.IsValidIndex(Index) && ResourceBars[Index])
    {
        ResourceBars[Index]->SetPercent(Resource / 100.0f);
    }

    if (LineStatusTexts.IsValidIndex(Index) && LineStatusTexts[Index])
    {
        LineStatusTexts[Index]->SetText(UEnum::GetDisplayValueAsText(State));
    }
}

void UUIFactoryHUD::ShowEmergencyPanel(int32 LineID)
{
    if (EmergencyWarning)
    {
        EmergencyWarning->SetVisibility(ESlateVisibility::Visible);
    }
}
