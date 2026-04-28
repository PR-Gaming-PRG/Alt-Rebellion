#include "Core/AR_GameInstance.h"

#include "Abilities/AR_AbilityUpgradeData.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
void UAR_GameInstance::Init()
{
    Super::Init();
    LoadGame();
}

bool UAR_GameInstance::HasSaveGame() const
{
    return UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex);
}

void UAR_GameInstance::SetSettingValue(FName SettingID, float Value)
{
    if (SettingID.IsNone())
    {
        return;
    }

    Settings.FindOrAdd(SettingID) = Value;
}

float UAR_GameInstance::GetSettingValue(FName SettingID, float DefaultValue) const
{
    if (SettingID.IsNone())
    {
        return DefaultValue;
    }

    const float* Value = Settings.Find(SettingID);
    return Value ? *Value : DefaultValue;
}

void UAR_GameInstance::SetSettingBool(FName SettingID, bool bValue)
{
    SetSettingValue(SettingID, bValue ? 1.0f : 0.0f);
}

bool UAR_GameInstance::GetSettingBool(FName SettingID, bool bDefaultValue) const
{
    return GetSettingValue(SettingID, bDefaultValue ? 1.0f : 0.0f) > 0.5f;
}

void UAR_GameInstance::SaveSettings()
{
    SaveGame();
}

bool UAR_GameInstance::SaveGame()
{
    UAR_SaveGame* SaveObject = CreateSaveObjectFromCurrentState();

    if (!SaveObject)
    {
        UE_LOG(LogTemp, Error, TEXT("SaveGame failed: SaveObject is null"));
        return false;
    }

    const bool bSuccess = UGameplayStatics::SaveGameToSlot(
        SaveObject,
        SaveSlotName,
        SaveUserIndex
    );

    if (bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game saved successfully to slot: %s"), *SaveSlotName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save game to slot: %s"), *SaveSlotName);
    }

    return bSuccess;
}

bool UAR_GameInstance::LoadGame()
{
    if (!HasSaveGame())
    {
        UE_LOG(LogTemp, Warning, TEXT("No save game found in slot: %s"), *SaveSlotName);
        return false;
    }

    USaveGame* LoadedObject = UGameplayStatics::LoadGameFromSlot(
        SaveSlotName,
        SaveUserIndex
    );

    UAR_SaveGame* LoadedSave = Cast<UAR_SaveGame>(LoadedObject);

    if (!LoadedSave)
    {
        UE_LOG(LogTemp, Error, TEXT("LoadGame failed: loaded object is not UAR_SaveGame"));
        return false;
    }

    ApplySaveObjectToCurrentState(LoadedSave);

    UE_LOG(LogTemp, Warning, TEXT("Game loaded successfully from slot: %s"), *SaveSlotName);
    return true;
}

bool UAR_GameInstance::DeleteSaveGame()
{
    if (!HasSaveGame())
    {
        UE_LOG(LogTemp, Warning, TEXT("DeleteSaveGame: no save game exists"));
        return false;
    }

    const bool bSuccess = UGameplayStatics::DeleteGameInSlot(
        SaveSlotName,
        SaveUserIndex
    );

    if (bSuccess)
    {
        SelectedCharacter = NAME_None;
        CurrentZoneID = NAME_None;
        ZoneStates.Empty();
        Resources.Empty();
        UnlockedAbilities.Empty();
        AbilityLevels.Empty();
        Settings.Empty();

        UE_LOG(LogTemp, Warning, TEXT("Save game deleted successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to delete save game"));
    }

    return bSuccess;
}

void UAR_GameInstance::StartNewGame(FName InitialCharacterID, FName InitialZoneID)
{
    SelectedCharacter = InitialCharacterID;
    CurrentZoneID = InitialZoneID;

    ZoneStates.Empty();
    Resources.Empty();
    UnlockedAbilities.Empty();
    AbilityLevels.Empty();

    // Базовые ресурсы
    Resources.Add(TEXT("Tokens"), 0);
    Resources.Add(TEXT("Paint"), 0);

    // Первая зона пока не освобождена
    FZoneProgress InitialZoneProgress;
    InitialZoneProgress.ZoneID = InitialZoneID;
    InitialZoneProgress.bLiberated = false;
    ZoneStates.Add(InitialZoneProgress);

    SaveGame();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("New game started. Character: %s, Zone: %s"),
        *InitialCharacterID.ToString(),
        *InitialZoneID.ToString()
    );
}

void UAR_GameInstance::AddResource(FName ResourceID, int32 Amount)
{
    if (ResourceID.IsNone() || Amount <= 0)
    {
        return;
    }

    int32& CurrentAmount = Resources.FindOrAdd(ResourceID);
    CurrentAmount += Amount;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Resource added: %s +%d. Total: %d"),
        *ResourceID.ToString(),
        Amount,
        CurrentAmount
    );
}

