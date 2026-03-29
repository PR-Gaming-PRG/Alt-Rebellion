#include "Components/AR_HealthComponent.h"

UAR_HealthComponent::UAR_HealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAR_HealthComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

void UAR_HealthComponent::ApplyDamage(float DamageAmount, const UDamageType* DamageType)
{
    if (bIsDead || DamageAmount <= 0.0f) return;

    // Применяем броню
    float ActualDamage = FMath::Max(0.0f, DamageAmount - Armor);
    CurrentHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

    // Оповещаем подписчиков (UI, звуки и т.д.)
    OnHealthChanged.Broadcast(this, CurrentHealth, -ActualDamage, DamageType);

    UE_LOG(LogTemp, Warning, TEXT("Health: %.1f / %.1f"), CurrentHealth, MaxHealth);

    if (CurrentHealth <= 0.0f)
    {
        bIsDead = true;
        OnDeath.Broadcast(GetOwner());
    }
}

void UAR_HealthComponent::Heal(float HealAmount)
{
    if (bIsDead || HealAmount <= 0.0f) return;

    CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
    OnHealthChanged.Broadcast(this, CurrentHealth, HealAmount, nullptr);
}

bool UAR_HealthComponent::IsDead() const
{
    return bIsDead;
}

float UAR_HealthComponent::GetHealthPercent() const
{
    return CurrentHealth / MaxHealth;
}