#include "Components/AR_AbilityComponent.h"
#include "Characters/AR_CharacterBase.h"
#include "Engine/World.h"

UAR_AbilityComponent::UAR_AbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAR_AbilityComponent::BeginPlay()
{
    Super::BeginPlay();

    Ability1 = CreateAbilityInstance(Ability1Class);
    Ability2 = CreateAbilityInstance(Ability2Class);
    Ultimate = CreateAbilityInstance(UltimateClass);
}

UAR_AbilityBase* UAR_AbilityComponent::CreateAbilityInstance(TSubclassOf<UAR_AbilityBase> AbilityClass)
{
    if (!AbilityClass)
    {
        return nullptr;
    }

    UAR_AbilityBase* NewAbility = NewObject<UAR_AbilityBase>(this, AbilityClass);

    if (!NewAbility)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create ability instance"));
        return nullptr;
    }

    return NewAbility;
}

bool UAR_AbilityBase::IsReady() const
{
    UWorld* World = GetWorld();

    if (!World)
    {
        return false;
    }

    return World->GetTimeSeconds() - LastActivationTime >= Cooldown;
}

float UAR_AbilityBase::GetCooldownPercent() const
{
    UWorld* World = GetWorld();

    if (!World)
    {
        return 1.0f;
    }

    const float Elapsed = World->GetTimeSeconds() - LastActivationTime;

    if (Cooldown <= 0.0f)
    {
        return 1.0f;
    }

    return FMath::Clamp(Elapsed / Cooldown, 0.0f, 1.0f);
}

void UAR_AbilityBase::StartCooldown()
{
    UWorld* World = GetWorld();

    if (World)
    {
        LastActivationTime = World->GetTimeSeconds();
    }
}

void UAR_AbilityBase::Activate_Implementation(AAR_CharacterBase* OwnerCharacter)
{
    CachedOwnerCharacter = OwnerCharacter;
}

void UAR_AbilityBase::Deactivate_Implementation()
{
}

void UAR_AbilityComponent::ActivateAbility1()
{
    AAR_CharacterBase* OwnerCharacter = Cast<AAR_CharacterBase>(GetOwner());

    if (Ability1 && Ability1->IsReady() && OwnerCharacter)
    {
        Ability1->Activate(OwnerCharacter);
        Ability1->StartCooldown();
    }
}

void UAR_AbilityComponent::ActivateAbility2()
{
    AAR_CharacterBase* OwnerCharacter = Cast<AAR_CharacterBase>(GetOwner());

    if (Ability2 && Ability2->IsReady() && OwnerCharacter)
    {
        Ability2->Activate(OwnerCharacter);
        Ability2->StartCooldown();
    }
}

void UAR_AbilityComponent::ActivateUltimate()
{
    AAR_CharacterBase* OwnerCharacter = Cast<AAR_CharacterBase>(GetOwner());

    if (Ultimate && Ultimate->IsReady() && OwnerCharacter)
    {
        Ultimate->Activate(OwnerCharacter);
        Ultimate->StartCooldown();
    }
}

float UAR_AbilityComponent::GetAbility1CooldownPercent() const
{
    return Ability1 ? Ability1->GetCooldownPercent() : 1.0f;
}

float UAR_AbilityComponent::GetAbility2CooldownPercent() const
{
    return Ability2 ? Ability2->GetCooldownPercent() : 1.0f;
}

float UAR_AbilityComponent::GetUltimateCooldownPercent() const
{
    return Ultimate ? Ultimate->GetCooldownPercent() : 1.0f;
}
