#include "Components/AR_AbilityComponent.h"

#include "Characters/AR_CharacterBase.h"
#include "Core/AR_GameInstance.h"
#include "Components/AR_HealthComponent.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UAR_AbilityComponent::UAR_AbilityComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void UAR_AbilityComponent::BeginPlay()
{
  Super::BeginPlay();

  AAR_CharacterBase* OwnerCharacter = Cast<AAR_CharacterBase>(GetOwner());

  Passive = CreateAbilityInstance(PassiveClass);
  Ability1 = CreateAbilityInstance(Ability1Class);
  Ability2 = CreateAbilityInstance(Ability2Class);
  Ultimate = CreateAbilityInstance(UltimateClass);

  if (Passive && OwnerCharacter)
  {
    Passive->InitializeAbility(OwnerCharacter);
  }

  if (Ability1 && OwnerCharacter)
  {
    Ability1->InitializeAbility(OwnerCharacter);
  }

  if (Ability2 && OwnerCharacter)
  {
    Ability2->InitializeAbility(OwnerCharacter);
  }

  if (Ultimate && OwnerCharacter)
  {
    Ultimate->InitializeAbility(OwnerCharacter);
  }

  if (UAR_GameInstance* GI = Cast<UAR_GameInstance>(UGameplayStatics::GetGameInstance(this)))
  {
    GI->OnAbilityUpgraded.AddDynamic(
        this,
        &UAR_AbilityComponent::HandleAbilityUpgraded
    );
  }

  ApplySavedUpgradeLevels();
}

void UAR_AbilityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  if (UAR_GameInstance* GI = Cast<UAR_GameInstance>(UGameplayStatics::GetGameInstance(this)))
  {
    GI->OnAbilityUpgraded.RemoveDynamic(
        this,
        &UAR_AbilityComponent::HandleAbilityUpgraded
    );
  }

  Super::EndPlay(EndPlayReason);
}

UAR_AbilityBase* UAR_AbilityComponent::CreateAbilityInstance(TSubclassOf<UAR_AbilityBase> AbilityClass)
{
  if (!AbilityClass)
  {
    return nullptr;
  }

  UAR_AbilityBase* NewAbility = NewObject<UAR_AbilityBase>(this, AbilityClass);

  if (!NewAbility)
  {
    UE_LOG(LogTemp, Error, TEXT("Failed to create ability instance"));
    return nullptr;
  }

  return NewAbility;
}

bool UAR_AbilityBase::IsReady() const
{
  UWorld* World = GetWorld();

  if (!World)
  {
    return false;
  }

  return World->GetTimeSeconds() - LastActivationTime >= Cooldown;
}

float UAR_AbilityBase::GetCooldownPercent() const
{
  UWorld* World = GetWorld();

  if (!World)
  {
    return 1.0f;
  }

  if (Cooldown <= 0.0f)
  {
    return 1.0f;
  }

  const float Elapsed = World->GetTimeSeconds() - LastActivationTime;
  return FMath::Clamp(Elapsed / Cooldown, 0.0f, 1.0f);
}

float UAR_AbilityBase::GetCooldownRemaining() const
{
  UWorld* World = GetWorld();

  if (!World || Cooldown <= 0.0f)
  {
    return 0.0f;
  }

  const float Elapsed = World->GetTimeSeconds() - LastActivationTime;
  return FMath::Max(0.0f, Cooldown - Elapsed);
}

float UAR_AbilityBase::GetCooldownOverlayPercent() const
{
  if (Cooldown <= 0.0f)
  {
    return 0.0f;
  }

  return 1.0f - GetCooldownPercent();
}

bool UAR_AbilityBase::IsOnCooldown() const
{
  return GetCooldownRemaining() > 0.0f;
}

float UAR_AbilityBase::GetUpgradedDamage(float BaseDamage) const
{
  return BaseDamage * UpgradeDamageMultiplier;
}

