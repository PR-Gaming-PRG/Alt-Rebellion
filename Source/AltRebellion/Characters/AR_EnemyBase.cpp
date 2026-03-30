#include "Characters/AR_EnemyBase.h"
#include "Components/AR_HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAR_EnemyBase::AAR_EnemyBase()
{
    PrimaryActorTick.bCanEverTick = false;

    HealthComponent = CreateDefaultSubobject<UAR_HealthComponent>(TEXT("HealthComponent"));
}

void AAR_EnemyBase::BeginPlay()
{
    Super::BeginPlay();

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    if (HealthComponent)
    {
        HealthComponent->OnDeath.AddDynamic(this, &AAR_EnemyBase::OnEnemyDeath);
    }
}

void AAR_EnemyBase::PerformAttack(AActor* Target)
{
    if (!Target) return;

    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastAttackTime < AttackCooldown) return;
    LastAttackTime = CurrentTime;

    // Проверяем дистанцию
    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > AttackRange) return;

    // Применяем урон через HealthComponent цели
    UAR_HealthComponent* TargetHealth = Target->FindComponentByClass<UAR_HealthComponent>();
    if (TargetHealth)
    {
        TargetHealth->ApplyDamage(AttackDamage, nullptr);
        UE_LOG(LogTemp, Warning, TEXT("Enemy attacked %s for %.1f damage"), *Target->GetName(), AttackDamage);
    }
}

void AAR_EnemyBase::OnEnemyDeath_Implementation(AActor* DeadActor)
{
    // Отключаем коллизию и движение
    SetActorEnableCollision(false);
    GetCharacterMovement()->DisableMovement();

    // Позже здесь: дроп лута, анимация смерти, уничтожение актора
    UE_LOG(LogTemp, Warning, TEXT("Enemy %s died! Reward: %d tokens"), *GetName(), TokenReward);

    // Удаляем через 2 секунды
    SetLifeSpan(2.0f);
}