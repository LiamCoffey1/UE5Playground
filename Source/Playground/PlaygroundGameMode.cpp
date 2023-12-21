// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaygroundGameMode.h"
#include "PlaygroundCharacter.h"
#include "GenericStructs.h"
#include "UObject/ConstructorHelpers.h"
#include "InventorySystem/InventoryComponent.h"

APlaygroundGameMode::APlaygroundGameMode() {
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void APlaygroundGameMode::PerformRecipie(int recipeId, AActor* initiator, int amountOfTimes) {
	APlaygroundCharacter* CustomPlayerCharacter = Cast<APlaygroundCharacter>(initiator);
	if (CustomPlayerCharacter) {
		CustomPlayerCharacter->StartTask(new ProcessTask(initiator, recipies[recipeId], amountOfTimes, ProcessMontage));
	}
}