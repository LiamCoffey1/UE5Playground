// Fill out your copyright notice in the Description page of Project Settings.


#include "InterfaceInteractionComponent.h"
#include <Playground/PlaygroundCharacter.h>


void UInterfaceInteractionComponent::Interact(AActor* Initiator)
{
	LookAtActor(Initiator);
	APlaygroundCharacter* character = Cast<APlaygroundCharacter>(Initiator);
	if (character != nullptr) {
		character->DisplayWidget(InterfaceID);
	}
}

