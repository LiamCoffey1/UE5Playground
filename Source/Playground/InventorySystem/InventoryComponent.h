// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../GenericStructs.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UENUM()
enum class FStorageTypes : uint8 {
	INVENTORY,
	BANK
};

class ItemContainer {
public:
	int maxCapacity = 30;
	TArray<FItem> Items;

	TArray<FItem>& GetItems() {
		return Items;
	}

	void UpdateItems(const TArray<FItem>& newItems) {
		Items = newItems;
	}

	ItemContainer() {
		Items.Init(FItem(-1, -1), maxCapacity);
	}
	ItemContainer(int maxCapacity) {
		this->maxCapacity = maxCapacity;
	}
};

class ItemContainersStorageState {
private:
	TMap<FStorageTypes, ItemContainer> state_;

public:
	FString errorMessage = "";
	ItemContainersStorageState() {}
	ItemContainersStorageState(TMap<FStorageTypes, ItemContainer> state) {
		state_ = state;
	}
	ItemContainersStorageState(TMap<FStorageTypes, ItemContainer> state, FString reason) {
		state_ = state;
		errorMessage = reason;
	}

	TMap<FStorageTypes, ItemContainer>& GetState()  {
		return state_;
	}

	void SetErrorMessage(FString stringInput) {
		errorMessage = stringInput;
	}

	bool IsValid() {
		return errorMessage.IsEmpty();
	}
};

class Command {
private:
	ItemContainersStorageState state;
public:
	Command() {
		state = ItemContainersStorageState();
	}
	virtual ~Command() {}

	ItemContainersStorageState GetState() {
		return state;
	}

	void SetState(const ItemContainersStorageState& currentState) {
		state = currentState;
	}

	virtual void Execute(ItemContainersStorageState& currentState) {}

};

class AddItemsTask : public Command {
public:
	AddItemsTask(const TArray<FItem>& items, FStorageTypes storageType)
		: itemsToAdd(items), storageType(storageType) {
	}

	void Execute(ItemContainersStorageState& currentState) override {
		int quantityToAdd;
		for (const FItem& item : itemsToAdd) {
			quantityToAdd = item.quantity;
			AddItemsToSameIDSlotsAndGetRemaining(item.itemID, quantityToAdd, currentState.GetState()[storageType].GetItems());
			AssignToRemainingFreeSpots(item.itemID, quantityToAdd, currentState.GetState()[storageType].GetItems());
			currentState.SetErrorMessage(quantityToAdd == 0 ? "" : "Not enough inventory space");
			SetState(currentState);
		}
	}

private:
	TArray<FItem> itemsToAdd;
	FStorageTypes storageType;

	void AddItemsToSameIDSlotsAndGetRemaining(int itemId, int& quantityToAdd, TArray<FItem>& itemsCopy) {
		int maxStack = storageType == FStorageTypes::BANK ? 2000000 : 1;
		int spaceAvailable, amountToAddToSlot;
		for (int i = 0; i < itemsCopy.Num() && quantityToAdd > 0; ++i) {
			if (itemsCopy[i].itemID == itemId && itemsCopy[i].quantity < maxStack) {
				spaceAvailable = maxStack - itemsCopy[i].quantity;
				amountToAddToSlot = FMath::Min(quantityToAdd, spaceAvailable);

				itemsCopy[i].quantity += amountToAddToSlot;
				quantityToAdd -= amountToAddToSlot;
			}
		}
	}

	void AssignToRemainingFreeSpots(int itemId, int& quantityToAdd, TArray<FItem>& itemsCopy) {
		int maxStack = storageType == FStorageTypes::BANK ? 2000000 : 1;
		int amountToAssignToSlot;
		for (int i = 0; i < itemsCopy.Num() && quantityToAdd > 0; ++i) {
			if (itemsCopy[i].itemID == -1) {
				amountToAssignToSlot = FMath::Min(quantityToAdd, maxStack);
				itemsCopy[i] = FItem(itemId, amountToAssignToSlot);
				quantityToAdd -= amountToAssignToSlot;
			}
		}
	}
};

class AddOrRemoveSlotQuantityTask : public Command {
public:
	int* amountToModify;
	FStorageTypes storageType;
	int slot;
	int operation;

	AddOrRemoveSlotQuantityTask(int& amountToModify, int slot, int operation, FStorageTypes storageType)
		: amountToModify(&amountToModify), storageType(storageType), slot(slot), operation(operation) {
	}

	void Execute(ItemContainersStorageState& currentState)  override {
		TArray<FItem>& itemsCopy = currentState.GetState()[storageType].GetItems();
		int quantityInSlot = itemsCopy[slot].quantity;
		int maxStack = storageType == FStorageTypes::BANK ? 2000000 : 1;
		int freeSpace = maxStack - quantityInSlot;
		if (operation == 0) {
		} else {
			if (*amountToModify >= quantityInSlot) {
				*amountToModify -= quantityInSlot;
				itemsCopy[slot] = FItem(-1, 0);
			} else {
				itemsCopy[slot] = FItem(itemsCopy[slot].itemID, itemsCopy[slot].quantity - *amountToModify);
				*amountToModify = 0;
			}
		}
		SetState(currentState);
	}
};


