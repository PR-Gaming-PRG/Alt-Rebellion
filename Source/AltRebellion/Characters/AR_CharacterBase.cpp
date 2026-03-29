#include "Characters/AR_CharacterBase.h"
#include "Components/AR_HealthComponent.h"
#include "Components/AR_WeaponComponent.h"
#include "Components/AR_AbilityComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAR_CharacterBase::AAR_CharacterBase()
{
    // Создаём компоненты
    HealthComponent = CreateDefaultSubobject<UAR_HealthComponent>(TEXT("HealthComponent"));
    WeaponComponent = CreateDefaultSubobject<UAR_WeaponComponent>(TEXT("WeaponComponent"));
    AbilityComponent = CreateDefaultSubobject<UAR_AbilityComponent>(TEXT("AbilityComponent"));
}

void AAR_CharacterBase::BeginPlay()
{
    Super::BeginPlay();

    // Устанавливаем скорость
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    // Подписываемся на смерть
    if (HealthComponent)
    {
        HealthComponent->OnDeath.AddDynamic(this, &AAR_CharacterBase::OnCharacterDeath);
    }
}

void AAR_CharacterBase::Move(FVector2D MovementVector)
{
    if (MovementVector.IsNearlyZero()) return;

    // Движение относительно мира (top-down не зависит от поворота камеры)
    AddMovementInput(FVector(MovementVector.Y, MovementVector.X, 0.0f));
}

void AAR_CharacterBase::StartShooting(FVector TargetLocation)
{
    if (WeaponComponent)
    {
        WeaponComponent->Fire(TargetLocation);
    }
}

void AAR_CharacterBase::OnCharacterDeath_Implementation(AActor* DeadActor)
{
    // Базовая логика смерти — отключить ввод, запустить анимацию
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        DisableInput(PC);
    }

    // TODO: анимация смерти, уведомление GameMode
    UE_LOG(LogTemp, Warning, TEXT("Character %s died!"), *GetName());
}