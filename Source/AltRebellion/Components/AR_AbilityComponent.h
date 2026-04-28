#pragma once

#include "CoreMinimal.h"
#include "Abilities/AR_AbilityUpgradeData.h"
#include "Components/ActorComponent.h"
#include "AR_AbilityComponent.generated.h"

class AAR_CharacterBase;
class UDataTable;
class UTexture2D;

UENUM(BlueprintType)
enum class EAR_AbilitySlot : uint8
{
Passive UMETA(DisplayName = "Passive"),
Ability1 UMETA(DisplayName = "Ability 1"),
Ability2 UMETA(DisplayName = "Ability 2"),
Ultimate UMETA(DisplayName = "Ultimate")
};

USTRUCT(BlueprintType)
struct FAR_ActiveBuffInfo
{
GENERATED_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
FName BuffID;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
FText DisplayName;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
FText Description;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
TObjectPtr<UTexture2D> Icon;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
float Duration = 0.0f;

UPROPERTY(BlueprintReadOnly, Category = "Buff")
float StartTime = 0.0f;

UPROPERTY(BlueprintReadOnly, Category = "Buff")
float RemainingTime = 0.0f;

UPROPERTY(BlueprintReadOnly, Category = "Buff")
float RemainingPercent = 0.0f;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
int32 StackCount = 0;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
int32 MaxStackCount = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnBuffInfoChanged,
    const FAR_ActiveBuffInfo&, BuffInfo
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnBuffRemoved,
    FName, BuffID
);

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ALTREBELLION_API UAR_AbilityBase : public UObject
{
GENERATED_BODY()

public:
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
FName AbilityID;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
FText DisplayName;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|UI")
TObjectPtr<UTexture2D> AbilityIcon;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Buff")
FText BuffDisplayName;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Buff")
FText BuffDescription;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Buff")
TObjectPtr<UTexture2D> BuffIcon;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
float Cooldown = 5.0f;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
float Duration = 0.0f;

UPROPERTY(BlueprintReadOnly, Category = "Ability|Upgrade")
int32 AbilityLevel = 0;

UPROPERTY(BlueprintReadOnly, Category = "Ability|Upgrade")
float UpgradeDamageMultiplier = 1.0f;

UFUNCTION(BlueprintCallable, Category = "Ability")
bool IsReady() const;

UFUNCTION(BlueprintCallable, Category = "Ability")
float GetCooldownPercent() const;

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
float GetCooldownRemaining() const;

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
float GetCooldownOverlayPercent() const;

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
bool IsOnCooldown() const;

UFUNCTION(BlueprintCallable, Category = "Ability")
float GetLastActivationTime() const { return LastActivationTime; }

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Upgrade")
int32 GetAbilityLevel() const { return AbilityLevel; }

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Upgrade")
float GetUpgradeDamageMultiplier() const { return UpgradeDamageMultiplier; }

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability|Upgrade")
float GetUpgradedDamage(float BaseDamage) const;

UFUNCTION(BlueprintCallable, Category = "Ability|Upgrade")
void ResetUpgradeState();

UFUNCTION(BlueprintCallable, Category = "Ability|Upgrade")
void ResetCooldown();

UFUNCTION(BlueprintCallable, Category = "Ability|Upgrade")
void ReduceCooldown(float Seconds);

// Вызывается один раз после создания способности.
// Для пассивок здесь будем подписываться на события.
UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
void InitializeAbility(AAR_CharacterBase* OwnerCharacter);

virtual void InitializeAbility_Implementation(AAR_CharacterBase* OwnerCharacter);

UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
void Activate(AAR_CharacterBase* OwnerCharacter);

virtual void Activate_Implementation(AAR_CharacterBase* OwnerCharacter);

UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
void ActivateAtLocation(AAR_CharacterBase* OwnerCharacter, FVector TargetLocation);

virtual void ActivateAtLocation_Implementation(AAR_CharacterBase* OwnerCharacter, FVector TargetLocation);

UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
void Deactivate();

virtual void Deactivate_Implementation();

UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability|Upgrade")
void ApplyUpgradeRow(const FAR_AbilityUpgradeRow& UpgradeRow);

virtual void ApplyUpgradeRow_Implementation(const FAR_AbilityUpgradeRow& UpgradeRow);

void StartCooldown();

protected:
UPROPERTY()
TObjectPtr<AAR_CharacterBase> CachedOwnerCharacter;

float LastActivationTime = -999.0f;

float BaseCooldownBeforeUpgrades = -1.0f;

void ShowBuffOnHUD(FName BuffID, float BuffDuration, const FText& DescriptionOverride, int32 StackCount = 0, int32 MaxStackCount = 0);
void HideBuffFromHUD(FName BuffID);

void ApplyAreaDamage(
    AAR_CharacterBase* OwnerCharacter,
    const FVector& Center,
    float Radius,
    float Damage,
    float KnockbackStrength = 0.0f
) const;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALTREBELLION_API UAR_AbilityComponent : public UActorComponent
{
GENERATED_BODY()

public:
UAR_AbilityComponent();

protected:
virtual void BeginPlay() override;
virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
// Пассивная способность
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
TSubclassOf<UAR_AbilityBase> PassiveClass;

// Активная способность 1
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
TSubclassOf<UAR_AbilityBase> Ability1Class;

// Активная способность 2
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
TSubclassOf<UAR_AbilityBase> Ability2Class;

// Ультимативная способность
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
TSubclassOf<UAR_AbilityBase> UltimateClass;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities|Upgrades")
TObjectPtr<UDataTable> AbilityUpgradeTable;

UPROPERTY(BlueprintReadOnly, Category = "Abilities")
TObjectPtr<UAR_AbilityBase> Passive;

UPROPERTY(BlueprintReadOnly, Category = "Abilities")
TObjectPtr<UAR_AbilityBase> Ability1;

UPROPERTY(BlueprintReadOnly, Category = "Abilities")
TObjectPtr<UAR_AbilityBase> Ability2;

UPROPERTY(BlueprintReadOnly, Category = "Abilities")
TObjectPtr<UAR_AbilityBase> Ultimate;

UPROPERTY(BlueprintAssignable, Category = "Abilities|Buffs")
FOnBuffInfoChanged OnBuffApplied;

UPROPERTY(BlueprintAssignable, Category = "Abilities|Buffs")
FOnBuffInfoChanged OnBuffUpdated;

UPROPERTY(BlueprintAssignable, Category = "Abilities|Buffs")
FOnBuffRemoved OnBuffRemoved;

UFUNCTION(BlueprintCallable, Category = "Abilities")
void ActivateAbility1();

UFUNCTION(BlueprintCallable, Category = "Abilities")
void ActivateAbility2();

UFUNCTION(BlueprintCallable, Category = "Abilities")
void ActivateUltimate();

UFUNCTION(BlueprintCallable, Category = "Abilities")
void ActivateAbility1AtLocation(FVector TargetLocation);

UFUNCTION(BlueprintCallable, Category = "Abilities")
void ActivateAbility2AtLocation(FVector TargetLocation);

UFUNCTION(BlueprintCallable, Category = "Abilities")
void ActivateUltimateAtLocation(FVector TargetLocation);

UFUNCTION(BlueprintCallable, Category = "Abilities")
float GetAbility1CooldownPercent() const;

UFUNCTION(BlueprintCallable, Category = "Abilities")
float GetAbility2CooldownPercent() const;

UFUNCTION(BlueprintCallable, Category = "Abilities")
float GetUltimateCooldownPercent() const;

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
UAR_AbilityBase* GetAbilityBySlot(EAR_AbilitySlot Slot) const;

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
UAR_AbilityBase* GetPassiveAbility() const { return Passive.Get(); }

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
UAR_AbilityBase* GetAbility1() const { return Ability1.Get(); }

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
UAR_AbilityBase* GetAbility2() const { return Ability2.Get(); }

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
UAR_AbilityBase* GetUltimateAbility() const { return Ultimate.Get(); }

UFUNCTION(BlueprintCallable, Category = "Abilities|Upgrades")
void ApplySavedUpgradeLevels();

UFUNCTION(BlueprintCallable, Category = "Abilities|Upgrades")
void ResetAbilityCooldowns();

UFUNCTION(BlueprintCallable, Category = "Abilities|Upgrades")
void ReduceAbilityCooldowns(float Seconds);

UFUNCTION(BlueprintCallable, Category = "Abilities|Buffs")
void ApplyBuff(const FAR_ActiveBuffInfo& BuffInfo);

UFUNCTION(BlueprintCallable, Category = "Abilities|Buffs")
void RemoveBuff(FName BuffID);

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities|Buffs")
TArray<FAR_ActiveBuffInfo> GetActiveBuffs() const;

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities|Buffs")
bool GetActiveBuff(FName BuffID, FAR_ActiveBuffInfo& OutBuffInfo) const;

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities|Buffs")
float GetBuffRemainingTime(FName BuffID) const;

UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities|Buffs")
float GetBuffRemainingPercent(FName BuffID) const;

private:
UFUNCTION()
void HandleAbilityUpgraded(FName AbilityID, int32 NewLevel);

UAR_AbilityBase* CreateAbilityInstance(TSubclassOf<UAR_AbilityBase> AbilityClass);
void ApplySavedUpgradeLevelToAbility(UAR_AbilityBase* Ability);
const FAR_AbilityUpgradeRow* FindUpgradeRow(FName AbilityID, int32 TargetLevel) const;
FAR_ActiveBuffInfo BuildBuffSnapshot(const FAR_ActiveBuffInfo& BuffInfo) const;

UPROPERTY()
TMap<FName, FAR_ActiveBuffInfo> ActiveBuffs;
};