void UAR_AbilityBase::ResetUpgradeState()
{
  if (BaseCooldownBeforeUpgrades < 0.0f)
  {
    BaseCooldownBeforeUpgrades = Cooldown;
  }

  Cooldown = BaseCooldownBeforeUpgrades;
  AbilityLevel = 0;
  UpgradeDamageMultiplier = 1.0f;
}

void UAR_AbilityBase::ResetCooldown()
{
  LastActivationTime = -999.0f;
}

void UAR_AbilityBase::ReduceCooldown(float Seconds)
{
  if (Seconds <= 0.0f)
  {
    ResetCooldown();
    return;
  }

  LastActivationTime -= Seconds;
}

void UAR_AbilityBase::StartCooldown()
{
  UWorld* World = GetWorld();

  if (World)
  {
    LastActivationTime = World->GetTimeSeconds();
  }
}

void UAR_AbilityBase::ShowBuffOnHUD(
    FName BuffID,
    float BuffDuration,
    const FText& DescriptionOverride,
    int32 StackCount,
    int32 MaxStackCount
)
{
  if (!CachedOwnerCharacter || !CachedOwnerCharacter->AbilityComponent || BuffID.IsNone())
  {
    return;
  }

  FAR_ActiveBuffInfo BuffInfo;
  BuffInfo.BuffID = BuffID;
  BuffInfo.DisplayName = BuffDisplayName.IsEmpty() ? DisplayName : BuffDisplayName;
  BuffInfo.Description = DescriptionOverride.IsEmpty() ? BuffDescription : DescriptionOverride;
  BuffInfo.Icon = BuffIcon ? BuffIcon : AbilityIcon;
  BuffInfo.Duration = BuffDuration;
  BuffInfo.StackCount = StackCount;
  BuffInfo.MaxStackCount = MaxStackCount;

  CachedOwnerCharacter->AbilityComponent->ApplyBuff(BuffInfo);
}

void UAR_AbilityBase::HideBuffFromHUD(FName BuffID)
{
  if (CachedOwnerCharacter && CachedOwnerCharacter->AbilityComponent)
  {
    CachedOwnerCharacter->AbilityComponent->RemoveBuff(BuffID);
  }
}

void UAR_AbilityBase::InitializeAbility_Implementation(AAR_CharacterBase* OwnerCharacter)
{
  CachedOwnerCharacter = OwnerCharacter;
}

void UAR_AbilityBase::Activate_Implementation(AAR_CharacterBase* OwnerCharacter)
{
  CachedOwnerCharacter = OwnerCharacter;
}

void UAR_AbilityBase::ActivateAtLocation_Implementation(
    AAR_CharacterBase* OwnerCharacter,
    FVector TargetLocation
)
{
  Activate(OwnerCharacter);
}

void UAR_AbilityBase::Deactivate_Implementation()
{
}

void UAR_AbilityBase::ApplyUpgradeRow_Implementation(const FAR_AbilityUpgradeRow& UpgradeRow)
{
  if (BaseCooldownBeforeUpgrades < 0.0f)
  {
    BaseCooldownBeforeUpgrades = Cooldown;
  }

  AbilityLevel = FMath::Max(AbilityLevel, UpgradeRow.Level);

  if (UpgradeRow.CooldownMultiplier > 0.0f)
  {
    Cooldown *= UpgradeRow.CooldownMultiplier;
  }

  if (!FMath::IsNearlyZero(UpgradeRow.DamageMultiplierBonus))
  {
    UpgradeDamageMultiplier *= FMath::Max(0.0f, 1.0f + UpgradeRow.DamageMultiplierBonus);
  }
}