class RemoveItemsTask : public Command {
public:
	TArray<FItem> itemsToRemove;
	FStorageTypes storageType;

	RemoveItemsTask(const TArray<FItem>& items, FStorageTypes storageType)
		: itemsToRemove(items), storageType(storageType) {
	}

	void Execute(ItemContainersStorageState& currentState)  override {
		TArray<FItem>& itemsCopy = currentState.GetState()[storageType].GetItems();
		bool canRemove = true;
		int quantityToRemove;
		int quantityInSlot;
		for (const FItem& item : itemsToRemove) {
			quantityToRemove = item.quantity;
			for (int i = 0; i < itemsCopy.Num() && quantityToRemove > 0; ++i) {
				if (itemsCopy[i].itemID == item.itemID) {
					quantityInSlot = itemsCopy[i].quantity;
					if (quantityInSlot <= quantityToRemove) {
						quantityToRemove -= quantityInSlot;
						itemsCopy[i] = FItem(-1, -1);
					} else {
						itemsCopy[i] = FItem(item.itemID, quantityInSlot - quantityToRemove);
						quantityToRemove = 0;
					}
				}
			}
			if (quantityToRemove != 0) {
				canRemove = false;
				break;
			}
		}
		currentState.SetErrorMessage((canRemove ? "" : "Missing items"));
		SetState(currentState);
	}
};


class ClearStorageTask : public Command {
public:
	FStorageTypes storageType;

	ClearStorageTask(FStorageTypes storageType)
		: storageType(storageType) {
	}

	void Execute(ItemContainersStorageState& currentState)  override {
		TArray<FItem>& itemsCopy = currentState.GetState()[storageType].GetItems();
		for (FItem& item : itemsCopy) {
			item.itemID = -1;
			item.quantity = 0;
		}
		SetState(currentState);
	}
};

class CommandQueue {
private:
	TArray<Command*> commands;
	ItemContainersStorageState currentState;
public:
	CommandQueue() {}
	CommandQueue(TMap<FStorageTypes, ItemContainer> initialState) {
		currentState = ItemContainersStorageState(initialState);
	}

	ItemContainersStorageState ExecuteAllCommands() {
		for (Command* command : commands) {
			command->Execute(currentState);
			currentState = command->GetState();
			if (!currentState.IsValid()) {
				break;
			}
		}
		return currentState;
	}

	CommandQueue* addCommand(Command* command...) {
		for (Command* c : { command }) {
			commands.Add(command);
		}
		return this;
	}

	ItemContainersStorageState GetFinalState() {
		return currentState;
	}

	void Clear() {
		for (Command* command : commands) {
			delete command;
		}
		commands.Empty();
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAYGROUND_API UItemContainersStorage : public UObject {

	GENERATED_BODY()

private:
	TMap<FStorageTypes, ItemContainer> itemStorage;

public:

	UPROPERTY(BlueprintAssignable, Category = "CustomEvents")
	FOnInventoryChanged OnItemChanged;

	UItemContainersStorage() {
		itemStorage = TMap<FStorageTypes, ItemContainer>();
	}

	void InitializeStorage(FStorageTypes storageType) {
		itemStorage.Add(storageType, ItemContainer());
	}

	TArray<FItem> GetItemsFromStorage(FStorageTypes storageType) {
		return itemStorage[storageType].GetItems();
	}

	TMap<FStorageTypes, ItemContainer> GetStorages() {
		return itemStorage;
	}

	void CommitCommandQueueState(CommandQueue* commandQueue) {
		itemStorage = commandQueue->GetFinalState().GetState();
		commandQueue->Clear();
		OnItemChanged.Broadcast();
	}

};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PLAYGROUND_API UInventoryComponent : public UActorComponent {

	GENERATED_BODY()

public:

	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
		TMap<int, int> GetItemCounts(FStorageTypes types);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
		TArray<FStorageTypes> ItemStorages;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
		UItemContainersStorage* ItemsStorageContainer = NewObject<UItemContainersStorage>();

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
	TArray<FItem> GetItems(FStorageTypes StorageType);

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
	bool AddToStorage(TArray<FItem> itemsToAdd, FStorageTypes StorageType);

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
	bool RemoveFromStorage(TArray<FItem> itemsToRemove, FStorageTypes StorageType);

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
	bool MoveItemToOtherStorage(TArray<FItem> itemsToMove, FStorageTypes SourceStorageType, FStorageTypes DestinationStorageType);

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
	bool MoveItemToOtherStorageFromSlot(FItem itemsToMove, int slot, FStorageTypes SourceStorageType, FStorageTypes DestinationStorageType);

	bool ModifyStorageWithCommands(bool overwriteIfValid, Command* commands...);

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
	bool ClearStorage(FStorageTypes StorageType);

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
		bool TryRemoveItems(TArray<FItem> itemsToRemove);

	bool TryRemoveItems(TArray<FItem> itemsToRemove, bool overwriteIfSuccessful);

	UFUNCTION(BlueprintCallable, Category = "InteractionSystem")
	bool TryAddItems(TArray<FItem> items);

	bool TryAddItems(TArray<FItem> itemsToAdd, bool overwriteIfSuccessful);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
