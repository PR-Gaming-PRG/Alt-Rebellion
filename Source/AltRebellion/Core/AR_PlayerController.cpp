#include "Core/AR_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/AR_AbilityComponent.h"
#include "Characters/AR_CharacterBase.h"
#include "Interactables/AR_InteractableNPC.h"
#include "Kismet/GameplayStatics.h"

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
    ARCharacter->AbilityComponent->ActivateAbility1AtLocation(GetAbilityTargetLocation(ARCharacter));
}

void AAR_PlayerController::HandleAbility2(const FInputActionValue& Value)
{
    AAR_CharacterBase* ARCharacter = GetARCharacter();
    if (!ARCharacter || !ARCharacter->AbilityComponent) return;
    ARCharacter->AbilityComponent->ActivateAbility2AtLocation(GetAbilityTargetLocation(ARCharacter));
}

void AAR_PlayerController::HandleUltimate(const FInputActionValue& Value)
{
    AAR_CharacterBase* ARCharacter = GetARCharacter();
    if (!ARCharacter || !ARCharacter->AbilityComponent) return;
    ARCharacter->AbilityComponent->ActivateUltimateAtLocation(GetAbilityTargetLocation(ARCharacter));
}

void AAR_PlayerController::HandleDodge(const FInputActionValue& Value)
{
    // TODO: логика уклонения
    UE_LOG(LogTemp, Warning, TEXT("Dodge!"));
}

void AAR_PlayerController::HandleInteract(const FInputActionValue& Value)
{
    AAR_CharacterBase* ARCharacter = GetARCharacter();

    if (!ARCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Interact failed: ARCharacter is null"));
        return;
    }

    AAR_InteractableNPC* NPC = FindNearestInteractableNPC();

    if (!NPC)
    {
        UE_LOG(LogTemp, Warning, TEXT("Interact failed: no interactable NPC nearby"));
        return;
    }

    NPC->Interact(ARCharacter);
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

FVector AAR_PlayerController::GetAbilityTargetLocation(const AAR_CharacterBase* ARCharacter) const
{
    if (!ARCharacter)
    {
        return FVector::ZeroVector;
    }

    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

    if (HitResult.bBlockingHit)
    {
        FVector TargetLocation = HitResult.Location;
        TargetLocation.Z = ARCharacter->GetActorLocation().Z;
        return TargetLocation;
    }

    return ARCharacter->GetActorLocation() + ARCharacter->GetActorForwardVector() * 600.0f;
}

AAR_InteractableNPC* AAR_PlayerController::FindNearestInteractableNPC() const
{
    AAR_CharacterBase* ARCharacter = GetARCharacter();

    if (!ARCharacter)
    {
        return nullptr;
    }

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        AAR_InteractableNPC::StaticClass(),
        FoundActors
    );

    AAR_InteractableNPC* BestNPC = nullptr;
    float BestDistanceSquared = TNumericLimits<float>::Max();

    const FVector PlayerLocation = ARCharacter->GetActorLocation();

    for (AActor* Actor : FoundActors)
    {
        AAR_InteractableNPC* NPC = Cast<AAR_InteractableNPC>(Actor);

        if (!NPC || !NPC->CanInteract())
        {
            continue;
        }

        const float DistanceSquared = FVector::DistSquared(
            PlayerLocation,
            NPC->GetActorLocation()
        );

        if (DistanceSquared < BestDistanceSquared)
        {
            BestDistanceSquared = DistanceSquared;
            BestNPC = NPC;
        }
    }

    return BestNPC;
}