void UAR_AbilityBase::ApplyAreaDamage(
    AAR_CharacterBase* OwnerCharacter,
    const FVector& Center,
    float Radius,
    float Damage,
    float KnockbackStrength
) const
{
  if (!OwnerCharacter || !OwnerCharacter->GetWorld() || Radius <= 0.0f || Damage <= 0.0f)
  {
    return;
  }

  TArray<FOverlapResult> Overlaps;
  FCollisionObjectQueryParams ObjectParams;
  ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

  FCollisionQueryParams QueryParams;
  QueryParams.AddIgnoredActor(OwnerCharacter);

  const bool bHasOverlaps = OwnerCharacter->GetWorld()->OverlapMultiByObjectType(
      Overlaps,
      Center,
      FQuat::Identity,
      ObjectParams,
      FCollisionShape::MakeSphere(Radius),
      QueryParams
  );

  if (!bHasOverlaps)
  {
    return;
  }

  TSet<AActor*> DamagedActors;

  for (const FOverlapResult& Overlap : Overlaps)
  {
    AActor* TargetActor = Overlap.GetActor();
    if (!TargetActor || TargetActor == OwnerCharacter || DamagedActors.Contains(TargetActor))
    {
      continue;
    }

    UAR_HealthComponent* TargetHealth = TargetActor->FindComponentByClass<UAR_HealthComponent>();
    if (!TargetHealth || TargetHealth->IsDead())
    {
      continue;
    }

    TargetHealth->ApplyDamageWithCauser(GetUpgradedDamage(Damage), nullptr, OwnerCharacter);
    DamagedActors.Add(TargetActor);

    if (KnockbackStrength > 0.0f)
    {
      if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
      {
        FVector KnockbackDirection = TargetActor->GetActorLocation() - Center;
        KnockbackDirection.Z = 0.0f;

        if (!KnockbackDirection.Normalize())
        {
          KnockbackDirection = OwnerCharacter->GetActorForwardVector();
        }

        TargetCharacter->LaunchCharacter(
            KnockbackDirection * KnockbackStrength,
            true,
            false
        );
      }
    }
  }
}

const FAR_AbilityUpgradeRow* UAR_AbilityComponent::FindUpgradeRow(
    FName AbilityID,
    int32 TargetLevel
) const
{
  if (AbilityID.IsNone() || TargetLevel <= 0 || !AbilityUpgradeTable)
  {
    return nullptr;
  }

  TArray<FAR_AbilityUpgradeRow*> Rows;
  AbilityUpgradeTable->GetAllRows<FAR_AbilityUpgradeRow>(
      TEXT("UAR_AbilityComponent::FindUpgradeRow"),
      Rows
  );

  for (const FAR_AbilityUpgradeRow* Row : Rows)
  {
    if (Row && Row->AbilityID == AbilityID && Row->Level == TargetLevel)
    {
      return Row;
    }
  }

  return nullptr;
}

void UAR_AbilityComponent::ApplySavedUpgradeLevels()
{
  ApplySavedUpgradeLevelToAbility(Passive.Get());
  ApplySavedUpgradeLevelToAbility(Ability1.Get());
  ApplySavedUpgradeLevelToAbility(Ability2.Get());
  ApplySavedUpgradeLevelToAbility(Ultimate.Get());
}

void UAR_AbilityComponent::ApplySavedUpgradeLevelToAbility(UAR_AbilityBase* Ability)
{
  if (!Ability || Ability->AbilityID.IsNone())
  {
    return;
  }

  const UAR_GameInstance* GI = Cast<UAR_GameInstance>(
      UGameplayStatics::GetGameInstance(this)
  );

  if (!GI)
  {
    return;
  }

  Ability->ResetUpgradeState();

  const int32 SavedLevel = GI->GetAbilityLevel(Ability->AbilityID);

  if (SavedLevel <= 1)
  {
    return;
  }

  if (!AbilityUpgradeTable)
  {
    Ability->AbilityLevel = SavedLevel;
    return;
  }

  for (int32 TargetLevel = 2; TargetLevel <= SavedLevel; ++TargetLevel)
  {
    const FAR_AbilityUpgradeRow* UpgradeRow = FindUpgradeRow(
        Ability->AbilityID,
        TargetLevel
    );

    if (UpgradeRow)
    {
      Ability->ApplyUpgradeRow(*UpgradeRow);
    }
  }

  Ability->AbilityLevel = SavedLevel;
}

