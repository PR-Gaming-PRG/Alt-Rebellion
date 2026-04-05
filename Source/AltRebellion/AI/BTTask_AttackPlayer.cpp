#include "AI/BTTask_AttackPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/AR_AIController.h"
#include "Characters/AR_EnemyBase.h"

UBTTask_AttackPlayer::UBTTask_AttackPlayer()
{
    NodeName = TEXT("Attack Player");
}

EBTNodeResult::Type UBTTask_AttackPlayer::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAR_AIController* AIController = Cast<AAR_AIController>(OwnerComp.GetAIOwner());
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!AIController || !Blackboard) return EBTNodeResult::Failed;

    // Получаем врага
    AAR_EnemyBase* Enemy = Cast<AAR_EnemyBase>(AIController->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    // Получаем цель из Blackboard
    AActor* Target = Cast<AActor>(
        Blackboard->GetValueAsObject(AAR_AIController::BlackboardKey_TargetActor)
    );
    if (!Target) return EBTNodeResult::Failed;

    // Атакуем
    Enemy->PerformAttack(Target);

    return EBTNodeResult::Succeeded;
}