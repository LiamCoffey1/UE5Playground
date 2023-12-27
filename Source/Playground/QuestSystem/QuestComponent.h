// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Playground/GenericStructs.h>
#include <Playground/InventorySystem/InventoryComponent.h>
#include <Playground/Notifications/Notifications.h>
#include "QuestComponent.generated.h"

UENUM()
enum class EQuestStatus : uint8 {
	STARTED,
	IN_PROGRESS,
	FINISHED
};
USTRUCT(BlueprintType)
struct FQuestObjective {
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ObjectiveName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Complete;
};
USTRUCT(BlueprintType)
struct FQuestStage {
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StageName;
	int objectivesComplete = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FQuestObjective> Objectives;
};
USTRUCT(BlueprintType)
struct FQuest {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int currentStage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString QuestName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestStatus Status;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FQuestStage> Stages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItem> ItemRewards;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeTrackedQuestDelegateType, int32, QuestID);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLAYGROUND_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuestComponent();

	UPROPERTY(BlueprintAssignable, Category = "MyCategory")
	FChangeTrackedQuestDelegateType OnTrackQuest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentlyTrackedQuest = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FQuest> ActiveQuests;

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
	void TrackQuest(int questID) {
		CurrentlyTrackedQuest = questID;
		OnTrackQuest.Broadcast(questID);
	}

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
	void CompleteObjective(int questID, int objectiveIndex) {
		FQuest* quest = &ActiveQuests[questID];
		FQuestStage* stage = &quest->Stages[quest->currentStage];
		stage->Objectives[objectiveIndex].Complete = true;
		stage->objectivesComplete++;
		if (stage->objectivesComplete == stage->Objectives.Num()) {
			if (quest->currentStage + 1 < quest->Stages.Num()) {
				quest->currentStage++;
				INotifications* handler = Cast<INotifications>(GetOwner());
				Notification* alert = NotificationFactory().CreateGameMessageNotification("Objective Complete " + stage->Objectives[objectiveIndex].ObjectiveName);
				handler->SendNotification(alert);
			} else {
				CompleteQuest(questID);
			}
		}
		OnTrackQuest.Broadcast(CurrentlyTrackedQuest);
	}

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
	void CompleteQuest(int questID) {
		FQuest* quest = &ActiveQuests[questID];
		GetOwner()->GetComponentByClass<UInventoryComponent>()->AddToStorage(quest->ItemRewards, FStorageTypes::INVENTORY);
		INotifications* handler = Cast<INotifications>(GetOwner());
		Notification* alert = NotificationFactory().CreateGameMessageNotification("Quest Complete: " + quest->QuestName);
		handler->SendNotification(alert);
		ActiveQuests.RemoveAt(questID);
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
