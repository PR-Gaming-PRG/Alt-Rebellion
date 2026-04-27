#include "Abilities/AR_ShockwaveAbility.h"

#include "Characters/AR_CharacterBase.h"

UAR_ShockwaveAbility::UAR_ShockwaveAbility()
{
    AbilityID = TEXT("Anya_Shockwave");
    DisplayName = FText::FromString(TEXT("Ударная волна"));
    Cooldown = 8.0f;
}

void UAR_ShockwaveAbility::Activate_Implementation(AAR_CharacterBase* OwnerCharacter)
{
    Super::Activate_Implementation(OwnerCharacter);

    if (!OwnerCharacter)
    {
        return;
    }

    ApplyAreaDamage(
        OwnerCharacter,
        OwnerCharacter->GetActorLocation(),
        ShockwaveRadius,
        ShockwaveDamage,
        KnockbackStrength
    );

    UE_LOG(LogTemp, Warning, TEXT("Shockwave activated"));
}
