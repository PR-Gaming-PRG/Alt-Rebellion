#include "Loot/AR_LootDrop.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
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
    // Подбирает только игрок
    AAR_CharacterBase* Player = Cast<AAR_CharacterBase>(OtherActor);
    if (Player)
    {
        PickUp(Player);
    }
}

void AAR_LootDrop::PickUp(AActor* Collector)
{
    if (bIsPickedUp) return;
    bIsPickedUp = true;

    AAR_CharacterBase* Character = Cast<AAR_CharacterBase>(Collector);
    if (!Character) return;

    switch (LootType)
    {
        case ELootType::HealthPack:
            if (Character->HealthComponent)
            {
                Character->HealthComponent->Heal(Value);
            }
            break;

        case ELootType::AmmoBox:
            if (Character->WeaponComponent)
            {
                int32 NewAmmo = Character->WeaponComponent->CurrentAmmo + Amount;
                int32 MaxAmmo = Character->WeaponComponent->AmmoPerClip;
                Character->WeaponComponent->CurrentAmmo = FMath::Min(NewAmmo, MaxAmmo);
            }
            break;

        case ELootType::Token:
        {
            UAR_GameInstance* GI = Cast<UAR_GameInstance>(
                UGameplayStatics::GetGameInstance(GetWorld())
            );
            if (GI)
            {
                int32& Tokens = GI->Resources.FindOrAdd(TEXT("Tokens"));
                Tokens += Amount;
                UE_LOG(LogTemp, Warning, TEXT("Tokens: %d"), Tokens);
            }
            break;
        }

        case ELootType::AbilityBoost:
            // Позже здесь: применить буст способностей
            UE_LOG(LogTemp, Warning, TEXT("AbilityBoost picked up!"));
            break;
    }

    OnPickedUp(Collector);
}

void AAR_LootDrop::OnPickedUp_Implementation(AActor* Collector)
{
    // Базовая логика - уничтожить актор
    // В Blueprint: добавить VFX, звук перед уничтожением
    Destroy();
}