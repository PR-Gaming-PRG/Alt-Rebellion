#include "Components/AR_WeaponComponent.h"
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
    return !bIsReloading && CurrentAmmo > 0;
}

void UAR_WeaponComponent::Fire(FVector TargetLocation)
{
    if (!CanFire()) return;

    CurrentAmmo--;
    OnAmmoChanged.Broadcast(CurrentAmmo, AmmoPerClip);

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
        // TODO: применить урон через HealthComponent
    }

    // Автоперезарядка если патроны кончились
    if (CurrentAmmo <= 0)
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