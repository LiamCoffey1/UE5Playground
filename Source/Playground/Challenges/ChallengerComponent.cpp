// Fill out your copyright notice in the Description page of Project Settings.


#include "Challenges/ChallengerComponent.h"

// Sets default values for this component's properties
UChallengerComponent::UChallengerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UChallengerComponent::BeginPlay()
{
	Super::BeginPlay();
	FChallenge challenge = FChallenge();
	challenge.ChallengeName = "Harvest 10 Ore";
	challenge.ChallengeDescription = "Harvest 10 Ore";
	challenge.CurrentProgress = 0;
	challenge.Goal = 10;
	Challenges.Add(challenge);
	Counters = NewObject<UCounterSet>(this);
	Counters->AddCounter(FString(TEXT("Harvest")));
	Counters->OnCounterUpdated.AddDynamic(this, &UChallengerComponent::UpdateChallenges);
	UEventBusComponent* bus = GetOwner()->GetComponentByClass<UEventBusComponent>();
	bus->OnEvent.AddDynamic(Counters, &UCounterSet::YourFunction);
	
}


// Called every frame
void UChallengerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

