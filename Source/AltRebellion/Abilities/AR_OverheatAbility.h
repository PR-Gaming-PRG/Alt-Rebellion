#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_OverheatAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_OverheatAbility : public UAR_AbilityBase
{
    GENERATED_BODY()

public:
    UAR_OverheatAbility();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overheat")
    float DamageMultiplier = 1.6f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overheat")
    float OverheatDuration = 6.0f;

    virtual void Activate_Implementation(AAR_CharacterBase* OwnerCharacter) override;
    virtual void Deactivate_Implementation() override;

private:
    float PreviousDamageMultiplier = 1.0f;
    bool bIsActive = false;
    FTimerHandle DeactivateTimerHandle;
};
