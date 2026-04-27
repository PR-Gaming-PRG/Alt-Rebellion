#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_RocketBarrageAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_RocketBarrageAbility : public UAR_AbilityBase
{
    GENERATED_BODY()

public:
    UAR_RocketBarrageAbility();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rocket Barrage")
    int32 RocketCount = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rocket Barrage")
    float RocketDelay = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rocket Barrage")
    float ExplosionRadius = 350.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rocket Barrage")
    float ExplosionDamage = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rocket Barrage")
    float BarrageSpreadRadius = 150.0f;

    virtual void ActivateAtLocation_Implementation(
        AAR_CharacterBase* OwnerCharacter,
        FVector TargetLocation
    ) override;

private:
    void FireNextRocket();

    FVector CachedTargetLocation = FVector::ZeroVector;
    int32 RocketsRemaining = 0;
    FTimerHandle RocketTimerHandle;
};
