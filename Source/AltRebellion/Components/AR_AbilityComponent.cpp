#include "Components/AR_AbilityComponent.h"

#include "Characters/AR_CharacterBase.h"
#include "Components/AR_HealthComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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

void UAR_AbilityBase::StartCooldown()
{
  UWorld* World = GetWorld();

  if (World)
  {
    LastActivationTime = World->GetTimeSeconds();
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

    TargetHealth->ApplyDamageWithCauser(Damage, nullptr, OwnerCharacter);
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
