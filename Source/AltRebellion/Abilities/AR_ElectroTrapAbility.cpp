#include "Abilities/AR_ElectroTrapAbility.h"

#include "Abilities/AR_ElectroTrap.h"
#include "Characters/AR_CharacterBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UAR_ElectroTrapAbility::UAR_ElectroTrapAbility()
{
    AbilityID = TEXT("Alisa_ElectroTrap");
    DisplayName = FText::FromString(TEXT("Электроловушка"));
    Cooldown = 10.0f;
}

void UAR_ElectroTrapAbility::ActivateAtLocation_Implementation(
    AAR_CharacterBase* OwnerCharacter,
    FVector TargetLocation
)
{
    Super::ActivateAtLocation_Implementation(OwnerCharacter, TargetLocation);

    if (!OwnerCharacter || !OwnerCharacter->GetWorld())
    {
        return;
    }

    TSubclassOf<AAR_ElectroTrap> ClassToSpawn = TrapClass;
    if (!ClassToSpawn)
    {
        ClassToSpawn = AAR_ElectroTrap::StaticClass();
    }

    const FTransform SpawnTransform(FRotator::ZeroRotator, TargetLocation);
    AAR_ElectroTrap* Trap = OwnerCharacter->GetWorld()->SpawnActorDeferred<AAR_ElectroTrap>(
        ClassToSpawn,
        SpawnTransform,
        OwnerCharacter,
        OwnerCharacter,
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
    );

    if (!Trap)
    {
        return;
    }

    Trap->InitializeTrap(
        OwnerCharacter,
        TrapRadius,
        TrapDuration,
        SlowMultiplier,
        GetUpgradedDamage(TickDamage),
        TickInterval
    );

    UGameplayStatics::FinishSpawningActor(Trap, SpawnTransform);

    UE_LOG(LogTemp, Warning, TEXT("Electro Trap placed"));
}
