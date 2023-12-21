// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueInteractionComponent.h"
#include "../InventorySystem/InventoryComponent.h"
#include <Playground/PlaygroundCharacter.h>

void UDialogueInteractionComponent::Interact(AActor* Initiator)
{
	LookAtActor(Initiator);
    APlaygroundCharacter* CustomPlayerCharacter = Cast<APlaygroundCharacter>(Initiator);
    if (CustomPlayerCharacter) {
        CustomPlayerCharacter->DialogueStart(dialogue);
    }
}
