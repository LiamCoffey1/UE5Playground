// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "Blueprint/UserWidget.h"
#include "UObject/NoExportTypes.h"
#include "InteractionSystem/HarvestingInteractionComponent.h"
#include "QuestSystem/QuestComponent.h"
#include <EventBus/EventBusComponent.h>
#include "AbilitySystem/PlaygroundAbilitySystemComponent.h"
#include "Dialogue/DialogueBase.h"
#include <Challenges/ChallengerComponent.h>
#include "Notifications/Notifications.h"
#include "PlaygroundCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

class ITaskInterface {
	bool isStopped = false;
public:
	virtual void Start() = 0;
	virtual void Execute() = 0;
	virtual void Stop() {
		isStopped = true;
	};
	bool IsStopped() {
		return isStopped;
	};
	virtual ~ITaskInterface() {}

};


class ProcessTask : public ITaskInterface {
public:
	AActor* Owner;
	FRecipie currentRecipie;
	int amountOfTimes;
	UAnimMontage* ProcessMontage;

	ProcessTask() {}
	ProcessTask(AActor* Owner, FRecipie currentRecipie, int amountOfTimes, UAnimMontage* ProcessMontage) :
		Owner(Owner),
		currentRecipie(currentRecipie),
		amountOfTimes(amountOfTimes),
		ProcessMontage(ProcessMontage) {
	}

	void Start() override {
		UAnimInstance* MyAnimInstance = Owner->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance();
		if (MyAnimInstance) {
			MyAnimInstance->Montage_Play(ProcessMontage, 1.0f);
		}
		CommandQueue* commandQueue = GetCommandQueue();
		commandQueue->Clear();
		if (!TryStorageOperations(commandQueue)) {
			Stop();
		}
	}

	CommandQueue* GetCommandQueue() {
		UInventoryComponent* inventory = Owner->FindComponentByClass<UInventoryComponent>();
		return  new CommandQueue(inventory->ItemsStorageContainer->GetStorages());
	}

	bool TryStorageOperations(CommandQueue* commandQueue) {
		return commandQueue
			->addCommand(new RemoveItemsTask(currentRecipie.itemsRequired, FStorageTypes::INVENTORY))
			->addCommand(new AddItemsTask(currentRecipie.itemsGiven, FStorageTypes::INVENTORY))
			->ExecuteAllCommands()
			.IsValid();
	}

	void Execute() override {
		if (amountOfTimes <= 0) {
			Stop();
			return;
		}
		if (Owner != nullptr) {
			UInventoryComponent* inventory = Owner->FindComponentByClass<UInventoryComponent>();
			CommandQueue* commandQueue = GetCommandQueue();
			bool valid = TryStorageOperations(commandQueue);
			if (!valid) {
				Stop();
			} else {
				inventory->ItemsStorageContainer->CommitCommandQueueState(commandQueue);
				amountOfTimes--;
				if (!TryStorageOperations(commandQueue)) {
					Stop();
				} else {
					commandQueue->Clear();
				}
			}
		}
	}

	void Stop() override {
		ITaskInterface::Stop();
		UAnimInstance* MyAnimInstance = Owner->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance();
		if (MyAnimInstance) {
			MyAnimInstance->StopAllMontages(true);
		}
	}
};

class HarvestTask : public ITaskInterface {
public:
	AActor* Owner;
	UHarvestingInteractionComponent* harvestEvent;
	UEventBusComponent* InitiatorEventBus;

	HarvestTask() {}
	HarvestTask(AActor* Owner, UHarvestingInteractionComponent* harvestEvent) : Owner(Owner), harvestEvent(harvestEvent) {
		InitiatorEventBus = Owner->GetComponentByClass<UEventBusComponent>();
	}

	void Start() override {
		UAnimInstance* MyAnimInstance = Owner->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance();
		if (MyAnimInstance) {
			MyAnimInstance->Montage_Play(harvestEvent->MontageToPlay, 1.0f);
		}
	}

	void Execute() override {
		UInventoryComponent* inventory = Owner->GetComponentByClass<UInventoryComponent>();
		if (inventory && harvestEvent && harvestEvent->GetCurrentCapacity() > 0 && !IsStopped()) {
			bool validAddItem = inventory->AddToStorage(harvestEvent->itemsGiven, FStorageTypes::INVENTORY);
			if (!validAddItem) {
				Stop();
				return;
			}
			harvestEvent->UpdateCapacity(harvestEvent->GetCurrentCapacity() - 1);
			if (InitiatorEventBus) {
				InitiatorEventBus->PublishEvent("Harvest");
			}
			if (harvestEvent->GetCurrentCapacity() <= 0) {
				Stop();
				return;
			}
		} else {
			Stop();
		}
	}

	void Stop() override {
		ITaskInterface::Stop();
		UAnimInstance* MyAnimInstance = Owner->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance();
		if (MyAnimInstance) {
			MyAnimInstance->StopAllMontages(true);
		}
	}
};


