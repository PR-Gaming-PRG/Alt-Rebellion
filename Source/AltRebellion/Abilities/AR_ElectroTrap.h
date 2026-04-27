#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AR_ElectroTrap.generated.h"

class AAR_CharacterBase;
class USphereComponent;

UCLASS(BlueprintType, Blueprintable)
class ALTREBELLION_API AAR_ElectroTrap : public AActor
{
    GENERATED_BODY()

public:
    AAR_ElectroTrap();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
    TObjectPtr<USphereComponent> CollisionSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
    float TrapRadius = 350.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
    float TrapDuration = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
    float SlowMultiplier = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
    float TickDamage = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
    float TickInterval = 1.0f;

    UFUNCTION(BlueprintCallable, Category = "Trap")
    void InitializeTrap(
        AAR_CharacterBase* NewSourceCharacter,
        float NewRadius,
        float NewDuration,
        float NewSlowMultiplier,
        float NewTickDamage,
        float NewTickInterval
    );

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void ApplyTrapPulse();
    void ExpireTrap();
    void RestoreCharacter(ACharacter* Character);

    TWeakObjectPtr<AAR_CharacterBase> SourceCharacter;
    TMap<TWeakObjectPtr<ACharacter>, float> OriginalMoveSpeeds;
    FTimerHandle PulseTimerHandle;
    FTimerHandle ExpireTimerHandle;
};
