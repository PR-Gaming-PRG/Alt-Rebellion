#pragma once

#include "CoreMinimal.h"
#include "Components/AR_AbilityComponent.h"
#include "AR_ExplosiveArmorAbility.generated.h"

class UAR_HealthComponent;

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API UAR_ExplosiveArmorAbility : public UAR_AbilityBase
{
    GENERATED_BODY()

public:
    UAR_ExplosiveArmorAbility();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosive Armor")
    float ExplosionRadius = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosive Armor")
    float ExplosionDamage = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosive Armor")
    float ExplosionCooldown = 4.0f;

    virtual void InitializeAbility_Implementation(AAR_CharacterBase* OwnerCharacter) override;

private:
    UFUNCTION()
    void HandleOwnerDamaged(
        UAR_HealthComponent* HealthComp,
        float Damage,
        AActor* DamageCauser,
        const UDamageType* DamageType
    );

    float LastExplosionTime = -999.0f;
};
