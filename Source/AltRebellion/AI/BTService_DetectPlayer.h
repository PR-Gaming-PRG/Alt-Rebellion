#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DetectPlayer.generated.h"

UCLASS()
class ALTREBELLION_API UBTService_DetectPlayer : public UBTService
{
    GENERATED_BODY()

public:
    UBTService_DetectPlayer();

    // Радиус обнаружения игрока
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float DetectionRadius = 1000.0f;

protected:
    virtual void TickNode(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory,
        float DeltaSeconds) override;
};