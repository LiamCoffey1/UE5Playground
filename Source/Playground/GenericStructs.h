// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenericStructs.generated.h"


class PLAYGROUND_API GenericStructs
{
public:
	GenericStructs();
	~GenericStructs();
};



	USTRUCT(BlueprintType)
		struct FItem
	{
		GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int itemID;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int quantity = 0;

		FItem() {
			itemID = -1;
			quantity = 0;
		}
		FItem(int itemID, int quantity) {
			this->itemID = itemID;
			this->quantity = quantity;
		}
	};

	USTRUCT(BlueprintType)
		struct FItemDefinition
	{
		GENERATED_USTRUCT_BODY()

			UPROPERTY(EditAnywhere, BlueprintReadWrite)
			int itemID;

			UPROPERTY(EditAnywhere, BlueprintReadWrite)
			FString itemName;

			UPROPERTY(EditAnywhere, BlueprintReadWrite)
			int maxStack;

			FItemDefinition() {
		}
			FItemDefinition(int itemID, FString itemName, int maxStack) {
			this->itemID = itemID;
			this->itemName = itemName;
			this->maxStack = maxStack;
		}
	};

	USTRUCT(BlueprintType)
		struct FRecipie
	{
		GENERATED_USTRUCT_BODY()

			UPROPERTY(EditAnywhere, BlueprintReadWrite)
			int recipeID;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			FString recipeName = "";

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			TArray<FItem> itemsRequired;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			TArray<FItem> itemsGiven;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			UTexture2D* recipeIcon;

		FRecipie() {
		}
		FRecipie(int recipeID, TArray<FItem> itemsRequired, TArray<FItem> itemsGiven) {
			this->recipeID = recipeID;
			this->itemsRequired = itemsRequired;
			this->itemsGiven = itemsGiven;
		}
	};

