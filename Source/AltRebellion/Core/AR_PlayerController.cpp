#include "Core/AR_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/AR_AbilityComponent.h"
#include "Characters/AR_CharacterBase.h"

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

void AAR_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
    if (!EIC) return;

    // Движение
    if (MoveAction)
        EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAR_PlayerController::HandleMove);

    // Стрельба
    if (ShootAction)
        EIC->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AAR_PlayerController::HandleShoot);

    // Способности
    if (Ability1Action)
        EIC->BindAction(Ability1Action, ETriggerEvent::Started, this, &AAR_PlayerController::HandleAbility1);

    if (Ability2Action)
        EIC->BindAction(Ability2Action, ETriggerEvent::Started, this, &AAR_PlayerController::HandleAbility2);

    if (UltimateAction)
        EIC->BindAction(UltimateAction, ETriggerEvent::Started, this, &AAR_PlayerController::HandleUltimate);

    // Уклонение
    if (DodgeAction)
        EIC->BindAction(DodgeAction, ETriggerEvent::Started, this, &AAR_PlayerController::HandleDodge);

    // Взаимодействие
    if (InteractAction)
        EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &AAR_PlayerController::HandleInteract);

    // Пауза
    if (PauseAction)
        EIC->BindAction(PauseAction, ETriggerEvent::Started, this, &AAR_PlayerController::HandlePause);
}

AAR_CharacterBase* AAR_PlayerController::GetARCharacter() const
{
    return Cast<AAR_CharacterBase>(GetPawn());
}

void AAR_PlayerController::HandleMove(const FInputActionValue& Value)
{
    AAR_CharacterBase* ARCharacter = GetARCharacter();
    if (!ARCharacter) return;
    FVector2D MovementVector = Value.Get<FVector2D>();
    ARCharacter->Move(MovementVector);
}

void AAR_PlayerController::HandleShoot(const FInputActionValue& Value)
{
    AAR_CharacterBase* ARCharacter = GetARCharacter();
    if (!ARCharacter) return;
    
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
    if (HitResult.bBlockingHit)
    {
        FVector TargetLocation = HitResult.Location;
        TargetLocation.Z = ARCharacter->GetActorLocation().Z;
        ARCharacter->StartShooting(TargetLocation);
    }
}

void AAR_PlayerController::HandleAbility1(const FInputActionValue& Value)
{
    AAR_CharacterBase* ARCharacter = GetARCharacter();
    if (!ARCharacter || !ARCharacter->AbilityComponent) return;
    ARCharacter->AbilityComponent->ActivateAbility1();
}

void AAR_PlayerController::HandleAbility2(const FInputActionValue& Value)
{
    AAR_CharacterBase* ARCharacter = GetARCharacter();
    if (!ARCharacter || !ARCharacter->AbilityComponent) return;
    ARCharacter->AbilityComponent->ActivateAbility2();
}

void AAR_PlayerController::HandleUltimate(const FInputActionValue& Value)
{
    AAR_CharacterBase* ARCharacter = GetARCharacter();
    if (!ARCharacter || !ARCharacter->AbilityComponent) return;
    ARCharacter->AbilityComponent->ActivateUltimate();
}

void AAR_PlayerController::HandleDodge(const FInputActionValue& Value)
{
    // TODO: логика уклонения
    UE_LOG(LogTemp, Warning, TEXT("Dodge!"));
}

void AAR_PlayerController::HandleInteract(const FInputActionValue& Value)
{
    // TODO: логика взаимодействия
    UE_LOG(LogTemp, Warning, TEXT("Interact!"));
}

void AAR_PlayerController::HandlePause(const FInputActionValue& Value)
{
    TogglePause();
}

void AAR_PlayerController::TogglePause()
{
    bIsPaused = !bIsPaused;
    SetPause(bIsPaused);
    UE_LOG(LogTemp, Warning, TEXT("Pause: %s"), bIsPaused ? TEXT("ON") : TEXT("OFF"));
}