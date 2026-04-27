#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_AlisaOverdriveAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_AlisaOverdriveAbility : public UAR_AbilityBase
{
    GENERATED_BODY()

public:
    UAR_AlisaOverdriveAbility();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overdrive")
    float OverdriveDuration = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overdrive")
    float FireRateMultiplier = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overdrive")
    float SpeedMultiplier = 1.2f;

    virtual void Activate_Implementation(AAR_CharacterBase* OwnerCharacter) override;
    virtual void Deactivate_Implementation() override;

private:
    float PreviousFireRate = 5.0f;
    float PreviousMoveSpeed = 600.0f;
    bool bPreviousInfiniteAmmo = false;
    bool bIsActive = false;
    FTimerHandle DeactivateTimerHandle;
};
