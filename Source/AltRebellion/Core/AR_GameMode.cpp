#include "Core/AR_GameMode.h"
#include "Core/AR_GameInstance.h"
#include "Kismet/GameplayStatics.h"

AAR_GameMode::AAR_GameMode()
{
}

void AAR_GameMode::OnWaveCleared()
{
    UE_LOG(LogTemp, Warning, TEXT("Wave Cleared!"));

    UAR_GameInstance* GI = Cast<UAR_GameInstance>(UGameplayStatics::GetGameInstance(this));
    if (GI)
    {
        GI->SaveGame();
    }
}

void AAR_GameMode::OnPlayerDied()
{
    UE_LOG(LogTemp, Warning, TEXT("Player Died!"));

    UAR_GameInstance* GI = Cast<UAR_GameInstance>(UGameplayStatics::GetGameInstance(this));
    if (GI)
    {
        GI->SaveGame();
    }
}

void AAR_GameMode::OnMissionComplete()
{
    UE_LOG(LogTemp, Warning, TEXT("Mission Complete!"));

    UAR_GameInstance* GI = Cast<UAR_GameInstance>(UGameplayStatics::GetGameInstance(this));
    if (GI)
    {
        if (!GI->CurrentZoneID.IsNone())
        {
            GI->SetZoneLiberated(GI->CurrentZoneID, true);
        }

        GI->SaveGame();
    }
}