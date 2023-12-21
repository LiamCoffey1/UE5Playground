// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueBase.generated.h"


USTRUCT(BlueprintType)
struct FDialogueOptions {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString OptionName;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bUseControllerOnSelect", EditConditionHides), BlueprintReadWrite)
		int SimpleChangeStageValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool ShowCondition;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMyDelegateType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMy2DelegateType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMyIntDelegateType, int32, IntD);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeSDelegateType, int32, NewStage);



UCLASS(Blueprintable)
class PLAYGROUND_API UDialogueStage : public UObject {
	GENERATED_BODY()

	public:


		UPROPERTY(BlueprintAssignable, Category = "MyCategory")
		FMyDelegateType OnContinue;

		UPROPERTY(BlueprintAssignable, Category = "MyCategory")
			FMyIntDelegateType OnOptionSelect;


		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString PersonTalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SpeechText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int SimpleChangeStageValue;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bHasOptions", EditConditionHides), BlueprintReadWrite)
		TArray<FDialogueOptions> Options;

};


UCLASS(Blueprintable)
class PLAYGROUND_API UDialogueBase : public UObject
{
	GENERATED_BODY()


public:
	FChangeSDelegateType OnChangeStage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDialogueStage* currentStage;

	UFUNCTION(BlueprintCallable)
	void ChangeDialogueStage(int newStage) {
		GetNextStage(newStage);
	}
	void UpdateCurrentStage(UDialogueStage* newStage) {
		currentStage = newStage;
	}
	UFUNCTION(BlueprintNativeEvent, Category = "MyCategory")
	int GetInitialStage();
	// Declare a function to be implemented in Blueprints
	UFUNCTION(BlueprintNativeEvent, Category = "MyCategory")
	void GetNextStage(int stage);
	
};