UENUM()
enum class EMovementStyle : uint8 {
	REGULAR_MOVEMENT,
	STRAFING_MOVEMENT
};

UENUM()
enum class EDirection: uint8 {
	FORWARD,
	BACKWARD,
	LEFTWARD,
	RIGHTWARD,
	FORWARD_DIAGONAL,
	BACKWARD_DIAGONAL
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameMessageDelegateType, FString, Message);

UCLASS(config = Game)
class APlaygroundCharacter : public ACharacter, public IAbilitySystemInterface, public INotifications, public IEventPublisherInterface {
	GENERATED_BODY()

		UPROPERTY()
		UAbilitySystemComponent* AbilitySystemComponent;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UEventBusComponent* EventBusComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UChallengerComponent* Challenges;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
		TArray<TSubclassOf<UUserWidget>> Interfaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
		TArray<TSubclassOf<UUserWidget>> NotificationWidgetClassess;

public:

	UPROPERTY(BlueprintAssignable, Category = "MyCategory")
	FGameMessageDelegateType OnGameMessage;

	UEventBus* EventBus;
	void Subscribe(TScriptInterface<IEventConsumerInterface> Consumer) override {
		EventBus->Subscribe(Consumer);
	}

	virtual void PublishEvent(const FString& Message) override {
		EventBus->PublishEvent(Message);
	}

	virtual void SendNotification(Notification* notification) override;

	UPROPERTY(BlueprintAssignable, Category = "CustomEvents")
	FOnInventoryChanged OnDialogueUpdated;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	UDialogueBase* dialogue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	UDialogueStage* currentStage;


	UFUNCTION()
	void ChangeStage(int newStage) {
		dialogue->GetNextStage(newStage);
		OnDialogueUpdated.Broadcast();
	}
	void DialogueStart(TSubclassOf<UDialogueBase> base) {
		dialogue = NewObject<UDialogueBase>(this, base);
		if (dialogue) {
			dialogue->OnChangeStage.AddDynamic(this, &APlaygroundCharacter::ChangeStage);
			dialogue->GetNextStage(dialogue->GetInitialStage());
			OnDialogueUpdated.Broadcast();
		}
	}


	UFUNCTION(BlueprintCallable, Category = "Targetting")
		void DialogueContinue();
	UFUNCTION(BlueprintCallable, Category = "Targetting")
		void DialogueClose();


	UFUNCTION(BlueprintCallable, Category = "Targetting")
		void DialogueSelectOption(int option);

	void UpdateCurrentDialogue(int stage) {
		if (dialogue->currentStage) {
			if (dialogue->currentStage->OnContinue.IsBound()) {
				dialogue->currentStage->OnContinue.Broadcast();
			} else {
				dialogue->GetNextStage(dialogue->currentStage->SimpleChangeStageValue);
			}
			OnDialogueUpdated.Broadcast();
		}
	}

	FTimerHandle CurrentTaskTimerHandle;
	ITaskInterface* CurrentTask;

	void ExecuteTask() {
		if (CurrentTask && !CurrentTask->IsStopped()) {
			CurrentTask->Execute();
		} else {
			StopCurrentTask();
		}
	}
	void StartTask(ITaskInterface* task) {
		CurrentTask = task;
		if (CurrentTask && GetWorld() && GetWorld()->GetGameInstance()) {
			FTimerDelegate TimerCallback;
			TimerCallback.BindUObject(this, &APlaygroundCharacter::ExecuteTask);
			CurrentTask->Start();
			GetWorldTimerManager().SetTimer(CurrentTaskTimerHandle, TimerCallback, 3, true);
		}
	}
	void StopCurrentTask() {
		if (IsValid(this) && CurrentTaskTimerHandle.IsValid() && CurrentTask) {
			GetWorldTimerManager().ClearTimer(CurrentTaskTimerHandle);
			CurrentTask->Stop();
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Targetting")
	void LookForTarget() {
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController) {
			FVector StartLocation;
			FRotator PlayerViewpoint;
			PlayerController->GetPlayerViewPoint(StartLocation, PlayerViewpoint);
			FVector EndLocation = StartLocation + PlayerViewpoint.Vector() * 1000.0f;
			FHitResult HitResult;
			FCollisionQueryParams TraceParams(FName(TEXT("LineTrace")), true, this);
			if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams)) {
				AActor* HitActor = HitResult.GetActor();
				if (HitActor) {
				}
			}
			if (GEngine) {
				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 1, 0, 1);
			}
		}
	}

	APlaygroundCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
		EMovementStyle MovementStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
		EDirection MovementDirection;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SwitchMovementStyle(EMovementStyle newStyle);

	void PerformRecipeCommand(const TArray<FString>& Args);

	void AddItemCommand(const TArray<FString>& Args);


protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
		void DisplayWidget(int WidgetID);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
