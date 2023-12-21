// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldInteractionComponent.h"
#include "InterfaceInteractionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAYGROUND_API UInterfaceInteractionComponent : public UWorldInteractionComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	int InterfaceID = 1;
	virtual void Interact(AActor* Initiator) override;
};
