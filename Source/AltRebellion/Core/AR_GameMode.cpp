#include "Core/AR_GameMode.h"
#include "Characters/AR_CharacterBase.h"
#include "Core/AR_GameInstance.h"
#include "Interactables/AR_InteractableNPC.h"
#include "Kismet/GameplayStatics.h"

AAR_GameMode::AAR_GameMode()
{
}

UClass* AAR_GameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    const UAR_GameInstance* GI = Cast<UAR_GameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GI || GI->SelectedCharacter.IsNone())
    {
        return Super::GetDefaultPawnClassForController_Implementation(InController);
    }

    const TSubclassOf<AAR_CharacterBase>* SelectedPawnClass =
        CharacterPawnClasses.Find(GI->SelectedCharacter);

    if (!SelectedPawnClass || !SelectedPawnClass->Get())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("No pawn class configured for selected character: %s"),
            *GI->SelectedCharacter.ToString()
        );

        return Super::GetDefaultPawnClassForController_Implementation(InController);
    }

    return SelectedPawnClass->Get();
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
