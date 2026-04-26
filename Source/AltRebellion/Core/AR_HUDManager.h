#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AR_HUDManager.generated.h"

class UUserWidget;

UCLASS()
class ALTREBELLION_API AAR_HUDManager : public AHUD
{
    GENERATED_BODY()

public:
    AAR_HUDManager();

    // Класс виджета HUD - назначается в Blueprint
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> HUDWidgetClass;

    // Текущий виджет HUD
    UPROPERTY(BlueprintReadOnly, Category = "UI")
    TObjectPtr<UUserWidget> HUDWidget;

    // Показать HUD
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowHUD_Widget();

    // Скрыть HUD
    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideHUD_Widget();

protected:
    virtual void BeginPlay() override;
};