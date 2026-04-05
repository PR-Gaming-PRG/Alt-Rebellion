#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPlayer.generated.h"

UCLASS()
class ALTREBELLION_API UBTTask_FindPlayer : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_FindPlayer();

protected:
    virtual EBTNodeResult::Type ExecuteTask(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory) override;
};