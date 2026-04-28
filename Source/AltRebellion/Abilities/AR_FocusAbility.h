#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_FocusAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_FocusAbility : public UAR_AbilityBase
{
    GENERATED_BODY()

public:
    UAR_FocusAbility();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Focus")
    int32 RequiredHits = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Focus")
    float FocusDuration = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Focus")
    float DamageMultiplier = 1.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Focus|Buff")
    FName FocusBuffID = TEXT("Alisa_Focus_Buff");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Focus|Buff")
    FText FocusBuffDisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Focus|Buff")
    FText FocusBuffDescription;

    virtual void InitializeAbility_Implementation(AAR_CharacterBase* OwnerCharacter) override;
    virtual void Deactivate_Implementation() override;

private:
    UFUNCTION()
    void HandleWeaponHit(AActor* TargetActor, float Damage, bool bKilled);

    UFUNCTION()
    void HandleWeaponMiss(FVector TargetLocation);

    int32 HitStreak = 0;
    float PreviousDamageMultiplier = 1.0f;
    bool bIsActive = false;
    FTimerHandle DeactivateTimerHandle;

    void ApplyFocusBuffToHUD();
};
