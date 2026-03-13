#include "AR_PlayerController.h"

AAR_PlayerController::AAR_PlayerController()
{
    bShowMouseCursor = true;
}

void AAR_PlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void AAR_PlayerController::TogglePause()
{
    bIsPaused = !bIsPaused;
    SetPause(bIsPaused);
    // Позже здесь: показать/скрыть меню паузы
    UE_LOG(LogTemp, Warning, TEXT("Pause: %s"), bIsPaused ? TEXT("ON") : TEXT("OFF"));
}