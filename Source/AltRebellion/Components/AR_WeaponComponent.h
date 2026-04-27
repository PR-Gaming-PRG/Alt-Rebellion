#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AR_WeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnAmmoChanged,
    int32, CurrentAmmo,
    int32, MaxAmmo
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnWeaponHit,
    AActor*, TargetActor,
    float, Damage,
    bool, bKilled
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnWeaponMiss,
    FVector, TargetLocation
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnTargetKilled,
    AActor*, KilledActor
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALTREBELLION_API UAR_WeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAR_WeaponComponent();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetNextShotDamageMultiplier(float Multiplier);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void AddEmpoweredShots(int32 ShotCount, float Multiplier);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetInfiniteAmmo(bool bNewInfiniteAmmo);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool HasInfiniteAmmo() const { return bInfiniteAmmo; }

    // Урон одного выстрела
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage = 25.0f;

    // Скорострельность (выстрелов в секунду)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float FireRate = 5.0f;

    // Дальность (для hitscan)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Range = 2000.0f;

    // Угол разброса
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float SpreadAngle = 2.0f;

    // Патроны в обойме
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 AmmoPerClip = 30;

    // Текущие патроны
    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    int32 CurrentAmmo;

    // Время перезарядки
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float ReloadTime = 1.5f;

    // Идёт ли перезарядка
    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    bool bIsReloading = false;

    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FOnAmmoChanged OnAmmoChanged;

    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FOnWeaponHit OnWeaponHit;

    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FOnWeaponMiss OnWeaponMiss;

    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FOnTargetKilled OnTargetKilled;

    // Выстрел в направлении цели
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Fire(FVector TargetLocation);

    // Начать перезарядку
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Reload();

    // Можно ли стрелять прямо сейчас
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool CanFire() const;

protected:
    virtual void BeginPlay() override;

private:
    // Таймер между выстрелами
    float FireCooldown = 0.0f;

    float LastFireTime = -999.0f;

    // Таймер для автоматической перезарядки
    FTimerHandle ReloadTimerHandle;

    float NextShotDamageMultiplier = 1.0f;

    int32 EmpoweredShotsRemaining = 0;

    float EmpoweredShotDamageMultiplier = 1.0f;

    bool bInfiniteAmmo = false;

    void FinishReload();
};
