#include "Core/AR_HUDManager.h"
#include "Blueprint/UserWidget.h"

AAR_HUDManager::AAR_HUDManager()
{
}

void AAR_HUDManager::BeginPlay()
{
    Super::BeginPlay();
    ShowHUD_Widget();
}

void AAR_HUDManager::ShowHUD_Widget()
{
    if (!HUDWidgetClass) return;

    if (!HUDWidget)
    {
        HUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
    }

    if (HUDWidget)
    {
        HUDWidget->AddToViewport();
    }
}

void AAR_HUDManager::HideHUD_Widget()
{
    if (HUDWidget)
    {
        HUDWidget->RemoveFromParent();
    }
}