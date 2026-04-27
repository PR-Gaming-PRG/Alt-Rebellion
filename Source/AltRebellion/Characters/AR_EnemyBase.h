#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AR_EnemyBase.generated.h"

class UAR_HealthComponent;
class AAR_LootDrop;
class AAR_EnemyBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnAnyEnemyKilled,
    AAR_EnemyBase*,
    KilledEnemy
);

UCLASS()
class ALTREBELLION_API AAR_EnemyBase : public ACharacter
{
    GENERATED_BODY()

public:
    AAR_EnemyBase();

    static FOnAnyEnemyKilled OnAnyEnemyKilled;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAR_HealthComponent> HealthComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float MoveSpeed = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Combat")
    float AttackDamage = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Combat")
    float AttackRange = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Combat")
    float AttackCooldown = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
    int32 TokenReward = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    TArray<TSubclassOf<AAR_LootDrop>> LootClasses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    float LootDropChance = 0.3f;

    UFUNCTION(BlueprintCallable, Category = "Enemy Combat")
    void PerformAttack(AActor* Target);

    UFUNCTION(BlueprintNativeEvent, Category = "Enemy")
    void OnEnemyDeath(AActor* DeadActor);

    virtual void OnEnemyDeath_Implementation(AActor* DeadActor);

protected:
    virtual void BeginPlay() override;

    float LastAttackTime = -999.0f;
};