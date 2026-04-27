#include "Characters/AR_BossBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/AR_AIController.h"
#include "Components/AR_HealthComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AAR_BossBase::AAR_BossBase()
{
}

void AAR_BossBase::BeginPlay()
{
    Super::BeginPlay();

    // Подписываемся на изменение HP для проверки фаз
    if (HealthComponent)
    {
        HealthComponent->OnHealthChanged.AddDynamic(
            this,
            &AAR_BossBase::CheckPhaseTransition
        );
    }
}

void AAR_BossBase::CheckPhaseTransition(
    UAR_HealthComponent* HealthComp,
    float Health,
    float HealthDelta,
    const UDamageType* DamageType)
{
    if (!HealthComp) return;

    float HealthPercent = HealthComp->GetHealthPercent();
    EBossPhase NewPhase = CurrentPhase;

    // Определяем новую фазу по проценту HP
    if (HealthPercent <= 0.25f && CurrentPhase != EBossPhase::Phase3)
    {
        NewPhase = EBossPhase::Phase3;
    }
    else if (HealthPercent <= 0.5f && CurrentPhase == EBossPhase::Phase1)
    {
        NewPhase = EBossPhase::Phase2;
    }

    // Если фаза изменилась
    if (NewPhase != CurrentPhase)
    {
        EBossPhase OldPhase = CurrentPhase;
        CurrentPhase = NewPhase;

        // Спавним подкрепления
        SpawnReinforcements();

        // Оповещаем подписчиков
        OnPhaseChanged.Broadcast(OldPhase, NewPhase);

        // Вызываем Blueprint событие
        OnPhaseTransition(NewPhase);

        UE_LOG(LogTemp, Warning, TEXT("Boss phase changed to %d"), (int32)NewPhase);
    }
}

void AAR_BossBase::SpawnReinforcements()
{
    if (!ReinforcementClass)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World, 0);

    for (int32 i = 0; i < ReinforcementsPerPhase; i++)
    {
        FVector SpawnOffset = FVector(
            FMath::RandRange(-300.0f, 300.0f),
            FMath::RandRange(-300.0f, 300.0f),
            0.0f
        );

        FVector SpawnLocation = GetActorLocation() + SpawnOffset;
        FRotator SpawnRotation = FRotator::ZeroRotator;

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AAR_EnemyBase* SpawnedEnemy = World->SpawnActor<AAR_EnemyBase>(
            ReinforcementClass,
            SpawnLocation,
            SpawnRotation,
            SpawnParams
        );

        if (!SpawnedEnemy)
        {
            continue;
        }

        SpawnedEnemy->SpawnDefaultController();

        AAR_AIController* AIController = Cast<AAR_AIController>(SpawnedEnemy->GetController());

        if (AIController && AIController->GetBlackboardComponent() && PlayerCharacter)
        {
            UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();

            Blackboard->SetValueAsObject(
                AAR_AIController::BlackboardKey_TargetActor,
                PlayerCharacter
            );

            Blackboard->SetValueAsVector(
                AAR_AIController::BlackboardKey_TargetLocation,
                PlayerCharacter->GetActorLocation()
            );

            Blackboard->SetValueAsBool(
                AAR_AIController::BlackboardKey_CanSeeTarget,
                true
            );

            UE_LOG(
                LogTemp,
                Warning,
                TEXT("Reinforcement AI initialized with target: %s"),
                *PlayerCharacter->GetName()
            );
        }
        else
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("Reinforcement spawned, but AI/Blackboard/Player is missing")
            );
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Boss spawned %d reinforcements"), ReinforcementsPerPhase);
}

void AAR_BossBase::OnPhaseTransition_Implementation(EBossPhase NewPhase)
{
    // Базовая логика в Blueprint переопределяется для VFX, звуков и т.д.
    UE_LOG(LogTemp, Warning, TEXT("Boss phase transition!"));
}