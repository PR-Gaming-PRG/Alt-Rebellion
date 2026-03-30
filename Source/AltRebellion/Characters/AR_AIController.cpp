#include "Characters/AR_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

// Имена ключей Blackboard
const FName AAR_AIController::BlackboardKey_TargetActor = TEXT("TargetActor");
const FName AAR_AIController::BlackboardKey_TargetLocation = TEXT("TargetLocation");
const FName AAR_AIController::BlackboardKey_CanSeeTarget = TEXT("CanSeeTarget");

AAR_AIController::AAR_AIController()
{
}

void AAR_AIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTree)
    {
        RunBehaviorTree(BehaviorTree);
    }
}