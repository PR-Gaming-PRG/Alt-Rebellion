#include "Abilities/AR_ExplosiveArmorAbility.h"

#include "Characters/AR_CharacterBase.h"
#include "Components/AR_HealthComponent.h"
#include "Engine/World.h"

UAR_ExplosiveArmorAbility::UAR_ExplosiveArmorAbility()
{
    AbilityID = TEXT("Anya_ExplosiveArmor");
    DisplayName = FText::FromString(TEXT("Взрывная броня"));
    Cooldown = 0.0f;
}

void UAR_ExplosiveArmorAbility::InitializeAbility_Implementation(AAR_CharacterBase* OwnerCharacter)
{
    Super::InitializeAbility_Implementation(OwnerCharacter);

    if (OwnerCharacter && OwnerCharacter->HealthComponent)
    {
        OwnerCharacter->HealthComponent->OnDamageTaken.AddDynamic(
            this,
            &UAR_ExplosiveArmorAbility::HandleOwnerDamaged
        );
    }
}

void UAR_ExplosiveArmorAbility::HandleOwnerDamaged(
    UAR_HealthComponent* HealthComp,
    float Damage,
    AActor* DamageCauser,
    const UDamageType* DamageType
)
{
    if (!CachedOwnerCharacter || !CachedOwnerCharacter->GetWorld())
    {
        return;
    }

    const float CurrentTime = CachedOwnerCharacter->GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastExplosionTime < ExplosionCooldown)
    {
        return;
    }

    LastExplosionTime = CurrentTime;

    ApplyAreaDamage(
        CachedOwnerCharacter,
        CachedOwnerCharacter->GetActorLocation(),
        ExplosionRadius,
        ExplosionDamage
    );

    UE_LOG(LogTemp, Warning, TEXT("Explosive Armor triggered"));
}
