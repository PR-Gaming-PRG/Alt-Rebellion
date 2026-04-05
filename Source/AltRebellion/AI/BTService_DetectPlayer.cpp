#include "AI/BTService_DetectPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/AR_AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UBTService_DetectPlayer::UBTService_DetectPlayer()
{
    NodeName = TEXT("Detect Player");
    Interval = 0.5f; // проверяем каждые 0.5 секунды
}

void UBTService_DetectPlayer::TickNode(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAR_AIController* AIController = Cast<AAR_AIController>(OwnerComp.GetAIOwner());
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!AIController || !Blackboard) return;

    APawn* EnemyPawn = AIController->GetPawn();
    if (!EnemyPawn) return;

    // Ищем игрока
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!PlayerCharacter) return;

    // Проверяем дистанцию
    float Distance = FVector::Dist(
        EnemyPawn->GetActorLocation(),
        PlayerCharacter->GetActorLocation()
    );

    bool bCanSee = Distance <= DetectionRadius;

    // Обновляем Blackboard
    Blackboard->SetValueAsBool(
        AAR_AIController::BlackboardKey_CanSeeTarget,
        bCanSee
    );

    if (bCanSee)
    {
        Blackboard->SetValueAsObject(
            AAR_AIController::BlackboardKey_TargetActor,
            PlayerCharacter
        );
        Blackboard->SetValueAsVector(
            AAR_AIController::BlackboardKey_TargetLocation,
            PlayerCharacter->GetActorLocation()
        );
    }
}