#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AR_GameMode.generated.h"

UCLASS()
class ALTREBELLION_API AAR_GameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AAR_GameMode();

    // Вызывается когда все враги в волне уничтожены
    UFUNCTION(BlueprintCallable, Category = "Game Mode")
    void OnWaveCleared();

    // Вызывается при смерти игрока
    UFUNCTION(BlueprintCallable, Category = "Game Mode")
    void OnPlayerDied();

    // Вызывается при победе (босс убит)
    UFUNCTION(BlueprintCallable, Category = "Game Mode")
    void OnMissionComplete();

protected:
    // Текущий счётчик врагов на уровне
    UPROPERTY(BlueprintReadOnly, Category = "Game Mode")
    int32 EnemiesRemaining = 0;
};