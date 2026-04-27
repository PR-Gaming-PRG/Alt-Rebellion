#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AR_AbilityUpgradeData.generated.h"

USTRUCT(BlueprintType)
struct FAR_AbilityUpgradeRow : public FTableRowBase
{
    GENERATED_BODY()

    // ID способности.
    // Например: Anya_RocketBarrage, Lena_Dash, Alisa_Trap
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Upgrade")
    FName AbilityID;

    // Уровень, который покупается этой строкой.
    // Например, если Level = 1, значит эта строка покупает первый уровень.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Upgrade")
    int32 Level = 1;

    // Цена улучшения в токенах
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Upgrade")
    int32 TokenCost = 1;

    // Название для UI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Upgrade")
    FText DisplayName;

    // Описание для UI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Upgrade")
    FText Description;

    // Бонус к урону.
    // Пока просто храним как данные, применять будем позже.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Upgrade")
    float DamageMultiplierBonus = 0.0f;

    // Множитель кулдауна.
    // Например 0.9 = кулдаун стал на 10% меньше.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Upgrade")
    float CooldownMultiplier = 1.0f;
};