#pragma once

#include "CoreMinimal.h"
#include "WorldInteractionComponent.h"
#include "../InventorySystem/InventoryComponent.h"
#include "../GenericStructs.h"
#include "HarvestingInteractionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAYGROUND_API UHarvestingInteractionComponent : public UWorldInteractionComponent {
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:

    AActor* Harvester;

    FTimerHandle RespawnTimerHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
        int itemToAdd = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
        TArray<FItem> itemsGiven;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
        double chanceToRecieve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
        UAnimMontage* MontageToPlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
        int maxCapacity = 1;

    int currentCapacity;

    int GetCurrentCapacity() {
        return currentCapacity;
    }

    virtual void Interact(AActor* Initiator) override;

    void Respawn();

    void UpdateCapacity(int capacity);
};
