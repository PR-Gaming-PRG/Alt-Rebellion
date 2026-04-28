#include "Abilities/AR_AdrenalineAbility.h"

#include "Characters/AR_CharacterBase.h"
#include "Components/AR_WeaponComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UAR_AdrenalineAbility::UAR_AdrenalineAbility()
{
    AbilityID = TEXT("Lena_Adrenaline");
    DisplayName = FText::FromString(TEXT("Адреналин"));
    Cooldown = 0.0f;
}

void UAR_AdrenalineAbility::InitializeAbility_Implementation(AAR_CharacterBase* OwnerCharacter)
{
    Super::InitializeAbility_Implementation(OwnerCharacter);

    if (OwnerCharacter && OwnerCharacter->WeaponComponent)
    {
        OwnerCharacter->WeaponComponent->OnTargetKilled.AddDynamic(
            this,
            &UAR_AdrenalineAbility::HandleTargetKilled
        );
    }
}

void UAR_AdrenalineAbility::HandleTargetKilled(AActor* KilledActor)
{
    if (!CachedOwnerCharacter || !CachedOwnerCharacter->GetWorld() || !CachedOwnerCharacter->GetCharacterMovement())
    {
        return;
    }

    if (!bIsActive)
    {
        PreviousMoveSpeed = CachedOwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
        CachedOwnerCharacter->GetCharacterMovement()->MaxWalkSpeed *= SpeedMultiplier;
        bIsActive = true;
    }

    CachedOwnerCharacter->GetWorld()->GetTimerManager().ClearTimer(DeactivateTimerHandle);
    CachedOwnerCharacter->GetWorld()->GetTimerManager().SetTimer(
        DeactivateTimerHandle,
        this,
        &UAR_AdrenalineAbility::Deactivate,
        SpeedDuration,
        false
    );

    const int32 SpeedBonusPercent = FMath::RoundToInt((SpeedMultiplier - 1.0f) * 100.0f);
    ShowBuffOnHUD(
        AbilityID,
        SpeedDuration,
        FText::Format(
            FText::FromString(TEXT("+{0}% к скорости после убийства.")),
            FText::AsNumber(SpeedBonusPercent)
        )
    );

    UE_LOG(LogTemp, Warning, TEXT("Adrenaline triggered"));
}

void UAR_AdrenalineAbility::Deactivate_Implementation()
{
    if (!CachedOwnerCharacter || !CachedOwnerCharacter->GetCharacterMovement() || !bIsActive)
    {
        return;
    }

    CachedOwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = PreviousMoveSpeed;
    bIsActive = false;
    HideBuffFromHUD(AbilityID);

    UE_LOG(LogTemp, Warning, TEXT("Adrenaline ended"));
}
