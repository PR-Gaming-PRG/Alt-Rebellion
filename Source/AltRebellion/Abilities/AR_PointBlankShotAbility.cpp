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

  const float FinalDamageMultiplier = DamageMultiplier * GetUpgradeDamageMultiplier();
  OwnerCharacter->WeaponComponent->SetNextShotDamageMultiplier(FinalDamageMultiplier);
  OwnerCharacter->WeaponComponent->OnWeaponHit.RemoveDynamic(
      this,
      &UAR_PointBlankShotAbility::HandleWeaponHit
  );
  OwnerCharacter->WeaponComponent->OnWeaponHit.AddDynamic(
      this,
      &UAR_PointBlankShotAbility::HandleWeaponHit
  );

  const int32 DamageBonusPercent = FMath::RoundToInt((FinalDamageMultiplier - 1.0f) * 100.0f);

  ShowBuffOnHUD(
      AbilityID,
      0.0f,
      FText::Format(
          FText::FromString(TEXT("+{0}% к урону следующего попадания.")),
          FText::AsNumber(DamageBonusPercent)
      ),
      1,
      1
  );

  UE_LOG(
      LogTemp,
      Warning,
      TEXT("Lena Point Blank Shot activated. Next shot multiplier: %.2f"),
      FinalDamageMultiplier
  );
}

void UAR_PointBlankShotAbility::HandleWeaponHit(AActor* TargetActor, float Damage, bool bKilled)
{
  if (CachedOwnerCharacter && CachedOwnerCharacter->WeaponComponent)
  {
    CachedOwnerCharacter->WeaponComponent->OnWeaponHit.RemoveDynamic(
        this,
        &UAR_PointBlankShotAbility::HandleWeaponHit
    );
  }

  HideBuffFromHUD(AbilityID);
}
