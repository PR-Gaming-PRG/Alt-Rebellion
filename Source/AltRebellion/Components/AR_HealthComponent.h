#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AR_HealthComponent.generated.h"

// Делегат который срабатывает при изменении HP
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
    FOnHealthChanged,
    UAR_HealthComponent*, HealthComp,
    float, Health,
    float, HealthDelta,
    const UDamageType*, DamageType
);

// Делегат который срабатывает при смерти
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnDeath,
    AActor*, DeadActor
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
    FOnDamageTaken,
    UAR_HealthComponent*, HealthComp,
    float, Damage,
    AActor*, DamageCauser,
    const UDamageType*, DamageType
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnDeathWithCauser,
    AActor*, DeadActor,
    AActor*, DamageCauser
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALTREBELLION_API UAR_HealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAR_HealthComponent();

    // Максимальное HP
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHealth = 100.0f;

    // Текущее HP
    UPROPERTY(BlueprintReadOnly, Category = "Health")
    float CurrentHealth;

    // Броня
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float Armor = 0.0f;

    // Делегаты
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnDeath OnDeath;

    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnDamageTaken OnDamageTaken;

    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnDeathWithCauser OnDeathWithCauser;

    // Применить урон
    UFUNCTION(BlueprintCallable, Category = "Health")
    void ApplyDamage(float DamageAmount, const UDamageType* DamageType);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void ApplyDamageWithCauser(float DamageAmount, const UDamageType* DamageType, AActor* DamageCauser);

    // Восстановить HP
    UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float HealAmount);

    // Мёртв ли актор
    UFUNCTION(BlueprintCallable, Category = "Health")
    bool IsDead() const;

    // Получить текущее HP в процентах (для UI)
    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Health")
    AActor* GetLastDamageCauser() const { return LastDamageCauser.Get(); }

protected:
    virtual void BeginPlay() override;

private:
    bool bIsDead = false;

    UPROPERTY()
    TWeakObjectPtr<AActor> LastDamageCauser;
};
