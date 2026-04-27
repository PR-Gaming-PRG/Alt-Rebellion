#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AR_AbilityComponent.generated.h"

class AAR_CharacterBase;

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ALTREBELLION_API UAR_AbilityBase : public UObject
{
    GENERATED_BODY()

public:
    // ID способности, совпадает с ID в DT_AbilityUpgrades
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FName AbilityID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Cooldown = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Duration = 0.0f;

    UFUNCTION(BlueprintCallable, Category = "Ability")
    bool IsReady() const;

    UFUNCTION(BlueprintCallable, Category = "Ability")
    float GetCooldownPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Ability")
    float GetLastActivationTime() const { return LastActivationTime; }

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
    void Activate(AAR_CharacterBase* OwnerCharacter);

    virtual void Activate_Implementation(AAR_CharacterBase* OwnerCharacter);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ability")
    void Deactivate();

    virtual void Deactivate_Implementation();

    void StartCooldown();

protected:
    UPROPERTY()
    TObjectPtr<AAR_CharacterBase> CachedOwnerCharacter;

    float LastActivationTime = -999.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALTREBELLION_API UAR_AbilityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAR_AbilityComponent();

protected:
    virtual void BeginPlay() override;

public:
    // Классы способностей, назначаются в BP_Anya / BP_Lena / BP_Alisa
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    TSubclassOf<UAR_AbilityBase> Ability1Class;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    TSubclassOf<UAR_AbilityBase> Ability2Class;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    TSubclassOf<UAR_AbilityBase> UltimateClass;

    // Созданные runtime-объекты способностей
    UPROPERTY(BlueprintReadOnly, Category = "Abilities")
    TObjectPtr<UAR_AbilityBase> Ability1;

    UPROPERTY(BlueprintReadOnly, Category = "Abilities")
    TObjectPtr<UAR_AbilityBase> Ability2;

    UPROPERTY(BlueprintReadOnly, Category = "Abilities")
    TObjectPtr<UAR_AbilityBase> Ultimate;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void ActivateAbility1();

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void ActivateAbility2();

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void ActivateUltimate();

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    float GetAbility1CooldownPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    float GetAbility2CooldownPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    float GetUltimateCooldownPercent() const;

private:
    UAR_AbilityBase* CreateAbilityInstance(TSubclassOf<UAR_AbilityBase> AbilityClass);
};
