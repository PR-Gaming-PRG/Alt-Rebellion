#include "Components/AR_WeaponComponent.h"
#include "Components/AR_HealthComponent.h"
#include "Characters/AR_CharacterBase.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"

UAR_WeaponComponent::UAR_WeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAR_WeaponComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentAmmo = AmmoPerClip;
}

bool UAR_WeaponComponent::CanFire() const
{
    return !bIsReloading && (bInfiniteAmmo || CurrentAmmo > 0);
}

void UAR_WeaponComponent::SetNextShotDamageMultiplier(float Multiplier)
{
  NextShotDamageMultiplier = FMath::Max(1.0f, Multiplier);

  UE_LOG(
      LogTemp,
      Warning,
      TEXT("Next shot damage multiplier set: %.2f"),
      NextShotDamageMultiplier
  );
}

void UAR_WeaponComponent::AddEmpoweredShots(int32 ShotCount, float Multiplier)
{
    if (ShotCount <= 0)
    {
        return;
    }

    EmpoweredShotsRemaining += ShotCount;
    EmpoweredShotDamageMultiplier = FMath::Max(1.0f, Multiplier);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Empowered shots added: %d, multiplier: %.2f"),
        EmpoweredShotsRemaining,
        EmpoweredShotDamageMultiplier
    );
}

void UAR_WeaponComponent::SetInfiniteAmmo(bool bNewInfiniteAmmo)
{
    bInfiniteAmmo = bNewInfiniteAmmo;

    if (bInfiniteAmmo)
    {
        bIsReloading = false;
        GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
    }
}

void UAR_WeaponComponent::Fire(FVector TargetLocation)
{
    if (!CanFire()) return;

    float CurrentTime = GetWorld()->GetTimeSeconds();
    float FireInterval = 1.0f / FireRate;
    if (CurrentTime - LastFireTime < FireInterval) return;
    LastFireTime = CurrentTime;

    if (!bInfiniteAmmo)
    {
        CurrentAmmo--;
        OnAmmoChanged.Broadcast(CurrentAmmo, AmmoPerClip);
    }

    // Hitscan - луч от владельца к цели
    AActor* Owner = GetOwner();
    if (!Owner) return;

    FVector StartLocation = Owner->GetActorLocation();
    FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();

    // Применяем разброс
    FVector SpreadDirection = FMath::VRandCone(Direction, FMath::DegreesToRadians(SpreadAngle));
    FVector EndLocation = StartLocation + SpreadDirection * Range;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Pawn,
        Params
    );

    // Debug линия (убрать позже)
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.5f);

    if (bHit && HitResult.GetActor())
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *HitResult.GetActor()->GetName());

        UAR_HealthComponent* TargetHealth = 
            HitResult.GetActor()->FindComponentByClass<UAR_HealthComponent>();

        if (TargetHealth)
        {
            const bool bWasDead = TargetHealth->IsDead();
            float FinalDamage = Damage;

            if (AAR_CharacterBase* OwnerCharacter = Cast<AAR_CharacterBase>(Owner))
            {
              FinalDamage *= OwnerCharacter->DamageMultiplier;
            }

            FinalDamage *= NextShotDamageMultiplier;
            NextShotDamageMultiplier = 1.0f;

            if (EmpoweredShotsRemaining > 0)
            {
                FinalDamage *= EmpoweredShotDamageMultiplier;
                EmpoweredShotsRemaining--;
            }

            TargetHealth->ApplyDamageWithCauser(FinalDamage, nullptr, Owner);
            const bool bKilled = !bWasDead && TargetHealth->IsDead();

            OnWeaponHit.Broadcast(HitResult.GetActor(), FinalDamage, bKilled);

            if (bKilled)
            {
                OnTargetKilled.Broadcast(HitResult.GetActor());
            }

            UE_LOG(
                LogTemp,
                Warning,
                TEXT("Damage applied: %.1f"),
                FinalDamage
            );
        }
        else
        {
            OnWeaponMiss.Broadcast(TargetLocation);
        }
    }
    else
    {
        OnWeaponMiss.Broadcast(TargetLocation);
    }

    // Автоперезарядка если патроны кончились
    if (!bInfiniteAmmo && CurrentAmmo <= 0)
    {
        Reload();
    }
}

void UAR_WeaponComponent::Reload()
{
    if (bIsReloading) return;

    bIsReloading = true;
    UE_LOG(LogTemp, Warning, TEXT("Reloading..."));

    GetWorld()->GetTimerManager().SetTimer(
        ReloadTimerHandle,
        this,
        &UAR_WeaponComponent::FinishReload,
        ReloadTime,
        false
    );
}

void UAR_WeaponComponent::FinishReload()
{
    bIsReloading = false;
    CurrentAmmo = AmmoPerClip;
    OnAmmoChanged.Broadcast(CurrentAmmo, AmmoPerClip);
    UE_LOG(LogTemp, Warning, TEXT("Reload Complete!"));
}
