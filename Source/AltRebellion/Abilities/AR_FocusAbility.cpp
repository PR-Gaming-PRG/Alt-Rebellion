#include "Abilities/AR_FocusAbility.h"

#include "Characters/AR_CharacterBase.h"
#include "Components/AR_WeaponComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UAR_FocusAbility::UAR_FocusAbility()
{
    AbilityID = TEXT("Alisa_Focus");
    DisplayName = FText::FromString(TEXT("Фокус"));
    BuffDisplayName = DisplayName;
    BuffDescription = FText::FromString(TEXT("Увеличивает урон оружия."));
    FocusBuffDisplayName = DisplayName;
    FocusBuffDescription = BuffDescription;
    Cooldown = 0.0f;
}

void UAR_FocusAbility::InitializeAbility_Implementation(AAR_CharacterBase* OwnerCharacter)
{
    Super::InitializeAbility_Implementation(OwnerCharacter);

    if (OwnerCharacter && OwnerCharacter->WeaponComponent)
    {
        OwnerCharacter->WeaponComponent->OnWeaponHit.AddDynamic(
            this,
            &UAR_FocusAbility::HandleWeaponHit
        );
        OwnerCharacter->WeaponComponent->OnWeaponMiss.AddDynamic(
            this,
            &UAR_FocusAbility::HandleWeaponMiss
        );
    }
}

void UAR_FocusAbility::HandleWeaponHit(AActor* TargetActor, float Damage, bool bKilled)
{
    if (!CachedOwnerCharacter || !CachedOwnerCharacter->GetWorld())
    {
        return;
    }

    HitStreak++;

    if (HitStreak < RequiredHits)
    {
        return;
    }

    if (!bIsActive)
    {
        PreviousDamageMultiplier = CachedOwnerCharacter->DamageMultiplier;
        CachedOwnerCharacter->DamageMultiplier *= DamageMultiplier * GetUpgradeDamageMultiplier();
        bIsActive = true;
    }

    CachedOwnerCharacter->GetWorld()->GetTimerManager().ClearTimer(DeactivateTimerHandle);
    CachedOwnerCharacter->GetWorld()->GetTimerManager().SetTimer(
        DeactivateTimerHandle,
        this,
        &UAR_FocusAbility::Deactivate,
        FocusDuration,
        false
    );

    ApplyFocusBuffToHUD();

    UE_LOG(LogTemp, Warning, TEXT("Focus triggered"));
}

void UAR_FocusAbility::HandleWeaponMiss(FVector TargetLocation)
{
    HitStreak = 0;
}

void UAR_FocusAbility::Deactivate_Implementation()
{
    if (!CachedOwnerCharacter || !bIsActive)
    {
        return;
    }

    CachedOwnerCharacter->DamageMultiplier = PreviousDamageMultiplier;
    HitStreak = 0;
    bIsActive = false;

    if (CachedOwnerCharacter->AbilityComponent)
    {
        CachedOwnerCharacter->AbilityComponent->RemoveBuff(FocusBuffID);
    }

    UE_LOG(LogTemp, Warning, TEXT("Focus ended"));
}

void UAR_FocusAbility::ApplyFocusBuffToHUD()
{
    if (!CachedOwnerCharacter || !CachedOwnerCharacter->AbilityComponent)
    {
        return;
    }

    const float TotalDamageMultiplier = DamageMultiplier * GetUpgradeDamageMultiplier();
    const int32 DamageBonusPercent = FMath::RoundToInt((TotalDamageMultiplier - 1.0f) * 100.0f);

    FAR_ActiveBuffInfo BuffInfo;
    BuffInfo.BuffID = FocusBuffID;
    BuffInfo.DisplayName = FocusBuffDisplayName.IsEmpty() ? DisplayName : FocusBuffDisplayName;
    BuffInfo.Description = FocusBuffDescription.IsEmpty()
        ? FText::Format(
            FText::FromString(TEXT("+{0}% к урону оружия.")),
            FText::AsNumber(DamageBonusPercent)
        )
        : FocusBuffDescription;
    BuffInfo.Icon = BuffIcon ? BuffIcon : AbilityIcon;
    BuffInfo.Duration = FocusDuration;

    CachedOwnerCharacter->AbilityComponent->ApplyBuff(BuffInfo);
}
