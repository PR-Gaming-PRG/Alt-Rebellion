#include "Abilities/AR_FocusAbility.h"

#include "Characters/AR_CharacterBase.h"
#include "Components/AR_WeaponComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UAR_FocusAbility::UAR_FocusAbility()
{
    AbilityID = TEXT("Alisa_Focus");
    DisplayName = FText::FromString(TEXT("Фокус"));
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

    UE_LOG(LogTemp, Warning, TEXT("Focus ended"));
}
