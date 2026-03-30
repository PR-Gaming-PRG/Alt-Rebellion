#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AR_AIController.generated.h"

class UBehaviorTree;
class UBlackboardComponent;

UCLASS()
class ALTREBELLION_API AAR_AIController : public AAIController
{
    GENERATED_BODY()

public:
    AAR_AIController();

    // BehaviorTree который запускается для этого врага
    // Назначается в Blueprint наследнике
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTree;

    // Ключи Blackboard
    static const FName BlackboardKey_TargetActor;
    static const FName BlackboardKey_TargetLocation;
    static const FName BlackboardKey_CanSeeTarget;

protected:
    virtual void OnPossess(APawn* InPawn) override;
};