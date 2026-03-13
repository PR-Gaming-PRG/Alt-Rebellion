#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AR_SaveGame.generated.h"

// Структура прогресса по одной зоне
USTRUCT(BlueprintType)
struct FZoneProgress
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Zone")
    FName ZoneID;

    UPROPERTY(BlueprintReadWrite, Category = "Zone")
    bool bLiberated = false;
};

UCLASS()
class ALTREBELLION_API UAR_SaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    // Выбранный персонаж
    UPROPERTY(BlueprintReadWrite, Category = "Save")
    FName SelectedCharacter;

    // Прогресс по зонам
    UPROPERTY(BlueprintReadWrite, Category = "Save")
    TArray<FZoneProgress> ZoneStates;

    // Ресурсы: жетоны, краска и т.д.
    UPROPERTY(BlueprintReadWrite, Category = "Save")
    TMap<FName, int32> Resources;

    // Разблокированные способности
    UPROPERTY(BlueprintReadWrite, Category = "Save")
    TArray<FName> UnlockedAbilities;

    // Настройки: громкость, чувствительность
    UPROPERTY(BlueprintReadWrite, Category = "Save")
    TMap<FName, float> Settings;
};