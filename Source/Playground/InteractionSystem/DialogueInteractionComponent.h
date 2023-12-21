// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldInteractionComponent.h"
#include <Playground/Dialogue/DialogueBase.h>
#include "DialogueInteractionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAYGROUND_API UDialogueInteractionComponent : public UWorldInteractionComponent
{
	GENERATED_BODY()
	public:

		UPROPERTY(EditAnywhere, Category = "MyCategory")
		TSubclassOf<UDialogueBase> dialogue;

	virtual void Interact(AActor* Initiator) override;
};
