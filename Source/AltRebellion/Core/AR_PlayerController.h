#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "AR_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class AAR_CharacterBase;
class AAR_InteractableNPC;

UCLASS()
class ALTREBELLION_API AAR_PlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AAR_PlayerController();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void TogglePause();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> ShootAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> Ability1Action;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> Ability2Action;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> UltimateAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> InteractAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> PauseAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> DodgeAction;

protected:
    virtual void BeginPlay() override;

    bool bIsPaused = false;

    // Привязка действий ввода
    virtual void SetupInputComponent() override;

private:
    // Обработчики ввода
    void HandleMove(const FInputActionValue& Value);
    void HandleShoot(const FInputActionValue& Value);
    void HandleAbility1(const FInputActionValue& Value);
    void HandleAbility2(const FInputActionValue& Value);
    void HandleUltimate(const FInputActionValue& Value);
    void HandleDodge(const FInputActionValue& Value);
    void HandleInteract(const FInputActionValue& Value);
    void HandlePause(const FInputActionValue& Value);

    // Получить текущего персонажа
    AAR_CharacterBase* GetARCharacter() const;

    FVector GetAbilityTargetLocation(const AAR_CharacterBase* ARCharacter) const;

    AAR_InteractableNPC* FindNearestInteractableNPC() const;
};
