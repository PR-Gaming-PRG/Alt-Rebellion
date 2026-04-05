#include "AI/BTTask_FindPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/AR_AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UBTTask_FindPlayer::UBTTask_FindPlayer()
{
    NodeName = TEXT("Find Player");
}

EBTNodeResult::Type UBTTask_FindPlayer::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    // Получаем контроллер и Blackboard
    AAR_AIController* AIController = Cast<AAR_AIController>(OwnerComp.GetAIOwner());
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!AIController || !Blackboard) return EBTNodeResult::Failed;

    // Находим игрока
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!PlayerCharacter) return EBTNodeResult::Failed;

    // Записываем в Blackboard
    Blackboard->SetValueAsObject(
        AAR_AIController::BlackboardKey_TargetActor,
        PlayerCharacter
    );
    Blackboard->SetValueAsVector(
        AAR_AIController::BlackboardKey_TargetLocation,
        PlayerCharacter->GetActorLocation()
    );

    return EBTNodeResult::Succeeded;
}