void UAR_AbilityComponent::HandleAbilityUpgraded(FName AbilityID, int32 NewLevel)
{
  (void)NewLevel;

  if (AbilityID.IsNone())
  {
    return;
  }

  TArray<UAR_AbilityBase*> Abilities = {
      Passive.Get(),
      Ability1.Get(),
      Ability2.Get(),
      Ultimate.Get()
  };

  for (UAR_AbilityBase* Ability : Abilities)
  {
    if (Ability && Ability->AbilityID == AbilityID)
    {
      ApplySavedUpgradeLevelToAbility(Ability);
      return;
    }
  }
}

void UAR_AbilityComponent::ResetAbilityCooldowns()
{
  if (Ability1)
  {
    Ability1->ResetCooldown();
  }

  if (Ability2)
  {
    Ability2->ResetCooldown();
  }

  if (Ultimate)
  {
    Ultimate->ResetCooldown();
  }
}

void UAR_AbilityComponent::ReduceAbilityCooldowns(float Seconds)
{
  if (Seconds <= 0.0f)
  {
    ResetAbilityCooldowns();
    return;
  }

  if (Ability1)
  {
    Ability1->ReduceCooldown(Seconds);
  }

  if (Ability2)
  {
    Ability2->ReduceCooldown(Seconds);
  }

  if (Ultimate)
  {
    Ultimate->ReduceCooldown(Seconds);
  }
}

void UAR_AbilityComponent::ApplyBuff(const FAR_ActiveBuffInfo& BuffInfo)
{
  if (BuffInfo.BuffID.IsNone())
  {
    return;
  }

  FAR_ActiveBuffInfo StoredBuff = BuffInfo;

  if (const UWorld* World = GetWorld())
  {
    StoredBuff.StartTime = World->GetTimeSeconds();
  }

  const bool bWasAlreadyActive = ActiveBuffs.Contains(StoredBuff.BuffID);
  ActiveBuffs.FindOrAdd(StoredBuff.BuffID) = StoredBuff;

  const FAR_ActiveBuffInfo Snapshot = BuildBuffSnapshot(StoredBuff);

  if (bWasAlreadyActive)
  {
    OnBuffUpdated.Broadcast(Snapshot);
  }
  else
  {
    OnBuffApplied.Broadcast(Snapshot);
  }
}

void UAR_AbilityComponent::RemoveBuff(FName BuffID)
{
  if (BuffID.IsNone())
  {
    return;
  }

  if (ActiveBuffs.Remove(BuffID) > 0)
  {
    OnBuffRemoved.Broadcast(BuffID);
  }
}

TArray<FAR_ActiveBuffInfo> UAR_AbilityComponent::GetActiveBuffs() const
{
  TArray<FAR_ActiveBuffInfo> Buffs;
  Buffs.Reserve(ActiveBuffs.Num());

  for (const TPair<FName, FAR_ActiveBuffInfo>& Pair : ActiveBuffs)
  {
    Buffs.Add(BuildBuffSnapshot(Pair.Value));
  }

  return Buffs;
}

bool UAR_AbilityComponent::GetActiveBuff(FName BuffID, FAR_ActiveBuffInfo& OutBuffInfo) const
{
  const FAR_ActiveBuffInfo* BuffInfo = ActiveBuffs.Find(BuffID);

  if (!BuffInfo)
  {
    return false;
  }

  OutBuffInfo = BuildBuffSnapshot(*BuffInfo);
  return true;
}

float UAR_AbilityComponent::GetBuffRemainingTime(FName BuffID) const
{
  FAR_ActiveBuffInfo BuffInfo;
  return GetActiveBuff(BuffID, BuffInfo) ? BuffInfo.RemainingTime : 0.0f;
}

float UAR_AbilityComponent::GetBuffRemainingPercent(FName BuffID) const
{
  FAR_ActiveBuffInfo BuffInfo;
  return GetActiveBuff(BuffID, BuffInfo) ? BuffInfo.RemainingPercent : 0.0f;
}

