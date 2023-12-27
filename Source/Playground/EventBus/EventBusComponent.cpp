// Fill out your copyright notice in the Description page of Project Settings.


#include "EventBusComponent.h"

// Sets default values for this component's properties
UEventBusComponent::UEventBusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UEventBusComponent::PublishEvent(const FString& Message) {
	OnEvent.Broadcast(Message);
}


// Called when the game starts
void UEventBusComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEventBusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

