#include "Abilities/AR_PointBlankShotAbility.h"

#include "Characters/AR_CharacterBase.h"
#include "Components/AR_WeaponComponent.h"

UAR_PointBlankShotAbility::UAR_PointBlankShotAbility()
{
  AbilityID = TEXT("Lena_PointBlankShot");
  DisplayName = FText::FromString(TEXT("Выстрел в упор"));
  Cooldown = 6.0f;
  Duration = 0.0f;
  DamageMultiplier = 2.0f;
}

void UAR_PointBlankShotAbility::Activate_Implementation(AAR_CharacterBase* OwnerCharacter)
{
  Super::Activate_Implementation(OwnerCharacter);

  if (!OwnerCharacter || !OwnerCharacter->WeaponComponent)
  {
    return;
  }

  OwnerCharacter->WeaponComponent->SetNextShotDamageMultiplier(DamageMultiplier);

  UE_LOG(
      LogTemp,
      Warning,
      TEXT("Lena Point Blank Shot activated. Next shot multiplier: %.2f"),
      DamageMultiplier
  );
}