FAR_ActiveBuffInfo UAR_AbilityComponent::BuildBuffSnapshot(const FAR_ActiveBuffInfo& BuffInfo) const
{
  FAR_ActiveBuffInfo Snapshot = BuffInfo;

  if (Snapshot.Duration <= 0.0f)
  {
    Snapshot.RemainingTime = 0.0f;
    Snapshot.RemainingPercent = Snapshot.MaxStackCount > 0
        ? FMath::Clamp(static_cast<float>(Snapshot.StackCount) / static_cast<float>(Snapshot.MaxStackCount), 0.0f, 1.0f)
        : 1.0f;
    return Snapshot;
  }

  const UWorld* World = GetWorld();
  const float CurrentTime = World ? World->GetTimeSeconds() : Snapshot.StartTime;
  const float ElapsedTime = FMath::Max(0.0f, CurrentTime - Snapshot.StartTime);

  Snapshot.RemainingTime = FMath::Max(0.0f, Snapshot.Duration - ElapsedTime);
  Snapshot.RemainingPercent = FMath::Clamp(Snapshot.RemainingTime / Snapshot.Duration, 0.0f, 1.0f);

  return Snapshot;
}

void UAR_AbilityComponent::ActivateAbility1()
{
  ActivateAbility1AtLocation(GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector);
}

void UAR_AbilityComponent::ActivateAbility2()
{
  ActivateAbility2AtLocation(GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector);
}

void UAR_AbilityComponent::ActivateUltimate()
{
  ActivateUltimateAtLocation(GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector);
}

void UAR_AbilityComponent::ActivateAbility1AtLocation(FVector TargetLocation)
{
  AAR_CharacterBase* OwnerCharacter = Cast<AAR_CharacterBase>(GetOwner());

  if (Ability1 && Ability1->IsReady() && OwnerCharacter)
  {
    Ability1->ActivateAtLocation(OwnerCharacter, TargetLocation);
    Ability1->StartCooldown();
  }
}

void UAR_AbilityComponent::ActivateAbility2AtLocation(FVector TargetLocation)
{
  AAR_CharacterBase* OwnerCharacter = Cast<AAR_CharacterBase>(GetOwner());

  if (Ability2 && Ability2->IsReady() && OwnerCharacter)
  {
    Ability2->ActivateAtLocation(OwnerCharacter, TargetLocation);
    Ability2->StartCooldown();
  }
}

void UAR_AbilityComponent::ActivateUltimateAtLocation(FVector TargetLocation)
{
  AAR_CharacterBase* OwnerCharacter = Cast<AAR_CharacterBase>(GetOwner());

  if (Ultimate && Ultimate->IsReady() && OwnerCharacter)
  {
    Ultimate->ActivateAtLocation(OwnerCharacter, TargetLocation);
    Ultimate->StartCooldown();
  }
}

float UAR_AbilityComponent::GetAbility1CooldownPercent() const
{
  return Ability1 ? Ability1->GetCooldownPercent() : 1.0f;
}

float UAR_AbilityComponent::GetAbility2CooldownPercent() const
{
  return Ability2 ? Ability2->GetCooldownPercent() : 1.0f;
}

float UAR_AbilityComponent::GetUltimateCooldownPercent() const
{
  return Ultimate ? Ultimate->GetCooldownPercent() : 1.0f;
}

UAR_AbilityBase* UAR_AbilityComponent::GetAbilityBySlot(EAR_AbilitySlot Slot) const
{
  switch (Slot)
  {
  case EAR_AbilitySlot::Passive:
    return Passive.Get();
  case EAR_AbilitySlot::Ability1:
    return Ability1.Get();
  case EAR_AbilitySlot::Ability2:
    return Ability2.Get();
  case EAR_AbilitySlot::Ultimate:
    return Ultimate.Get();
  default:
    return nullptr;
  }
}