bool UAR_GameInstance::SpendResource(FName ResourceID, int32 Amount)
{
    if (ResourceID.IsNone() || Amount <= 0)
    {
        return false;
    }

    int32* CurrentAmount = Resources.Find(ResourceID);

    if (!CurrentAmount || *CurrentAmount < Amount)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Not enough resource: %s. Need: %d"),
            *ResourceID.ToString(),
            Amount
        );

        return false;
    }

    *CurrentAmount -= Amount;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Resource spent: %s -%d. Left: %d"),
        *ResourceID.ToString(),
        Amount,
        *CurrentAmount
    );

    return true;
}

int32 UAR_GameInstance::GetResourceAmount(FName ResourceID) const
{
    const int32* CurrentAmount = Resources.Find(ResourceID);
    return CurrentAmount ? *CurrentAmount : 0;
}

void UAR_GameInstance::SetZoneLiberated(FName ZoneID, bool bLiberated)
{
    if (ZoneID.IsNone())
    {
        return;
    }

    for (FZoneProgress& ZoneProgress : ZoneStates)
    {
        if (ZoneProgress.ZoneID == ZoneID)
        {
            ZoneProgress.bLiberated = bLiberated;
            return;
        }
    }

    FZoneProgress NewZoneProgress;
    NewZoneProgress.ZoneID = ZoneID;
    NewZoneProgress.bLiberated = bLiberated;

    ZoneStates.Add(NewZoneProgress);
}

bool UAR_GameInstance::IsZoneLiberated(FName ZoneID) const
{
    for (const FZoneProgress& ZoneProgress : ZoneStates)
    {
        if (ZoneProgress.ZoneID == ZoneID)
        {
            return ZoneProgress.bLiberated;
        }
    }

    return false;
}

int32 UAR_GameInstance::GetAbilityLevel(FName AbilityID) const
{
    const int32* Level = AbilityLevels.Find(AbilityID);
    return Level ? FMath::Max(1, *Level) : 1;
}

void UAR_GameInstance::SetAbilityLevel(FName AbilityID, int32 NewLevel)
{
    if (AbilityID.IsNone())
    {
        return;
    }

    AbilityLevels.FindOrAdd(AbilityID) = FMath::Max(1, NewLevel);
}

void UAR_GameInstance::UnlockAbility(FName AbilityID)
{
    if (AbilityID.IsNone())
    {
        return;
    }

    if (!UnlockedAbilities.Contains(AbilityID))
    {
        UnlockedAbilities.Add(AbilityID);
    }
}

bool UAR_GameInstance::IsAbilityUnlocked(FName AbilityID) const
{
    return !AbilityID.IsNone() && UnlockedAbilities.Contains(AbilityID);
}

bool UAR_GameInstance::UpgradeAbility(FName AbilityID, int32 Cost, int32 MaxLevel)
{
    if (AbilityID.IsNone() || Cost < 0 || MaxLevel <= 0)
    {
        return false;
    }

    const int32 CurrentLevel = GetAbilityLevel(AbilityID);

    if (CurrentLevel >= MaxLevel)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Ability already maxed: %s"),
            *AbilityID.ToString()
        );

        return false;
    }

    if (!SpendResource(TEXT("Tokens"), Cost))
    {
        return false;
    }

    const int32 NewLevel = CurrentLevel + 1;
    SetAbilityLevel(AbilityID, NewLevel);
    UnlockAbility(AbilityID);

    SaveGame();
    OnAbilityUpgraded.Broadcast(AbilityID, NewLevel);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Ability upgraded: %s. Level: %d / %d"),
        *AbilityID.ToString(),
        NewLevel,
        MaxLevel
    );

    return true;
}

UAR_SaveGame* UAR_GameInstance::CreateSaveObjectFromCurrentState() const
{
    USaveGame* SaveObjectBase = UGameplayStatics::CreateSaveGameObject(UAR_SaveGame::StaticClass());
    UAR_SaveGame* SaveObject = Cast<UAR_SaveGame>(SaveObjectBase);

    if (!SaveObject)
    {
        return nullptr;
    }

    SaveObject->SelectedCharacter = SelectedCharacter;
    SaveObject->CurrentZoneID = CurrentZoneID;
    SaveObject->ZoneStates = ZoneStates;
    SaveObject->Resources = Resources;
    SaveObject->UnlockedAbilities = UnlockedAbilities;
    SaveObject->AbilityLevels = AbilityLevels;
    SaveObject->Settings = Settings;

    return SaveObject;
}

void UAR_GameInstance::ApplySaveObjectToCurrentState(UAR_SaveGame* LoadedSave)
{
    if (!LoadedSave)
    {
        return;
    }

    SelectedCharacter = LoadedSave->SelectedCharacter;
    CurrentZoneID = LoadedSave->CurrentZoneID;
    ZoneStates = LoadedSave->ZoneStates;
    Resources = LoadedSave->Resources;
    UnlockedAbilities = LoadedSave->UnlockedAbilities;
    AbilityLevels = LoadedSave->AbilityLevels;
    Settings = LoadedSave->Settings;
}

