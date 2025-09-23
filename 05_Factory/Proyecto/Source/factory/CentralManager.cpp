#include "CentralManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "UIFactoryHUD.h"

ACentralManager::ACentralManager()
{
    PrimaryActorTick.bCanEverTick = false;
}
void ACentralManager::BeginPlay()
{
    Super::BeginPlay();

    // Coger el controlador del personaje para modificar sus opciones
    auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = true;
        PlayerController->SetInputMode(FInputModeUIOnly());
    }

    // Recuperar todas las lineas de producción del mundo
    TArray<AActor*> FoundActors;
    TArray<AProductionLine*> ProductionLines;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProductionLine::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        AProductionLine* Line = Cast<AProductionLine>(Actor);
        ProductionLines.Add(Line);
        if (Line)
            Line->OnStateChanged.AddDynamic(this, &ACentralManager::OnProductionLineStateChanged);
    }

    // Ver si UIFactoryHUD es válido antes de crear el widget de la HUD
    if (UIFactoryHUDClass)
    {
        // Instanciar el widget de la HUD
        UIFactoryHUD = CreateWidget<UUIFactoryHUD>(GetWorld(), UIFactoryHUDClass);
        if (UIFactoryHUD)
        {
            UIFactoryHUD->AddToViewport();
            UIFactoryHUD->AssignLines(ProductionLines);
        }
    }
}

void ACentralManager::OnProductionLineStateChanged(EProductionState NewState, int32 LineID)
{
    ProductionLineStates.Add(LineID, NewState);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Line %d state changed to %s"), LineID, *UEnum::GetValueAsString(NewState)));

    if (NewState == EProductionState::Critical)
        DetectEmergencies();
}

void ACentralManager::DetectEmergencies()
{
    UIFactoryHUD->UpdateWarningVisibility(true);
}