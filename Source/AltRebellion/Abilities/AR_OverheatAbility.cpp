#include "Abilities/AR_OverheatAbility.h"

#include "Characters/AR_CharacterBase.h"
#include "Engine/World.h"
#include "TimerManager.h"

UAR_OverheatAbility::UAR_OverheatAbility()
{
    AbilityID = TEXT("Anya_Overheat");
    DisplayName = FText::FromString(TEXT("Перегрев"));
    Cooldown = 18.0f;
    Duration = 6.0f;
}

void UAR_OverheatAbility::Activate_Implementation(AAR_CharacterBase* OwnerCharacter)
{
    Super::Activate_Implementation(OwnerCharacter);

    if (!OwnerCharacter || !OwnerCharacter->GetWorld())
    {
        return;
    }

    CachedOwnerCharacter = OwnerCharacter;

    if (!bIsActive)
    {
        PreviousDamageMultiplier = OwnerCharacter->DamageMultiplier;
        OwnerCharacter->DamageMultiplier *= DamageMultiplier * GetUpgradeDamageMultiplier();
        bIsActive = true;
    }

    OwnerCharacter->GetWorld()->GetTimerManager().ClearTimer(DeactivateTimerHandle);
    OwnerCharacter->GetWorld()->GetTimerManager().SetTimer(
        DeactivateTimerHandle,
        this,
        &UAR_OverheatAbility::Deactivate,
        OverheatDuration,
        false
    );

    const float TotalDamageMultiplier = DamageMultiplier * GetUpgradeDamageMultiplier();
    const int32 DamageBonusPercent = FMath::RoundToInt((TotalDamageMultiplier - 1.0f) * 100.0f);

    ShowBuffOnHUD(
        AbilityID,
        OverheatDuration,
        FText::Format(
            FText::FromString(TEXT("+{0}% к урону оружия.")),
            FText::AsNumber(DamageBonusPercent)
        )
    );

    UE_LOG(LogTemp, Warning, TEXT("Overheat activated"));
}

void UAR_OverheatAbility::Deactivate_Implementation()
{
    if (!CachedOwnerCharacter || !bIsActive)
    {
        return;
    }

    CachedOwnerCharacter->DamageMultiplier = PreviousDamageMultiplier;
    bIsActive = false;
    HideBuffFromHUD(AbilityID);

    UE_LOG(LogTemp, Warning, TEXT("Overheat ended"));
}
