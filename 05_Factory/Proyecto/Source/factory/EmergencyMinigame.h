#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmergencyMiniGame.generated.h"

UCLASS()
class FACTORY_API UEmergencyMiniGame : public UUserWidget
{
    GENERATED_BODY()

public:
    // Inicia la secuencia de emergencia para una línea de producción específica
    UFUNCTION(BlueprintCallable, Category = "Emergency")
    void LaunchEmergencySequence(int32 LineID);

    // Reinicia el estado de la línea de producción
    UFUNCTION(BlueprintCallable, Category = "Emergency")
    void ReinitializeProductionLine();

private:
    
    int32 AssignedLineID;                   // ID de la línea objetivo para la secuencia de emergencia    
    void OnFailureEvent();                  // Acción a ejecutar cuando la emergencia falla
    FTimerHandle EmergencyFailureTimer;     // Temporizador para manejar el tiempo de fallo de la emergencia    

};
