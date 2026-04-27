#include "Core/AR_GameMode.h"
#include "Core/AR_GameInstance.h"
#include "Interactables/AR_InteractableNPC.h"
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

    TArray<AActor*> FoundNPCs;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        AAR_InteractableNPC::StaticClass(),
        FoundNPCs
    );

    for (AActor* Actor : FoundNPCs)
    {
        AAR_InteractableNPC* NPC = Cast<AAR_InteractableNPC>(Actor);

        if (NPC)
        {
            NPC->SetInteractionEnabled(true);
        }
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Upgrade NPCs enabled: %d"),
        FoundNPCs.Num()
    );
}