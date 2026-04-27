#include "Characters/AR_EnemyBase.h"

#include "Components/AR_HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Core/AR_GameInstance.h"
#include "Loot/AR_LootDrop.h"

FOnAnyEnemyKilled AAR_EnemyBase::OnAnyEnemyKilled;

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
    if (!Target)
    {
        return;
    }

    float CurrentTime = GetWorld()->GetTimeSeconds();

    if (CurrentTime - LastAttackTime < AttackCooldown)
    {
        return;
    }

    LastAttackTime = CurrentTime;

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    if (Distance > AttackRange)
    {
        return;
    }

    UAR_HealthComponent* TargetHealth = Target->FindComponentByClass<UAR_HealthComponent>();

    if (TargetHealth)
    {
        TargetHealth->ApplyDamageWithCauser(AttackDamage, nullptr, this);

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Enemy attacked %s for %.1f damage"),
            *Target->GetName(),
            AttackDamage
        );
    }
}

void AAR_EnemyBase::OnEnemyDeath_Implementation(AActor* DeadActor)
{
    SetActorEnableCollision(false);
    GetCharacterMovement()->DisableMovement();

    OnAnyEnemyKilled.Broadcast(this);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Enemy %s died! Reward: %d tokens"),
        *GetName(),
        TokenReward
    );

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("LootClasses count: %d"),
        LootClasses.Num()
    );

    UAR_GameInstance* GI = Cast<UAR_GameInstance>(
        UGameplayStatics::GetGameInstance(GetWorld())
    );

    if (GI && TokenReward > 0)
    {
        GI->AddResource(TEXT("Tokens"), TokenReward);
        GI->SaveGame();

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Tokens added: %d, Total: %d"),
            TokenReward,
            GI->GetResourceAmount(TEXT("Tokens"))
        );
    }

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