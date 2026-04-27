#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_ShockwaveAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_ShockwaveAbility : public UAR_AbilityBase
{
    GENERATED_BODY()

public:
    UAR_ShockwaveAbility();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    float ShockwaveRadius = 450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    float ShockwaveDamage = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    float KnockbackStrength = 900.0f;

    virtual void Activate_Implementation(AAR_CharacterBase* OwnerCharacter) override;
};
