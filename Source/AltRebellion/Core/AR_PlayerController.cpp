#include "AR_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AAR_PlayerController::AAR_PlayerController()
{
    bShowMouseCursor = true;
}

void AAR_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void AAR_PlayerController::TogglePause()
{
    bIsPaused = !bIsPaused;
    SetPause(bIsPaused);
    UE_LOG(LogTemp, Warning, TEXT("Pause: %s"), bIsPaused ? TEXT("ON") : TEXT("OFF"));
}