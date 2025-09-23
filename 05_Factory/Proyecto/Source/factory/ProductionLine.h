#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProductionLine.generated.h"

// Enum de estados de producción
UENUM(BlueprintType)
enum class EProductionState : uint8
{
    Operational UMETA(DisplayName = "Operational"),
    Warning     UMETA(DisplayName = "Warning"),
    Critical    UMETA(DisplayName = "Critical")
};

// Delegado para cambios en el estado de la producción
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProductionStateChanged, EProductionState, NewState, int32, LineID);

UCLASS()
class FACTORY_API AProductionLine : public AActor
{
    GENERATED_BODY()

public:
    // Constructor
    AProductionLine();
    virtual void BeginPlay() override;

    // Comienza la monitorización de la línea de producción
    void InitiateMonitoring();

    // Reinicia la línea de producción
    void ResetProductionLine()
    {
        Efficiency = ResourceLevel = 100.0f;
        CurrentState = EProductionState::Operational;
    }

    // Delegado para notificar cambios en el estado de la línea
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnProductionStateChanged OnStateChanged;

    // ID único de la línea de producción
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production")
    int32 LineID;

    // Eficiencia de la línea de producción
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production")
    float Efficiency;

    // Nivel de recursos de la línea de producción
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production")
    float ResourceLevel;

    // Estado actual de la línea de producción
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Production")
    EProductionState CurrentState;

    UFUNCTION(BlueprintCallable)
    void ResetLine();

private:
    
    void UpdateData();            // Actualiza los parámetros de la línea de producción    
    void EvaluateState();              // Determina el estado actual de la línea de producción    
    FTimerHandle Timer_Update;    // Temporizador para actualizar los parámetros a intervalos
};