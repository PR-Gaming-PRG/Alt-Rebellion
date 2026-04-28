#include "Abilities/AR_DamageBuffAbility.h"

#include "Characters/AR_CharacterBase.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UAR_DamageBuffAbility::UAR_DamageBuffAbility()
{
  AbilityID = TEXT("Generic_DamageBuff");
  DisplayName = FText::FromString(TEXT("Усиление"));
  Cooldown = 12.0f;
  Duration = 5.0f;
  BuffDuration = 5.0f;
  DamageMultiplierBonus = 1.5f;
  SpeedMultiplierBonus = 1.0f;
}

void UAR_DamageBuffAbility::Activate_Implementation(AAR_CharacterBase* OwnerCharacter)
{
  Super::Activate_Implementation(OwnerCharacter);

  if (!OwnerCharacter || !OwnerCharacter->GetWorld())
  {
    return;
  }

  const float TotalDamageMultiplier = DamageMultiplierBonus * GetUpgradeDamageMultiplier();

  if (!bIsActive)
  {
    PreviousDamageMultiplier = OwnerCharacter->DamageMultiplier;
    OwnerCharacter->DamageMultiplier *= TotalDamageMultiplier;

    if (OwnerCharacter->GetCharacterMovement())
    {
      PreviousMoveSpeed = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
      OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed *= SpeedMultiplierBonus;
    }

    bIsActive = true;
  }

  OwnerCharacter->GetWorld()->GetTimerManager().ClearTimer(DeactivateTimerHandle);

  OwnerCharacter->GetWorld()->GetTimerManager().SetTimer(
      DeactivateTimerHandle,
      this,
      &UAR_DamageBuffAbility::Deactivate,
      BuffDuration,
      false
  );

  const int32 DamageBonusPercent = FMath::RoundToInt((TotalDamageMultiplier - 1.0f) * 100.0f);
  const int32 SpeedBonusPercent = FMath::RoundToInt((SpeedMultiplierBonus - 1.0f) * 100.0f);

  ShowBuffOnHUD(
      AbilityID,
      BuffDuration,
      FText::Format(
          FText::FromString(TEXT("+{0}% к урону, +{1}% к скорости.")),
          FText::AsNumber(DamageBonusPercent),
          FText::AsNumber(SpeedBonusPercent)
      )
  );

  UE_LOG(
      LogTemp,
      Warning,
      TEXT("Buff activated. Damage x%.2f, Speed x%.2f"),
      TotalDamageMultiplier,
      SpeedMultiplierBonus
  );
}

void UAR_DamageBuffAbility::Deactivate_Implementation()
{
  if (!CachedOwnerCharacter)
  {
    return;
  }

  CachedOwnerCharacter->DamageMultiplier = PreviousDamageMultiplier;

  if (CachedOwnerCharacter->GetCharacterMovement())
  {
    CachedOwnerCharacter->GetCharacterMovement()->MaxWalkSpeed = PreviousMoveSpeed;
  }

  bIsActive = false;
  HideBuffFromHUD(AbilityID);

  UE_LOG(
      LogTemp,
      Warning,
      TEXT("Buff ended. Damage and speed restored")
  );
}
