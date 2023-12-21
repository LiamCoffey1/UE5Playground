#include "HarvestingInteractionComponent.h"
#include "Playground/InventorySystem/InventoryComponent.h"
#include "Playground/PlaygroundCharacter.h"

void UHarvestingInteractionComponent::BeginPlay() {
    Super::BeginPlay();
    currentCapacity = maxCapacity;
}

void UHarvestingInteractionComponent::Interact(AActor* Initiator) {
    LookAtActor(Initiator);
    Harvester = Initiator;
    APlaygroundCharacter* CustomPlayerCharacter = Cast<APlaygroundCharacter>(Initiator);
    if (CustomPlayerCharacter) {
        CustomPlayerCharacter->StartTask(new HarvestTask(Initiator, this));
    }
}

void UHarvestingInteractionComponent::Respawn() {
    UpdateCapacity(maxCapacity);
    USceneComponent* RootComponent = GetOwner()->FindComponentByClass<USceneComponent>();
    if (RootComponent) {
        RootComponent->SetVisibility(true);
        GetOwner()->SetActorEnableCollision(true);
    }
}

void UHarvestingInteractionComponent::UpdateCapacity(int capacity) {
    currentCapacity = capacity;
    if (currentCapacity <= 0) {
        USceneComponent* RootComponent = GetOwner()->FindComponentByClass<USceneComponent>();
        if (RootComponent) {
            RootComponent->SetVisibility(false);
            GetOwner()->SetActorEnableCollision(false);
        }
        GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &UHarvestingInteractionComponent::Respawn, 4, false);
    }
}
