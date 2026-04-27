#include "Loot/AR_LootDrop.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AR_AbilityComponent.h"
#include "Components/AR_HealthComponent.h"
#include "Components/AR_WeaponComponent.h"
#include "Characters/AR_CharacterBase.h"
#include "Core/AR_GameInstance.h"
#include "Kismet/GameplayStatics.h"

AAR_LootDrop::AAR_LootDrop()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
    PickupSphere->SetupAttachment(RootComponent);
    PickupSphere->SetSphereRadius(75.0f);
    PickupSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AAR_LootDrop::BeginPlay()
{
    Super::BeginPlay();

    // Подписываемся на оверлап - автоподбор при приближении
    PickupSphere->OnComponentBeginOverlap.AddDynamic(
        this,
        &AAR_LootDrop::OnSphereOverlap
    );
}

void AAR_LootDrop::OnSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Overlap! Actor: %s"), *OtherActor->GetName());

    // Подбирает только игрок
    AAR_CharacterBase* Player = Cast<AAR_CharacterBase>(OtherActor);
    if (Player)
    {
        PickUp(Player);
    }
}

void AAR_LootDrop::PickUp(AActor* Collector)
{
    FString LootTypeName;
    switch(LootType)
    {
        case ELootType::HealthPack: LootTypeName = "HealthPack"; break;
        case ELootType::AmmoBox: LootTypeName = "AmmoBox"; break;
        case ELootType::Token: LootTypeName = "Token"; break;
        case ELootType::AbilityBoost: LootTypeName = "AbilityBoost"; break;
    }
    UE_LOG(LogTemp, Warning, TEXT("PickUp called! LootType: %s"), *LootTypeName);

    if (bIsPickedUp) return;
    bIsPickedUp = true;

    AAR_CharacterBase* Character = Cast<AAR_CharacterBase>(Collector);
    if (!Character) return;

    switch (LootType)
    {
        case ELootType::HealthPack:
        {
            if (Character->HealthComponent)
            {
                Character->HealthComponent->Heal(Value);

                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("Health picked up. Heal Value: %.1f"),
                    Value
                );
            }

            break;
        }
        
        case ELootType::AmmoBox:
        {
            if (Character->WeaponComponent)
            {
                const int32 NewAmmo = Character->WeaponComponent->CurrentAmmo + Amount;
                const int32 MaxAmmo = Character->WeaponComponent->AmmoPerClip;

                Character->WeaponComponent->CurrentAmmo = FMath::Min(NewAmmo, MaxAmmo);

                Character->WeaponComponent->OnAmmoChanged.Broadcast(
                    Character->WeaponComponent->CurrentAmmo,
                    Character->WeaponComponent->AmmoPerClip
                );

                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("Ammo picked up. Current Ammo: %d / %d"),
                    Character->WeaponComponent->CurrentAmmo,
                    Character->WeaponComponent->AmmoPerClip
                );
            }

            break;
        }

        case ELootType::Token:
        {
            UAR_GameInstance* GI = Cast<UAR_GameInstance>(
                UGameplayStatics::GetGameInstance(GetWorld())
            );

            if (GI)
            {
                GI->AddResource(TEXT("Tokens"), Amount);
                GI->SaveGame();

                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("Token picked up. Added: %d, Total Tokens: %d"),
                    Amount,
                    GI->GetResourceAmount(TEXT("Tokens"))
                );
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Token pickup failed: GameInstance is null"));
            }

            break;
        }

        case ELootType::AbilityBoost:
        {
            if (Character->AbilityComponent)
            {
                if (Value > 0.0f)
                {
                    Character->AbilityComponent->ReduceAbilityCooldowns(Value);
                }
                else
                {
                    Character->AbilityComponent->ResetAbilityCooldowns();
                }

                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("AbilityBoost picked up. Cooldowns reduced by %.1f seconds"),
                    Value
                );
            }

            break;
        }
    }

    OnPickedUp(Collector);
}

void AAR_LootDrop::OnPickedUp_Implementation(AActor* Collector)
{
    // Базовая логика - уничтожить актор
    // В Blueprint: добавить VFX, звук перед уничтожением
    Destroy();
}
