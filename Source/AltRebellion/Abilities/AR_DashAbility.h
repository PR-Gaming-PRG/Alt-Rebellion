#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_DashAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_DashAbility : public UAR_AbilityBase
{
    GENERATED_BODY()

public:
    UAR_DashAbility();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    float DashDistance = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    float DashCooldown = 4.0f;

    virtual void Activate_Implementation(AAR_CharacterBase* OwnerCharacter) override;
};