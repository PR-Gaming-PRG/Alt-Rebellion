#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_ElectroTrapAbility.generated.h"

class AAR_ElectroTrap;

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_ElectroTrapAbility : public UAR_AbilityBase
{
    GENERATED_BODY()

public:
    UAR_ElectroTrapAbility();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electro Trap")
    TSubclassOf<AAR_ElectroTrap> TrapClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electro Trap")
    float TrapRadius = 350.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electro Trap")
    float TrapDuration = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electro Trap")
    float SlowMultiplier = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electro Trap")
    float TickDamage = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electro Trap")
    float TickInterval = 1.0f;

    virtual void ActivateAtLocation_Implementation(
        AAR_CharacterBase* OwnerCharacter,
        FVector TargetLocation
    ) override;
};
