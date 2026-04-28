#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AR_CharacterDataAsset.generated.h"

class UDataTable;
class UAnimationAsset;
class USkeletalMesh;

UCLASS(BlueprintType)
class ALTREBELLION_API UAR_CharacterDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    // Имя персонажа (отображается в UI)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
    FText DisplayName;

    // ID персонажа (совпадает с CharacterID в AR_CharacterBase)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
    FName CharacterID;

    // Описание для экрана выбора
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Preview")
    TObjectPtr<USkeletalMesh> PreviewSkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Preview")
    FVector PreviewMeshScale = FVector(1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Preview")
    TObjectPtr<UAnimationAsset> MainMenuAnimation;

    // --- Характеристики ---

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float MoveSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float Armor = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float DamageMultiplier = 1.0f;

    // --- Оружие ---

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float WeaponDamage = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float FireRate = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float Range = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float SpreadAngle = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    int32 AmmoPerClip = 30;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    float ReloadTime = 1.5f;

    // Таблица уровней способностей этого персонажа.
    // Строки должны использовать FAR_AbilityUpgradeRow.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Progression")
    TObjectPtr<UDataTable> AbilityUpgradeTable;

    // --- Иконка для UI ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UTexture2D> CharacterIcon;
};
