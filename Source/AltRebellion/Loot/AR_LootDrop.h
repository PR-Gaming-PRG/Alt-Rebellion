#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AR_LootDrop.generated.h"

UENUM(BlueprintType)
enum class ELootType : uint8
{
    HealthPack      UMETA(DisplayName = "Health Pack"),
    AmmoBox         UMETA(DisplayName = "Ammo Box"),
    Token           UMETA(DisplayName = "Token"),
    AbilityBoost    UMETA(DisplayName = "Ability Boost")
};

UCLASS()
class ALTREBELLION_API AAR_LootDrop : public AActor
{
    GENERATED_BODY()

public:
    AAR_LootDrop();

    // Тип лута
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    ELootType LootType = ELootType::Token;

    // Количество (для токенов и патронов)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    int32 Amount = 1;

    // Значение (для хилки - сколько HP восстанавливает)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    float Value = 25.0f;

    // Меш лута
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    // Сфера подбора
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class USphereComponent> PickupSphere;

    // Подобрать лут
    UFUNCTION(BlueprintCallable, Category = "Loot")
    void PickUp(AActor* Collector);

    // Событие подбора - переопределяется в Blueprint для VFX/звуков
    UFUNCTION(BlueprintNativeEvent, Category = "Loot")
    void OnPickedUp(AActor* Collector);
    virtual void OnPickedUp_Implementation(AActor* Collector);

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnSphereOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    bool bIsPickedUp = false;
};