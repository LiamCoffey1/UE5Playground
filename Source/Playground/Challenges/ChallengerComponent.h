// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <EventBus/EventBusComponent.h>
#include "ChallengerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCounterUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAYGROUND_API UCounterSet : public UObject {

	GENERATED_BODY()

public:
	TMap<FString, int32> CounterMap;

	UPROPERTY(BlueprintAssignable, Category = "CustomEvents")
	FOnCounterUpdated OnCounterUpdated;

	void AddCounter(FString CounterEvent) {
		CounterMap.Add(CounterEvent, 0);
	}

protected:

public:

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
		void YourFunction(const FString& QuestID) {
		if (CounterMap.Contains(*QuestID)) {
			CounterMap[*QuestID]++;
			OnCounterUpdated.Broadcast();
		}
	}
};
USTRUCT(BlueprintType)
struct FChallenge {
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Goal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CurrentProgress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ChallengeName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ChallengeDescription;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLAYGROUND_API UChallengerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCounterSet* Counters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChallenge> Challenges;
	UChallengerComponent();

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
		void UpdateChallenges() {
			int index = 0;
			for (auto& Elem : Counters->CounterMap) {
				FChallenge challenge = Challenges[index];
				if ((challenge.CurrentProgress < challenge.Goal) && (Elem.Value >= challenge.Goal)) {
					INotifications* handler = Cast<INotifications>(GetOwner());
					Notification* alert = NotificationFactory().CreateGameMessageNotification("Challenge Complete " + challenge.ChallengeName);
					handler->SendNotification(alert);
				}
				Challenges[index].CurrentProgress = Elem.Value;
				index++;
			}
		}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