const FAR_AbilityUpgradeRow* UAR_GameInstance::FindUpgradeRow(
    FName AbilityID,
    int32 TargetLevel,
    UDataTable* UpgradeTable
) const
{
    if (AbilityID.IsNone() || !UpgradeTable || TargetLevel <= 0)
    {
        return nullptr;
    }

    TArray<FAR_AbilityUpgradeRow*> Rows;
    UpgradeTable->GetAllRows<FAR_AbilityUpgradeRow>(
        TEXT("FindUpgradeRow"),
        Rows
    );

    for (const FAR_AbilityUpgradeRow* Row : Rows)
    {
        if (!Row)
        {
            continue;
        }

        if (Row->AbilityID == AbilityID && Row->Level == TargetLevel)
        {
            return Row;
        }
    }

    return nullptr;
}

int32 UAR_GameInstance::GetMaxAbilityLevelFromTable(
    FName AbilityID,
    UDataTable* UpgradeTable
) const
{
    if (AbilityID.IsNone() || !UpgradeTable)
    {
        return 0;
    }

    TArray<FAR_AbilityUpgradeRow*> Rows;
    UpgradeTable->GetAllRows<FAR_AbilityUpgradeRow>(
        TEXT("GetMaxAbilityLevelFromTable"),
        Rows
    );

    int32 MaxLevel = 0;

    for (const FAR_AbilityUpgradeRow* Row : Rows)
    {
        if (!Row)
        {
            continue;
        }

        if (Row->AbilityID == AbilityID)
        {
            MaxLevel = FMath::Max(MaxLevel, Row->Level);
        }
    }

    return MaxLevel;
}

int32 UAR_GameInstance::GetNextAbilityUpgradeCost(
    FName AbilityID,
    UDataTable* UpgradeTable
) const
{
    FAR_AbilityUpgradeRow UpgradeRow;

    if (!GetNextAbilityUpgradeRow(AbilityID, UpgradeTable, UpgradeRow))
    {
        return -1;
    }

    return UpgradeRow.TokenCost;
}

bool UAR_GameInstance::GetAbilityUpgradeRow(
    FName AbilityID,
    int32 Level,
    UDataTable* UpgradeTable,
    FAR_AbilityUpgradeRow& OutUpgradeRow
) const
{
    const FAR_AbilityUpgradeRow* UpgradeRow = FindUpgradeRow(
        AbilityID,
        Level,
        UpgradeTable
    );

    if (!UpgradeRow)
    {
        return false;
    }

    OutUpgradeRow = *UpgradeRow;
    return true;
}

bool UAR_GameInstance::GetNextAbilityUpgradeRow(
    FName AbilityID,
    UDataTable* UpgradeTable,
    FAR_AbilityUpgradeRow& OutUpgradeRow
) const
{
    const int32 CurrentLevel = GetAbilityLevel(AbilityID);
    const int32 TargetLevel = CurrentLevel + 1;

    const FAR_AbilityUpgradeRow* UpgradeRow = FindUpgradeRow(
        AbilityID,
        TargetLevel,
        UpgradeTable
    );

    if (!UpgradeRow)
    {
        return false;
    }

    OutUpgradeRow = *UpgradeRow;
    return true;
}

bool UAR_GameInstance::CanUpgradeAbility(
    FName AbilityID,
    UDataTable* UpgradeTable
) const
{
    if (AbilityID.IsNone() || !UpgradeTable)
    {
        return false;
    }

    const int32 CurrentLevel = GetAbilityLevel(AbilityID);
    const int32 TargetLevel = CurrentLevel + 1;

    const FAR_AbilityUpgradeRow* UpgradeRow = FindUpgradeRow(
        AbilityID,
        TargetLevel,
        UpgradeTable
    );

    if (!UpgradeRow)
    {
        return false;
    }

    return GetResourceAmount(TEXT("Tokens")) >= UpgradeRow->TokenCost;
}

bool UAR_GameInstance::UpgradeAbilityFromTable(
    FName AbilityID,
    UDataTable* UpgradeTable
)
{
    if (AbilityID.IsNone() || !UpgradeTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpgradeAbilityFromTable failed: invalid input"));
        return false;
    }

    const int32 CurrentLevel = GetAbilityLevel(AbilityID);
    const int32 TargetLevel = CurrentLevel + 1;

    const FAR_AbilityUpgradeRow* UpgradeRow = FindUpgradeRow(
        AbilityID,
        TargetLevel,
        UpgradeTable
    );

    if (!UpgradeRow)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("No upgrade row found for AbilityID: %s, TargetLevel: %d"),
            *AbilityID.ToString(),
            TargetLevel
        );

        return false;
    }

    if (!SpendResource(TEXT("Tokens"), UpgradeRow->TokenCost))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Not enough tokens for ability upgrade: %s. Cost: %d, Tokens: %d"),
            *AbilityID.ToString(),
            UpgradeRow->TokenCost,
            GetResourceAmount(TEXT("Tokens"))
        );

        return false;
    }

    SetAbilityLevel(AbilityID, TargetLevel);
    UnlockAbility(AbilityID);

    SaveGame();
    OnAbilityUpgraded.Broadcast(AbilityID, TargetLevel);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Ability upgraded from table: %s -> Level %d. Cost: %d. Tokens left: %d"),
        *AbilityID.ToString(),
        TargetLevel,
        UpgradeRow->TokenCost,
        GetResourceAmount(TEXT("Tokens"))
    );

    return true;
}
