#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_PointBlankShotAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_PointBlankShotAbility : public UAR_AbilityBase
{
GENERATED_BODY()

public:
UAR_PointBlankShotAbility();

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point Blank")
float DamageMultiplier = 2.0f;

virtual void Activate_Implementation(AAR_CharacterBase* OwnerCharacter) override;
};