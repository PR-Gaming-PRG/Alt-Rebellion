#include "Abilities/AR_DamageBuffAbility.h"

#include "Characters/AR_CharacterBase.h"
#include "Engine/World.h"
#include "TimerManager.h"

UAR_DamageBuffAbility::UAR_DamageBuffAbility()
{
    AbilityID = TEXT("Generic_DamageBuff");
    DisplayName = FText::FromString(TEXT("Усиление урона"));
    Cooldown = 12.0f;
    Duration = 5.0f;
    BuffDuration = 5.0f;
    DamageMultiplierBonus = 1.5f;
}

void UAR_DamageBuffAbility::Activate_Implementation(AAR_CharacterBase* OwnerCharacter)
{
    Super::Activate_Implementation(OwnerCharacter);

    if (!OwnerCharacter || !OwnerCharacter->GetWorld())
    {
        return;
    }

    PreviousDamageMultiplier = OwnerCharacter->DamageMultiplier;
    OwnerCharacter->DamageMultiplier *= DamageMultiplierBonus;

    OwnerCharacter->GetWorld()->GetTimerManager().ClearTimer(DeactivateTimerHandle);

    OwnerCharacter->GetWorld()->GetTimerManager().SetTimer(
        DeactivateTimerHandle,
        this,
        &UAR_DamageBuffAbility::Deactivate,
        BuffDuration,
        false
    );

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Damage buff activated. Multiplier: %.2f"),
        OwnerCharacter->DamageMultiplier
    );
}

void UAR_DamageBuffAbility::Deactivate_Implementation()
{
    if (CachedOwnerCharacter)
    {
        CachedOwnerCharacter->DamageMultiplier = PreviousDamageMultiplier;

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Damage buff ended. Multiplier restored: %.2f"),
            CachedOwnerCharacter->DamageMultiplier
        );
    }
}