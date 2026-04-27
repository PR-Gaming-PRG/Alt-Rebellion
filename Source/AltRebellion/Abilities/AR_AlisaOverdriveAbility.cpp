#include "Abilities/AR_AlisaOverdriveAbility.h"

#include "Characters/AR_CharacterBase.h"
#include "Components/AR_WeaponComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UAR_AlisaOverdriveAbility::UAR_AlisaOverdriveAbility()
{
    AbilityID = TEXT("Alisa_Overdrive");
    DisplayName = FText::FromString(TEXT("Овердрайв"));
    Cooldown = 20.0f;
    Duration = 6.0f;
}

void UAR_AlisaOverdriveAbility::Activate_Implementation(AAR_CharacterBase* OwnerCharacter)
{
    Super::Activate_Implementation(OwnerCharacter);

    if (!OwnerCharacter || !OwnerCharacter->GetWorld() || !OwnerCharacter->WeaponComponent)
    {
        return;
    }

    CachedOwnerCharacter = OwnerCharacter;

    if (!bIsActive)
    {
        PreviousFireRate = OwnerCharacter->WeaponComponent->FireRate;
        bPreviousInfiniteAmmo = OwnerCharacter->WeaponComponent->HasInfiniteAmmo();
        OwnerCharacter->WeaponComponent->FireRate *= FireRateMultiplier;
        OwnerCharacter->WeaponComponent->SetInfiniteAmmo(true);

        if (OwnerCharacter->GetCharacterMovement())
        {
            PreviousMoveSpeed = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
            OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed *= SpeedMultiplier;
        }

        bIsActive = true;
    }

    OwnerCharacter->GetWorld()->GetTimerManager().ClearTimer(DeactivateTimerHandle);
    OwnerCharacter->GetWorld()->GetTimerManager().SetTimer(
        DeactivateTimerHandle,
        this,
        &UAR_AlisaOverdriveAbility::Deactivate,
        OverdriveDuration,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("Alisa Overdrive activated"));
}

void UAR_AlisaOverdriveAbility::Deactivate_Implementation()
{
    if (!CachedOwnerCharacter || !CachedOwnerCharacter->WeaponComponent || !bIsActive)
    {
        return;
    }

    CachedOwnerCharacter->WeaponComponent->FireRate = PreviousFireRate;
    CachedOwnerCharacter->WeaponComponent->SetInfiniteAmmo(bPreviousInfiniteAmmo);

    if (CachedOwnerCharacter->GetCharacterMovement())
    {
        CachedOwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = PreviousMoveSpeed;
    }

    bIsActive = false;

    UE_LOG(LogTemp, Warning, TEXT("Alisa Overdrive ended"));
}
