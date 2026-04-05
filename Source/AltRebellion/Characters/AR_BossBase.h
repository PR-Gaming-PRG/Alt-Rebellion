#pragma once

#include "CoreMinimal.h"
#include "Characters/AR_EnemyBase.h"
#include "AR_BossBase.generated.h"

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
    Phase1 UMETA(DisplayName = "Phase 1 - Full Health"),
    Phase2 UMETA(DisplayName = "Phase 2 - 50% Health"),
    Phase3 UMETA(DisplayName = "Phase 3 - 25% Health")
};

// Делегат для смены фазы
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnBossPhaseChanged,
    EBossPhase, OldPhase,
    EBossPhase, NewPhase
);

UCLASS()
class ALTREBELLION_API AAR_BossBase : public AAR_EnemyBase
{
    GENERATED_BODY()

public:
    AAR_BossBase();

    // Текущая фаза босса
    UPROPERTY(BlueprintReadOnly, Category = "Boss")
    EBossPhase CurrentPhase = EBossPhase::Phase1;

    // Делегат смены фазы
    UPROPERTY(BlueprintAssignable, Category = "Boss")
    FOnBossPhaseChanged OnPhaseChanged;

    // Сколько врагов спавнить при смене фазы
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    int32 ReinforcementsPerPhase = 3;

    // Класс подкреплений которых спавнит босс
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    TSubclassOf<AAR_EnemyBase> ReinforcementClass;

    // Вызывается при смене фазы - переопределяется в Blueprint
    UFUNCTION(BlueprintNativeEvent, Category = "Boss")
    void OnPhaseTransition(EBossPhase NewPhase);
    virtual void OnPhaseTransition_Implementation(EBossPhase NewPhase);

protected:
    virtual void BeginPlay() override;

    // Проверка смены фазы
    UFUNCTION()
    void CheckPhaseTransition(
        UAR_HealthComponent* HealthComp,
        float Health,
        float HealthDelta,
        const UDamageType* DamageType);

private:
    // Спавн подкреплений
    void SpawnReinforcements();
};