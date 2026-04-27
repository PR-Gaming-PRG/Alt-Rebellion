#include "Abilities/AR_DashAbility.h"
#include "Characters/AR_CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UAR_DashAbility::UAR_DashAbility()
{
    AbilityID = TEXT("Lena_Dash");
    DisplayName = FText::FromString(TEXT("Рывок"));
    Cooldown = 4.0f;
    DashCooldown = 4.0f;
    DashDistance = 600.0f;
}

void UAR_DashAbility::Activate_Implementation(AAR_CharacterBase* OwnerCharacter)
{
    Super::Activate_Implementation(OwnerCharacter);

    if (!OwnerCharacter)
    {
        return;
    }

    FVector DashDirection = OwnerCharacter->GetActorForwardVector();

    if (OwnerCharacter->GetVelocity().SizeSquared() > 10.0f)
    {
        DashDirection = OwnerCharacter->GetVelocity().GetSafeNormal();
    }

    const FVector StartLocation = OwnerCharacter->GetActorLocation();
    const FVector EndLocation = StartLocation + DashDirection * DashDistance;

    FHitResult HitResult;
    OwnerCharacter->SetActorLocation(EndLocation, true, &HitResult);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Dash ability activated. Distance: %.1f"),
        DashDistance
    );
}