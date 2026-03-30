#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AR_EnemyBase.generated.h"

class UAR_HealthComponent;

UCLASS()
class ALTREBELLION_API AAR_EnemyBase : public ACharacter
{
    GENERATED_BODY()

public:
    AAR_EnemyBase();

    // Компонент здоровья
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAR_HealthComponent> HealthComponent;

    // Урон при атаке
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackDamage = 10.0f;

    // Дальность атаки
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRange = 150.0f;

    // Кулдаун атаки
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackCooldown = 1.5f;

    // Скорость передвижения
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MoveSpeed = 300.0f;

    // Очки опыта/лута за убийство
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    int32 TokenReward = 5;

    // Атаковать цель
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PerformAttack(AActor* Target);

    // Смерть врага
    UFUNCTION(BlueprintNativeEvent, Category = "Enemy")
    void OnEnemyDeath(AActor* DeadActor);
    virtual void OnEnemyDeath_Implementation(AActor* DeadActor);

protected:
    virtual void BeginPlay() override;

private:
    float LastAttackTime = -999.0f;
};