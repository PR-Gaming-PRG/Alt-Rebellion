#include "Abilities/AR_ElectroTrap.h"

#include "Characters/AR_CharacterBase.h"
#include "Components/AR_HealthComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

AAR_ElectroTrap::AAR_ElectroTrap()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;

    CollisionSphere->SetSphereRadius(TrapRadius);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAR_ElectroTrap::InitializeTrap(
    AAR_CharacterBase* NewSourceCharacter,
    float NewRadius,
    float NewDuration,
    float NewSlowMultiplier,
    float NewTickDamage,
    float NewTickInterval
)
{
    SourceCharacter = NewSourceCharacter;
    TrapRadius = NewRadius;
    TrapDuration = NewDuration;
    SlowMultiplier = NewSlowMultiplier;
    TickDamage = NewTickDamage;
    TickInterval = NewTickInterval;

    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(TrapRadius);
    }
}

void AAR_ElectroTrap::BeginPlay()
{
    Super::BeginPlay();

    if (CollisionSphere)
    {
        CollisionSphere->SetSphereRadius(TrapRadius);
    }

    ApplyTrapPulse();

    GetWorldTimerManager().SetTimer(
        PulseTimerHandle,
        this,
        &AAR_ElectroTrap::ApplyTrapPulse,
        FMath::Max(0.05f, TickInterval),
        true
    );

    GetWorldTimerManager().SetTimer(
        ExpireTimerHandle,
        this,
        &AAR_ElectroTrap::ExpireTrap,
        FMath::Max(0.05f, TrapDuration),
        false
    );
}

void AAR_ElectroTrap::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    for (TPair<TWeakObjectPtr<ACharacter>, float>& Pair : OriginalMoveSpeeds)
    {
        RestoreCharacter(Pair.Key.Get());
    }

    OriginalMoveSpeeds.Empty();

    Super::EndPlay(EndPlayReason);
}

void AAR_ElectroTrap::ApplyTrapPulse()
{
    if (!CollisionSphere)
    {
        return;
    }

    TArray<AActor*> OverlappingActors;
    CollisionSphere->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

    TSet<TWeakObjectPtr<ACharacter>> CurrentCharacters;

    for (AActor* Actor : OverlappingActors)
    {
        ACharacter* Character = Cast<ACharacter>(Actor);
        if (!Character || Character == SourceCharacter.Get())
        {
            continue;
        }

        UAR_HealthComponent* HealthComponent = Character->FindComponentByClass<UAR_HealthComponent>();
        if (!HealthComponent || HealthComponent->IsDead())
        {
            continue;
        }

        CurrentCharacters.Add(Character);

        if (!OriginalMoveSpeeds.Contains(Character))
        {
            if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
            {
                OriginalMoveSpeeds.Add(Character, Movement->MaxWalkSpeed);
                Movement->MaxWalkSpeed *= FMath::Clamp(SlowMultiplier, 0.05f, 1.0f);
            }
        }

        if (TickDamage > 0.0f)
        {
            HealthComponent->ApplyDamageWithCauser(TickDamage, nullptr, SourceCharacter.Get());
        }
    }

    TArray<TWeakObjectPtr<ACharacter>> CharactersToRestore;
    for (const TPair<TWeakObjectPtr<ACharacter>, float>& Pair : OriginalMoveSpeeds)
    {
        if (!Pair.Key.IsValid() || !CurrentCharacters.Contains(Pair.Key))
        {
            CharactersToRestore.Add(Pair.Key);
        }
    }

    for (const TWeakObjectPtr<ACharacter>& CharacterPtr : CharactersToRestore)
    {
        RestoreCharacter(CharacterPtr.Get());
        OriginalMoveSpeeds.Remove(CharacterPtr);
    }
}

void AAR_ElectroTrap::ExpireTrap()
{
    Destroy();
}

void AAR_ElectroTrap::RestoreCharacter(ACharacter* Character)
{
    if (!Character)
    {
        return;
    }

    const float* OriginalSpeed = OriginalMoveSpeeds.Find(TWeakObjectPtr<ACharacter>(Character));
    if (!OriginalSpeed)
    {
        return;
    }

    if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
    {
        Movement->MaxWalkSpeed = *OriginalSpeed;
    }
}
