#include "Interactables/AR_InteractableNPC.h"

#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"

AAR_InteractableNPC::AAR_InteractableNPC()
{
    PrimaryActorTick.bCanEverTick = false;

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(RootComponent);
    InteractionSphere->SetSphereRadius(180.0f);
    InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    NPCName = FText::FromString(TEXT("Миша"));
}

void AAR_InteractableNPC::BeginPlay()
{
    Super::BeginPlay();

    if (InteractionSphere)
    {
        InteractionSphere->OnComponentBeginOverlap.AddDynamic(
            this,
            &AAR_InteractableNPC::OnInteractionSphereBeginOverlap
        );

        InteractionSphere->OnComponentEndOverlap.AddDynamic(
            this,
            &AAR_InteractableNPC::OnInteractionSphereEndOverlap
        );
    }
}

void AAR_InteractableNPC::OnInteractionSphereBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    if (OtherActor->IsA(APawn::StaticClass()))
    {
        bPlayerInRange = true;

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Player entered interaction range with %s"),
            *NPCName.ToString()
        );
    }
}

void AAR_InteractableNPC::OnInteractionSphereEndOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    if (OtherActor->IsA(APawn::StaticClass()))
    {
        bPlayerInRange = false;

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Player left interaction range with %s"),
            *NPCName.ToString()
        );
    }
}

bool AAR_InteractableNPC::CanInteract() const
{
    return bPlayerInRange;
}

void AAR_InteractableNPC::Interact(AActor* Interactor)
{
    if (!CanInteract())
    {
        UE_LOG(LogTemp, Warning, TEXT("Interact failed: player is not in range"));
        return;
    }

    OnInteract(Interactor);
}

void AAR_InteractableNPC::OnInteract_Implementation(AActor* Interactor)
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Interacted with NPC: %s"),
        *NPCName.ToString()
    );

    OpenUpgradeMenu(Interactor);
}

void AAR_InteractableNPC::OpenUpgradeMenu(AActor* Interactor)
{
    if (!UpgradeWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpgradeWidgetClass is not set on NPC"));
        return;
    }

    APlayerController* PC = nullptr;

    if (APawn* Pawn = Cast<APawn>(Interactor))
    {
        PC = Cast<APlayerController>(Pawn->GetController());
    }

    if (!PC)
    {
        PC = GetWorld()->GetFirstPlayerController();
    }

    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("OpenUpgradeMenu failed: PlayerController is null"));
        return;
    }

    CurrentUpgradeWidget = CreateWidget<UUserWidget>(PC, UpgradeWidgetClass);

    if (!CurrentUpgradeWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("OpenUpgradeMenu failed: widget was not created"));
        return;
    }

    CurrentUpgradeWidget->AddToViewport();

    PC->SetPause(true);
    PC->bShowMouseCursor = true;

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(CurrentUpgradeWidget->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(InputMode);

    UE_LOG(LogTemp, Warning, TEXT("Upgrade menu opened by NPC"));
}