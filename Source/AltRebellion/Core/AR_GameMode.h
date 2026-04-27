#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AR_GameMode.generated.h"

class AAR_CharacterBase;
class AAR_EnemyBase;
class AAR_BossBase;
class UUserWidget;

UCLASS()
class ALTREBELLION_API AAR_GameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AAR_GameMode();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

    // Соответствие CharacterID из GameInstance и Blueprint-класса игрока для спавна на уровне
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Characters", meta = (DisplayName = "Character Pawn Classes"))
    TMap<FName, TSubclassOf<AAR_CharacterBase>> CharacterPawnClasses;

    // Вызывается когда все враги в волне уничтожены
    UFUNCTION(BlueprintCallable, Category = "Game Mode")
    void OnWaveCleared();

    // Вызывается при смерти игрока
    UFUNCTION(BlueprintCallable, Category = "Game Mode")
    void OnPlayerDied();

    // Вызывается при победе на уровне
    UFUNCTION(BlueprintCallable, Category = "Game Mode")
    void OnMissionComplete();

protected:
    // Сколько обычных врагов надо убить, чтобы открыть босса / завершить уровень
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
    int32 RequiredEnemyKills = 5;

    // Нужно ли убить босса после убийства обычных мобов
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
    bool bRequireBossKill = false;

    // Если true — после нужного числа убийств босс появится автоматически
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
    bool bSpawnBossAutomatically = false;

    // Класс босса для автоспавна
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss", meta = (EditCondition = "bSpawnBossAutomatically"))
    TSubclassOf<AAR_BossBase> BossClass;

    // Точка спавна босса
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss", meta = (EditCondition = "bSpawnBossAutomatically"))
    AActor* BossSpawnPoint = nullptr;

    // Сколько обычных врагов уже убито
    UPROPERTY(BlueprintReadOnly, Category = "Mission")
    int32 CurrentEnemyKills = 0;

    // Открыта ли цель с боссом / проход к боссу
    UPROPERTY(BlueprintReadOnly, Category = "Mission")
    bool bBossUnlocked = false;

    // Завершена ли миссия
    UPROPERTY(BlueprintReadOnly, Category = "Mission")
    bool bMissionCompleted = false;

    // Заспавненный босс, если используется автоспавн
    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    TObjectPtr<AAR_BossBase> SpawnedBoss;

    // Старое поле можно оставить, если где-то уже используется
    UPROPERTY(BlueprintReadOnly, Category = "Game Mode")
    int32 EnemiesRemaining = 0;

    // Финальный ли это уровень.
    // Если true, после завершения миссии открывается экран победы, а не Миша.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
    bool bIsFinalLevel = false;

    // Виджет финального экрана
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission")
    TSubclassOf<UUserWidget> GameCompletedWidgetClass;

protected:
    UFUNCTION()
    void HandleAnyEnemyKilled(AAR_EnemyBase* KilledEnemy);

    void UnlockBossObjective();

    void SpawnBoss();

    void ShowGameCompletedScreen();

    // Для Blueprint: обновить UI прогресса убийств
    UFUNCTION(BlueprintImplementableEvent, Category = "Mission")
    void BP_OnKillProgressChanged(int32 CurrentKills, int32 RequiredKills);

    // Для Blueprint: открыть проход к боссу / включить босса
    UFUNCTION(BlueprintImplementableEvent, Category = "Mission")
    void BP_OnBossUnlocked();

    // Для Blueprint: босс убит
    UFUNCTION(BlueprintImplementableEvent, Category = "Mission")
    void BP_OnBossKilled();

    // Для Blueprint: миссия завершена
    UFUNCTION(BlueprintImplementableEvent, Category = "Mission")
    void BP_OnMissionCompleted();
};