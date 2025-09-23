#include "ProductionLine.h"
#include "Async/Async.h"

AProductionLine::AProductionLine()
{
    PrimaryActorTick.bCanEverTick = false;
    Efficiency = 100.0f;
    ResourceLevel = 100.0f;
    CurrentState = EProductionState::Operational;
    LineID = 0;
}
void AProductionLine::BeginPlay()
{
    Super::BeginPlay();
    InitiateMonitoring();  // Inicia la monitorización de parámetros
}

void AProductionLine::InitiateMonitoring()
{
    // Establece un temporizador para actualizar los parámetros cada segundo
    GetWorld()->GetTimerManager().SetTimer(Timer_Update, this, &AProductionLine::UpdateData, 1.0f, true);
}

void AProductionLine::UpdateData()
{
    // Ejecuta en un hilo del pool para evitar bloquear el hilo principal
    Async(EAsyncExecution::ThreadPool, [this]() 
        {
        // Genera deltas aleatorios para eficiencia y nivel de recursos
        float EfficiencyDelta   = FMath::RandRange(-5.0f, 5.0f);
        float ResourceDelta     = FMath::RandRange(-5.0f, 5.0f);

        // Calcula los nuevos valores para eficiencia y recursos
        float NewEfficiency     = FMath::Clamp(Efficiency + EfficiencyDelta, 0.0f, 100.0f);
        float NewResource       = FMath::Clamp(ResourceLevel + ResourceDelta, 0.0f, 100.0f);

        // Determina el nuevo estado de la producción según los valores calculados
        EProductionState NewState;
        if (NewEfficiency >= 75.0f && NewResource >= 75.0f)
            NewState = EProductionState::Operational;
        else if (NewEfficiency >= 50.0f && NewResource >= 50.0f)
            NewState = EProductionState::Warning;
        else
            NewState = EProductionState::Critical;

        // Devuelve al hilo principal para actualizar las variables y comprobar el estado
        AsyncTask(ENamedThreads::GameThread, [this, NewEfficiency, NewResource, NewState]() 
            {
            Efficiency = NewEfficiency;
            ResourceLevel = NewResource;

            // Si el estado cambia, emite el delegado de cambio de estado
            if (NewState != CurrentState)
            {
                CurrentState = NewState;
                OnStateChanged.Broadcast(NewState, LineID);
            }
            });
        });
}

void AProductionLine::ResetLine()
{
    Efficiency = 100.f;
    ResourceLevel = 100.f;
    CurrentState = EProductionState::Operational;
}