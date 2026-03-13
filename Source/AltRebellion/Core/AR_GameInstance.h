#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AR_GameInstance.generated.h"

UCLASS()
class ALTREBELLION_API UAR_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    // Выбранный персонаж (задаётся в меню выбора)
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    FName SelectedCharacter;

    // Имя слота сохранения
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    FString SaveSlotName = TEXT("SaveSlot_01");

    // Текущая зона (для отслеживания прогресса)
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    FName CurrentZoneID;
};