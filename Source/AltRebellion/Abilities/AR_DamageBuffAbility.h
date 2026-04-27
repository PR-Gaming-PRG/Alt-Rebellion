#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_DamageBuffAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_DamageBuffAbility : public UAR_AbilityBase
{
GENERATED_BODY()

public:
UAR_DamageBuffAbility();

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
float DamageMultiplierBonus = 1.5f;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
float SpeedMultiplierBonus = 1.0f;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
float BuffDuration = 5.0f;

virtual void Activate_Implementation(AAR_CharacterBase* OwnerCharacter) override;
virtual void Deactivate_Implementation() override;

private:
float PreviousDamageMultiplier = 1.0f;
float PreviousMoveSpeed = 600.0f;

FTimerHandle DeactivateTimerHandle;
};