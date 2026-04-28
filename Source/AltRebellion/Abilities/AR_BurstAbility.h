#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_BurstAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_BurstAbility : public UAR_AbilityBase
{
    GENERATED_BODY()

public:
    UAR_BurstAbility();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Burst")
    int32 EmpoweredShotCount = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Burst")
    float DamageMultiplier = 1.5f;

    virtual void Activate_Implementation(AAR_CharacterBase* OwnerCharacter) override;

private:
    UFUNCTION()
    void HandleWeaponHit(AActor* TargetActor, float Damage, bool bKilled);

    int32 RemainingEmpoweredShots = 0;
};
