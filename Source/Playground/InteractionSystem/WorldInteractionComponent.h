// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "WorldInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLAYGROUND_API UWorldInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWorldInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "Interaction"))
	FString InteractionDisplayName = TEXT("");
	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
	virtual void Interact(AActor* Initiator);
	void LookAtActor(AActor* Initiator) {
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Initiator->GetActorLocation(), GetOwner()->GetActorLocation());
		Initiator->SetActorRotation(LookAtRotation);
	}

		
};
