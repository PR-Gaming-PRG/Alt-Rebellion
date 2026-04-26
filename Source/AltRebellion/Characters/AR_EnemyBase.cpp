#include "Characters/AR_EnemyBase.h"
#include "Components/AR_HealthComponent.h"
#include "Loot/AR_LootDrop.h"
#include "Core/AR_GameInstance.h"
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
    SetActorEnableCollision(false);
    GetCharacterMovement()->DisableMovement();

    UE_LOG(LogTemp, Warning, TEXT("Enemy %s died! Reward: %d tokens"), *GetName(), TokenReward);
    UE_LOG(LogTemp, Warning, TEXT("LootClasses count: %d"), LootClasses.Num());

    UAR_GameInstance* GI = Cast<UAR_GameInstance>(
        UGameplayStatics::GetGameInstance(GetWorld())
    );
    if (GI)
    {
        int32& Tokens = GI->Resources.FindOrAdd(TEXT("Tokens"));
        Tokens += TokenReward;
        UE_LOG(LogTemp, Warning, TEXT("Tokens added: %d, Total: %d"), TokenReward, Tokens);
    }

    // Дроп лута
    if (LootClasses.Num() > 0 && FMath::RandRange(0.0f, 1.0f) <= LootDropChance)
    {
        int32 RandomIndex = FMath::RandRange(0, LootClasses.Num() - 1);
        if (LootClasses[RandomIndex])
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride =
                ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            GetWorld()->SpawnActor<AAR_LootDrop>(
                LootClasses[RandomIndex],
                GetActorLocation(),
                FRotator::ZeroRotator,
                SpawnParams
            );
        }
    }

    SetLifeSpan(2.0f);
}