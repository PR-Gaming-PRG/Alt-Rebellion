#include "Abilities/AR_RocketBarrageAbility.h"

#include "Characters/AR_CharacterBase.h"
#include "Engine/World.h"
#include "TimerManager.h"

UAR_RocketBarrageAbility::UAR_RocketBarrageAbility()
{
    AbilityID = TEXT("Anya_RocketBarrage");
    DisplayName = FText::FromString(TEXT("Ракетный залп"));
    Cooldown = 8.0f;
}

void UAR_RocketBarrageAbility::ActivateAtLocation_Implementation(
    AAR_CharacterBase* OwnerCharacter,
    FVector TargetLocation
)
{
    Super::ActivateAtLocation_Implementation(OwnerCharacter, TargetLocation);

    if (!OwnerCharacter || !OwnerCharacter->GetWorld())
    {
        return;
    }

    CachedOwnerCharacter = OwnerCharacter;
    CachedTargetLocation = TargetLocation;
    RocketsRemaining = FMath::Max(1, RocketCount);

    OwnerCharacter->GetWorld()->GetTimerManager().ClearTimer(RocketTimerHandle);
    FireNextRocket();
}

void UAR_RocketBarrageAbility::FireNextRocket()
{
    if (!CachedOwnerCharacter || !CachedOwnerCharacter->GetWorld() || RocketsRemaining <= 0)
    {
        return;
    }

    FVector ImpactLocation = CachedTargetLocation;

    if (BarrageSpreadRadius > 0.0f)
    {
        const FVector2D RandomOffset = FMath::RandPointInCircle(BarrageSpreadRadius);
        ImpactLocation.X += RandomOffset.X;
        ImpactLocation.Y += RandomOffset.Y;
    }

    ApplyAreaDamage(
        CachedOwnerCharacter,
        ImpactLocation,
        ExplosionRadius,
        ExplosionDamage
    );

    RocketsRemaining--;

    if (RocketsRemaining > 0)
    {
        CachedOwnerCharacter->GetWorld()->GetTimerManager().SetTimer(
            RocketTimerHandle,
            this,
            &UAR_RocketBarrageAbility::FireNextRocket,
            FMath::Max(0.01f, RocketDelay),
            false
        );
    }

    UE_LOG(LogTemp, Warning, TEXT("Rocket Barrage impact"));
}
