#include "Core/AR_GameMode.h"

#include "Characters/AR_BossBase.h"
#include "Characters/AR_CharacterBase.h"
#include "Characters/AR_EnemyBase.h"
#include "Core/AR_GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Interactables/AR_InteractableNPC.h"
#include "Kismet/GameplayStatics.h"

AAR_GameMode::AAR_GameMode()
{
}

void AAR_GameMode::BeginPlay()
{
    Super::BeginPlay();

    CurrentEnemyKills = 0;
    bBossUnlocked = false;
    bMissionCompleted = false;
    SpawnedBoss = nullptr;

    AAR_EnemyBase::OnAnyEnemyKilled.AddDynamic(
        this,
        &AAR_GameMode::HandleAnyEnemyKilled
    );

    BP_OnKillProgressChanged(CurrentEnemyKills, RequiredEnemyKills);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Mission started. RequiredEnemyKills: %d, RequireBossKill: %s"),
        RequiredEnemyKills,
        bRequireBossKill ? TEXT("true") : TEXT("false")
    );
}

void AAR_GameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    AAR_EnemyBase::OnAnyEnemyKilled.RemoveDynamic(
        this,
        &AAR_GameMode::HandleAnyEnemyKilled
    );

    Super::EndPlay(EndPlayReason);
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

void AAR_GameMode::HandleAnyEnemyKilled(AAR_EnemyBase* KilledEnemy)
{
    if (!KilledEnemy || bMissionCompleted)
    {
        return;
    }

    // Если умер босс
    if (AAR_BossBase* Boss = Cast<AAR_BossBase>(KilledEnemy))
    {
        UE_LOG(LogTemp, Warning, TEXT("Boss killed: %s"), *Boss->GetName());

        BP_OnBossKilled();

        OnMissionComplete();
        return;
    }

    // Если цель с боссом уже открыта, обычные мобы больше не двигают прогресс
    if (bBossUnlocked)
    {
        return;
    }

    CurrentEnemyKills++;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Enemy kill progress: %d / %d"),
        CurrentEnemyKills,
        RequiredEnemyKills
    );

    BP_OnKillProgressChanged(CurrentEnemyKills, RequiredEnemyKills);

    if (CurrentEnemyKills >= RequiredEnemyKills)
    {
        UnlockBossObjective();
    }
}

void AAR_GameMode::UnlockBossObjective()
{
    if (bBossUnlocked)
    {
        return;
    }

    bBossUnlocked = true;

    UE_LOG(LogTemp, Warning, TEXT("Boss objective unlocked!"));

    BP_OnBossUnlocked();

    if (bSpawnBossAutomatically)
    {
        SpawnBoss();
    }

    if (!bRequireBossKill)
    {
        OnMissionComplete();
    }
}

void AAR_GameMode::SpawnBoss()
{
    if (!BossClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnBoss failed: BossClass is not set"));
        return;
    }

    if (!BossSpawnPoint)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnBoss failed: BossSpawnPoint is not set"));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    SpawnedBoss = GetWorld()->SpawnActor<AAR_BossBase>(
        BossClass,
        BossSpawnPoint->GetActorLocation(),
        BossSpawnPoint->GetActorRotation(),
        SpawnParams
    );

    if (SpawnedBoss)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Boss spawned: %s"),
            *SpawnedBoss->GetName()
        );
    }
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
    if (bMissionCompleted)
    {
        return;
    }

    bMissionCompleted = true;

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

    BP_OnMissionCompleted();

    if (bIsFinalLevel)
    {
        ShowGameCompletedScreen();
        return;
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

void AAR_GameMode::ShowGameCompletedScreen()
{
    if (!GameCompletedWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameCompletedWidgetClass is not set"));
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowGameCompletedScreen failed: PlayerController is null"));
        return;
    }

    UUserWidget* CompletedWidget = CreateWidget<UUserWidget>(PC, GameCompletedWidgetClass);

    if (!CompletedWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowGameCompletedScreen failed: widget was not created"));
        return;
    }

    CompletedWidget->AddToViewport();

    PC->SetPause(true);
    PC->bShowMouseCursor = true;

    FInputModeUIOnly InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(InputMode);

    UE_LOG(LogTemp, Warning, TEXT("Game completed screen opened"));
}