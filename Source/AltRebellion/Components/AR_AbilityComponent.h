#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AR_AbilityComponent.generated.h"

// Базовый класс способности
UCLASS(Abstract, BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_AbilityBase : public UObject
{
    GENERATED_BODY()

public:
    // Геттер для кулдауна (используется в AbilityComponent)
    float GetLastActivationTime() const { return LastActivationTime; }

    // Время перезарядки
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Cooldown = 5.0f;

    // Длительность эффекта
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Duration = 2.0f;

    // Название способности
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FName AbilityName;

    // Активировать способность
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
    void Activate(AActor* Owner);
    virtual void Activate_Implementation(AActor* Owner) {}

    // Деактивировать способность
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
    void Deactivate();
    virtual void Deactivate_Implementation() {}

    // Готова ли способность
    UFUNCTION(BlueprintCallable, Category = "Ability")
    bool IsReady() const;

    // Сбросить кулдаун (вызывается после активации)
    void StartCooldown();

protected:
    float LastActivationTime = -999.0f;
};

// Компонент который управляет способностями персонажа
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALTREBELLION_API UAR_AbilityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAR_AbilityComponent();

    // Слоты способностей
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TObjectPtr<UAR_AbilityBase> Ability1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TObjectPtr<UAR_AbilityBase> Ability2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TObjectPtr<UAR_AbilityBase> Ultimate;

    // Активировать способность по слоту
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void ActivateAbility1();

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void ActivateAbility2();

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void ActivateUltimate();

    // Получить прогресс кулдауна для UI (0.0 - 1.0)
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    float GetAbility1CooldownPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    float GetAbility2CooldownPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    float GetUltimateCooldownPercent() const;
};