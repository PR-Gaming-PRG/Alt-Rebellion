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

  PreviousDamageMultiplier = OwnerCharacter->DamageMultiplier;
  OwnerCharacter->DamageMultiplier *= DamageMultiplierBonus * GetUpgradeDamageMultiplier();

  if (OwnerCharacter->GetCharacterMovement())
  {
    PreviousMoveSpeed = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
    OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed *= SpeedMultiplierBonus;
  }

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
      TEXT("Buff activated. Damage x%.2f, Speed x%.2f"),
      DamageMultiplierBonus * GetUpgradeDamageMultiplier(),
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

  UE_LOG(
      LogTemp,
      Warning,
      TEXT("Buff ended. Damage and speed restored")
  );
}
