#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_AdrenalineAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_AdrenalineAbility : public UAR_AbilityBase
{
    GENERATED_BODY()

public:
    UAR_AdrenalineAbility();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adrenaline")
    float SpeedMultiplier = 1.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Adrenaline")
    float SpeedDuration = 4.0f;

    virtual void InitializeAbility_Implementation(AAR_CharacterBase* OwnerCharacter) override;
    virtual void Deactivate_Implementation() override;

private:
    UFUNCTION()
    void HandleTargetKilled(AActor* KilledActor);

    float PreviousMoveSpeed = 600.0f;
    bool bIsActive = false;
    FTimerHandle DeactivateTimerHandle;
};
