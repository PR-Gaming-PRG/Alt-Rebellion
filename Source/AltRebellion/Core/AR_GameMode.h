#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AR_GameMode.generated.h"

class AAR_CharacterBase;

UCLASS()
class ALTREBELLION_API AAR_GameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AAR_GameMode();

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

    // Вызывается при победе (босс убит)
    UFUNCTION(BlueprintCallable, Category = "Game Mode")
    void OnMissionComplete();

protected:
    // Текущий счётчик врагов на уровне
    UPROPERTY(BlueprintReadOnly, Category = "Game Mode")
    int32 EnemiesRemaining = 0;
};
