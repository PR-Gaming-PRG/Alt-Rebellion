#include "Abilities/AR_BurstAbility.h"

#include "Characters/AR_CharacterBase.h"
#include "Components/AR_WeaponComponent.h"

UAR_BurstAbility::UAR_BurstAbility()
{
    AbilityID = TEXT("Alisa_Burst");
    DisplayName = FText::FromString(TEXT("Очередь"));
    Cooldown = 8.0f;
}

void UAR_BurstAbility::Activate_Implementation(AAR_CharacterBase* OwnerCharacter)
{
    Super::Activate_Implementation(OwnerCharacter);

    if (!OwnerCharacter || !OwnerCharacter->WeaponComponent)
    {
        return;
    }

    OwnerCharacter->WeaponComponent->AddEmpoweredShots(EmpoweredShotCount, DamageMultiplier);

    UE_LOG(LogTemp, Warning, TEXT("Burst activated"));
}
