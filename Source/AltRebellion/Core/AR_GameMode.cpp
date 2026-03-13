#include "AR_GameMode.h"
#include "AR_GameInstance.h"
#include "Kismet/GameplayStatics.h"

AAR_GameMode::AAR_GameMode()
{
}

void AAR_GameMode::OnWaveCleared()
{
    // Позже здесь: автосохранение, спавн следующей волны
    UE_LOG(LogTemp, Warning, TEXT("Wave Cleared!"));
}

void AAR_GameMode::OnPlayerDied()
{
    // Позже здесь: показать экран смерти
    UE_LOG(LogTemp, Warning, TEXT("Player Died!"));
}

void AAR_GameMode::OnMissionComplete()
{
    // Позже здесь: сохранение, переход к следующей зоне
    UE_LOG(LogTemp, Warning, TEXT("Mission Complete!"));
}