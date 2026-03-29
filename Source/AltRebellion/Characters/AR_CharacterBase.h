#pragma once

#include "CoreMinimal.h"
#include "AltRebellionCharacter.h"
#include "AR_CharacterBase.generated.h"

class UAR_HealthComponent;
class UAR_WeaponComponent;
class UAR_AbilityComponent;
class UAR_CharacterDataAsset;

UCLASS()
class ALTREBELLION_API AAR_CharacterBase : public AAltRebellionCharacter
{
    GENERATED_BODY()

public:
    AAR_CharacterBase();

    // Компоненты
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAR_HealthComponent> HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAR_WeaponComponent> WeaponComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAR_AbilityComponent> AbilityComponent;

    // Скорость передвижения
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MoveSpeed = 600.0f;

    // Множитель урона
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float DamageMultiplier = 1.0f;

    // Имя персонажа (совпадает с SelectedCharacter в GameInstance)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
    FName CharacterID;

    // Обработка движения
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void Move(FVector2D MovementVector);

    // Обработка стрельбы
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartShooting(FVector TargetLocation);

    // Смерть персонажа
    UFUNCTION(BlueprintNativeEvent, Category = "Character")
    void OnCharacterDeath(AActor* DeadActor);
    virtual void OnCharacterDeath_Implementation(AActor* DeadActor);
    
    // DataAsset с параметрами персонажа
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
    TObjectPtr<UAR_CharacterDataAsset> CharacterData;

protected:
    virtual void BeginPlay() override;
};