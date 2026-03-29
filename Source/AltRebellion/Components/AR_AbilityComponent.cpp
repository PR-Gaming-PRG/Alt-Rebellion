#include "Components/AR_AbilityComponent.h"
#include "Engine/World.h"

UAR_AbilityComponent::UAR_AbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UAR_AbilityBase::IsReady() const
{
    float CurrentTime = 0.0f;
    // Проверяем прошло ли достаточно времени с последней активации
    return (GetWorld() && GetWorld()->GetTimeSeconds() - LastActivationTime >= Cooldown);
}

void UAR_AbilityBase::StartCooldown()
{
    if (GetWorld())
    {
        LastActivationTime = GetWorld()->GetTimeSeconds();
    }
}

void UAR_AbilityComponent::ActivateAbility1()
{
    if (Ability1 && Ability1->IsReady())
    {
        Ability1->Activate(GetOwner());
        Ability1->StartCooldown();
    }
}

void UAR_AbilityComponent::ActivateAbility2()
{
    if (Ability2 && Ability2->IsReady())
    {
        Ability2->Activate(GetOwner());
        Ability2->StartCooldown();
    }
}

void UAR_AbilityComponent::ActivateUltimate()
{
    if (Ultimate && Ultimate->IsReady())
    {
        Ultimate->Activate(GetOwner());
        Ultimate->StartCooldown();
    }
}

float UAR_AbilityComponent::GetAbility1CooldownPercent() const
{
    if (!Ability1) return 1.0f;
    if (!GetWorld()) return 1.0f;
    float Elapsed = GetWorld()->GetTimeSeconds() - Ability1->GetLastActivationTime();
    return FMath::Clamp(Elapsed / Ability1->Cooldown, 0.0f, 1.0f);
}

float UAR_AbilityComponent::GetAbility2CooldownPercent() const
{
    if (!Ability2) return 1.0f;
    if (!GetWorld()) return 1.0f;
    float Elapsed = GetWorld()->GetTimeSeconds() - Ability2->GetLastActivationTime();
    return FMath::Clamp(Elapsed / Ability2->Cooldown, 0.0f, 1.0f);
}

float UAR_AbilityComponent::GetUltimateCooldownPercent() const
{
    if (!Ultimate) return 1.0f;
    if (!GetWorld()) return 1.0f;
    float Elapsed = GetWorld()->GetTimeSeconds() - Ultimate->GetLastActivationTime();
    return FMath::Clamp(Elapsed / Ultimate->Cooldown, 0.0f, 1.0f);
}