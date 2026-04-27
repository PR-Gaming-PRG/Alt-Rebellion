#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Core/AR_SaveGame.h"
#include "AR_GameInstance.generated.h"

class UDataTable;
struct FAR_AbilityUpgradeRow;

UCLASS()
class ALTREBELLION_API UAR_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    // Выбранный персонаж
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    FName SelectedCharacter;

    // Имя слота сохранения
    UPROPERTY(BlueprintReadWrite, Category = "Save")
    FString SaveSlotName = TEXT("SaveSlot_01");

    // Индекс пользователя для SaveGame
    UPROPERTY(BlueprintReadWrite, Category = "Save")
    int32 SaveUserIndex = 0;

    // Текущая зона
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    FName CurrentZoneID;

    // Прогресс по зонам
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    TArray<FZoneProgress> ZoneStates;

    // Ресурсы игрока: Tokens, Paint и т.д.
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    TMap<FName, int32> Resources;

    // Разблокированные способности
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    TArray<FName> UnlockedAbilities;

    // Уровни способностей
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    TMap<FName, int32> AbilityLevels;

    // Настройки
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    TMap<FName, float> Settings;

    // Сохранить текущее состояние игры
    UFUNCTION(BlueprintCallable, Category = "Save")
    bool SaveGame();

    // Загрузить сохранение
    UFUNCTION(BlueprintCallable, Category = "Save")
    bool LoadGame();

    // Проверить, есть ли файл сохранения
    UFUNCTION(BlueprintCallable, Category = "Save")
    bool HasSaveGame() const;

    // Удалить текущее сохранение
    UFUNCTION(BlueprintCallable, Category = "Save")
    bool DeleteSaveGame();

    // Создать новое состояние игры в памяти и сразу сохранить
    UFUNCTION(BlueprintCallable, Category = "Save")
    void StartNewGame(FName InitialCharacterID, FName InitialZoneID);

    // Добавить ресурс
    UFUNCTION(BlueprintCallable, Category = "Resources")
    void AddResource(FName ResourceID, int32 Amount);

    // Потратить ресурс. Возвращает true, если ресурса хватило
    UFUNCTION(BlueprintCallable, Category = "Resources")
    bool SpendResource(FName ResourceID, int32 Amount);

    // Получить количество ресурса
    UFUNCTION(BlueprintCallable, Category = "Resources")
    int32 GetResourceAmount(FName ResourceID) const;

    // Отметить зону освобождённой
    UFUNCTION(BlueprintCallable, Category = "Progress")
    void SetZoneLiberated(FName ZoneID, bool bLiberated);

    // Проверить, освобождена ли зона
    UFUNCTION(BlueprintCallable, Category = "Progress")
    bool IsZoneLiberated(FName ZoneID) const;

    // Получить уровень способности
    UFUNCTION(BlueprintCallable, Category = "Progression")
    int32 GetAbilityLevel(FName AbilityID) const;

    // Установить уровень способности
    UFUNCTION(BlueprintCallable, Category = "Progression")
    void SetAbilityLevel(FName AbilityID, int32 NewLevel);

    // Прокачать способность за жетоны
    UFUNCTION(BlueprintCallable, Category = "Progression")
    bool UpgradeAbility(FName AbilityID, int32 Cost, int32 MaxLevel);

    // Прокачать способность, используя DataTable улучшений
    UFUNCTION(BlueprintCallable, Category = "Progression")
    bool UpgradeAbilityFromTable(FName AbilityID, UDataTable* UpgradeTable);

    // Можно ли сейчас прокачать способность
    UFUNCTION(BlueprintCallable, Category = "Progression")
    bool CanUpgradeAbility(FName AbilityID, UDataTable* UpgradeTable) const;

    // Получить стоимость следующего уровня способности
    UFUNCTION(BlueprintCallable, Category = "Progression")
    int32 GetNextAbilityUpgradeCost(FName AbilityID, UDataTable* UpgradeTable) const;

    // Получить максимальный уровень способности из таблицы
    UFUNCTION(BlueprintCallable, Category = "Progression")
    int32 GetMaxAbilityLevelFromTable(FName AbilityID, UDataTable* UpgradeTable) const;

private:
    UAR_SaveGame* CreateSaveObjectFromCurrentState() const;
    void ApplySaveObjectToCurrentState(UAR_SaveGame* LoadedSave);

    const FAR_AbilityUpgradeRow* FindUpgradeRow(
        FName AbilityID,
        int32 TargetLevel,
        UDataTable* UpgradeTable
    ) const;
};