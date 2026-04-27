#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AR_InteractableNPC.generated.h"

class USphereComponent;
class UUserWidget;

UCLASS()
class ALTREBELLION_API AAR_InteractableNPC : public ACharacter
{
    GENERATED_BODY()

public:
    AAR_InteractableNPC();

protected:
    virtual void BeginPlay() override;

    // Радиус взаимодействия с NPC
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> InteractionSphere;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    FText NPCName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    FText InteractionPromptText = FText::FromString(TEXT("Нажмите F, чтобы поговорить"));

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> UpgradeWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    bool bPlayerInRange = false;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    TObjectPtr<UUserWidget> CurrentUpgradeWidget;

    // Доступен ли NPC сразу при старте уровня
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    bool bStartInteractionEnabled = false;

    // Скрывать ли NPC, пока он недоступен
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    bool bHideWhenDisabled = true;

    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    bool bInteractionEnabled = false;

public:
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void Interact(AActor* Interactor);

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    bool CanInteract() const;

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void SetInteractionEnabled(bool bEnabled);

protected:
    UFUNCTION()
    void OnInteractionSphereBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnInteractionSphereEndOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

    UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
    void OnInteract(AActor* Interactor);

    virtual void OnInteract_Implementation(AActor* Interactor);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void OpenUpgradeMenu(AActor* Interactor);
};