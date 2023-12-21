// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GenericStructs.h"
#include "InteractionSystem/HarvestingInteractionComponent.h"
#include "PlaygroundGameMode.generated.h"

UCLASS(minimalapi)
class APlaygroundGameMode : public AGameModeBase {
public:
	GENERATED_BODY()

		APlaygroundGameMode();

	FTimerHandle TimerHandle;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* ProcessMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
		TArray<FRecipie> recipies;

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
		void PerformRecipie(int recipeId, AActor* initiator, int amountOfTimes);
};



