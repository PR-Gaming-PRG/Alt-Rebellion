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

    OwnerCharacter->WeaponComponent->AddEmpoweredShots(
        EmpoweredShotCount,
        DamageMultiplier * GetUpgradeDamageMultiplier()
    );

    RemainingEmpoweredShots = EmpoweredShotCount;
    OwnerCharacter->WeaponComponent->OnWeaponHit.RemoveDynamic(
        this,
        &UAR_BurstAbility::HandleWeaponHit
    );
    OwnerCharacter->WeaponComponent->OnWeaponHit.AddDynamic(
        this,
        &UAR_BurstAbility::HandleWeaponHit
    );

    const float TotalDamageMultiplier = DamageMultiplier * GetUpgradeDamageMultiplier();
    const int32 DamageBonusPercent = FMath::RoundToInt((TotalDamageMultiplier - 1.0f) * 100.0f);

    ShowBuffOnHUD(
        AbilityID,
        0.0f,
        FText::Format(
            FText::FromString(TEXT("+{0}% к урону следующих выстрелов.")),
            FText::AsNumber(DamageBonusPercent)
        ),
        RemainingEmpoweredShots,
        EmpoweredShotCount
    );

    UE_LOG(LogTemp, Warning, TEXT("Burst activated"));
}

void UAR_BurstAbility::HandleWeaponHit(AActor* TargetActor, float Damage, bool bKilled)
{
    if (!CachedOwnerCharacter || !CachedOwnerCharacter->WeaponComponent)
    {
        return;
    }

    RemainingEmpoweredShots = FMath::Max(0, RemainingEmpoweredShots - 1);

    if (RemainingEmpoweredShots <= 0)
    {
        CachedOwnerCharacter->WeaponComponent->OnWeaponHit.RemoveDynamic(
            this,
            &UAR_BurstAbility::HandleWeaponHit
        );
        HideBuffFromHUD(AbilityID);
        return;
    }

    const float TotalDamageMultiplier = DamageMultiplier * GetUpgradeDamageMultiplier();
    const int32 DamageBonusPercent = FMath::RoundToInt((TotalDamageMultiplier - 1.0f) * 100.0f);

    ShowBuffOnHUD(
        AbilityID,
        0.0f,
        FText::Format(
            FText::FromString(TEXT("+{0}% к урону следующих выстрелов.")),
            FText::AsNumber(DamageBonusPercent)
        ),
        RemainingEmpoweredShots,
        EmpoweredShotCount
    );
}
