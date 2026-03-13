#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AR_PlayerController.generated.h"

UCLASS()
class ALTREBELLION_API AAR_PlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AAR_PlayerController();

    // Поставить/снять паузу
    UFUNCTION(BlueprintCallable, Category = "Input")
    void TogglePause();

protected:
    virtual void BeginPlay() override;

    // Флаг паузы
    bool bIsPaused = false;
};