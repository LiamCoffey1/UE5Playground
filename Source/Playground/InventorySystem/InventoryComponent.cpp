// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "../GenericStructs.h"

UInventoryComponent::UInventoryComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

TArray<FItem>  UInventoryComponent::GetItems(FStorageTypes StorageType) {
	return ItemsStorageContainer->GetItemsFromStorage(StorageType);
}

TMap<int, int> UInventoryComponent::GetItemCounts(FStorageTypes type) {
	TMap<int, int> itemCounts;
	for (const FItem& item : ItemsStorageContainer->GetItemsFromStorage(type)) {
		if (item.itemID != -1) {
			int& count = itemCounts.FindOrAdd(item.itemID, 0);
			count += item.quantity;
		}
	}
	return itemCounts;
}

bool UInventoryComponent::TryAddItems(TArray<FItem> items) {
	return TryAddItems(items, true);
}

bool UInventoryComponent::ClearStorage(FStorageTypes StorageType) {
	return ModifyStorageWithCommands(true,
		new ClearStorageTask(StorageType)
	);
}

bool UInventoryComponent::TryAddItems(TArray<FItem> itemsToAdd, bool overwriteIfSuccessful) {
	return ModifyStorageWithCommands(overwriteIfSuccessful,
		new AddItemsTask(itemsToAdd, FStorageTypes::INVENTORY)
	);
}

bool UInventoryComponent::TryRemoveItems(TArray<FItem> items) {
	return TryRemoveItems(items, true);
}

bool UInventoryComponent::TryRemoveItems(TArray<FItem> itemsToRemove, bool overwriteIfSuccessful) {
	return ModifyStorageWithCommands(overwriteIfSuccessful,
		new RemoveItemsTask(itemsToRemove, FStorageTypes::INVENTORY)
	);
}

bool UInventoryComponent::AddToStorage(TArray<FItem> itemsToAdd, FStorageTypes StorageType) {
	return ModifyStorageWithCommands(true ,
		new AddItemsTask(itemsToAdd, StorageType)
	);
}

bool UInventoryComponent::RemoveFromStorage(TArray<FItem> itemsToRemove, FStorageTypes StorageType) {
	return ModifyStorageWithCommands(true,
		new RemoveItemsTask(itemsToRemove, StorageType)
	);
}

bool UInventoryComponent::MoveItemToOtherStorageFromSlot(FItem itemsToMove, int slot, FStorageTypes SourceStorageType, FStorageTypes DestinationStorageType) {
	CommandQueue* commandQueue = new CommandQueue(ItemsStorageContainer->GetStorages());
	int quantityToRemove = itemsToMove.quantity;
	bool valid = ModifyStorageWithCommands(false, new AddOrRemoveSlotQuantityTask(quantityToRemove, slot, 1, SourceStorageType));
	commandQueue->Clear();
	valid &= ModifyStorageWithCommands(false,
			new RemoveItemsTask(TArray<FItem> { FItem(itemsToMove.itemID, quantityToRemove) }, SourceStorageType),
			new AddItemsTask(TArray<FItem> { itemsToMove }, DestinationStorageType)
	);
	if (valid) {
		ItemsStorageContainer->CommitCommandQueueState(commandQueue);
	}
	return valid;
}

bool UInventoryComponent::MoveItemToOtherStorage(TArray<FItem> itemsToMove, FStorageTypes SourceStorageType, FStorageTypes DestinationStorageType) {
	return ModifyStorageWithCommands(true,
		new AddItemsTask(itemsToMove, DestinationStorageType),
		new RemoveItemsTask(itemsToMove, SourceStorageType)
	);
}

bool UInventoryComponent::ModifyStorageWithCommands(bool overwriteIfValid, Command* commands...) {
	CommandQueue* commandQueue = new CommandQueue(ItemsStorageContainer->GetStorages());
	commandQueue->addCommand(commands);
	bool valid = commandQueue
		->ExecuteAllCommands()
		.IsValid();
	if (valid && overwriteIfValid) {
		ItemsStorageContainer->CommitCommandQueueState(commandQueue);
	}
	return valid;
}

void UInventoryComponent::BeginPlay() {
	Super::BeginPlay();
	ItemsStorageContainer = NewObject<UItemContainersStorage>();
	for (FStorageTypes type : ItemStorages) {
		ItemsStorageContainer->InitializeStorage(type);
	}
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
