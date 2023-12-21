// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaygroundCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InventorySystem/InventoryComponent.h"
#include "PlaygroundGameMode.h"
#include "Notifications/Notifications.h"


UAbilitySystemComponent* APlaygroundCharacter::GetAbilitySystemComponent() const {
	return nullptr;
}

void APlaygroundCharacter::DialogueContinue() {
	if (dialogue->currentStage) {
		if (dialogue->currentStage->OnContinue.IsBound()) {
			dialogue->currentStage->OnContinue.Broadcast();
		} else {
			int nextStage = dialogue->currentStage->SimpleChangeStageValue;
			if (nextStage == -1) {
				DialogueClose();
			} else {
				dialogue->GetNextStage(dialogue->currentStage->SimpleChangeStageValue);
			}
		}
		OnDialogueUpdated.Broadcast();
	}
}

void APlaygroundCharacter::DialogueClose() {
	dialogue = nullptr;
	OnDialogueUpdated.Broadcast();
}

void APlaygroundCharacter::DialogueSelectOption(int option) {
	if (dialogue->currentStage) {
		if (dialogue->currentStage->OnOptionSelect.IsBound()) {
			dialogue->currentStage->OnOptionSelect.Broadcast(option);
		} else {
			int nextStage = dialogue->currentStage->Options[option].SimpleChangeStageValue;
			if (nextStage == -1) {
				DialogueClose();
			} else {
				dialogue->GetNextStage(dialogue->currentStage->Options[option].SimpleChangeStageValue);
			}
		}
		OnDialogueUpdated.Broadcast();
	}
}

APlaygroundCharacter::APlaygroundCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	FConsoleCommandWithArgsDelegate DumpMaterial;
	FConsoleCommandWithArgsDelegate RecipeCommandDelegate;

	DumpMaterial.BindLambda([this](const TArray<FString>& Args) {
		this->AddItemCommand(Args);
	});
	RecipeCommandDelegate.BindLambda([this](const TArray<FString>& Args) {
		this->PerformRecipeCommand(Args);
	});
	//Register the commands
	IConsoleManager::Get().RegisterConsoleCommand(TEXT("AddItem"), TEXT("Adds an inventory item to player"), DumpMaterial);
	IConsoleManager::Get().RegisterConsoleCommand(TEXT("PerformRecipe"), TEXT("Peforms a recipe"), RecipeCommandDelegate);

	AbilitySystemComponent = CreateDefaultSubobject<UPlaygroundAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void APlaygroundCharacter::SwitchMovementStyle(EMovementStyle newStyle) {
	MovementStyle = newStyle;
	if (MovementStyle == EMovementStyle::STRAFING_MOVEMENT) {
		GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0F, 0.0f); // ...at this rotation rate
		bUseControllerRotationYaw = true;
	} else {
		GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0F, 0.0f); // ...at this rotation rate
		bUseControllerRotationYaw = false;
	}
}

void APlaygroundCharacter::PerformRecipeCommand(const TArray<FString>& Args) {
	if (Args.Num() == 1)
	{
		FString recipeId = Args[0];

		UWorld* World = GetWorld();

		if (World)
		{
			AGameModeBase* GameMode = World->GetAuthGameMode();
			APlaygroundGameMode* CustomGameMode = Cast<APlaygroundGameMode>(GameMode);
			if (CustomGameMode)
			{
				CustomGameMode->PerformRecipie(FCString::Atoi(*recipeId), this, 1);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("PerformRecipe executed with recipeId: %s."), *recipeId);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Insufficient arguments. Usage: PerformRecipe {recipeId}"));
	}
}

void APlaygroundCharacter::AddItemCommand(const TArray<FString>& Args) {
	if (Args.Num() == 2)
	{
		FString itemId = Args[0];
		FString amount = Args[1];

		UInventoryComponent* inventory = this->FindComponentByClass<UInventoryComponent>();
		if (inventory != nullptr) {
			inventory->TryAddItems(TArray<FItem>{ FItem(FCString::Atoi(*itemId), FCString::Atoi(*amount))});
		}
		UE_LOG(LogTemp, Warning, TEXT("AddItem executed with itemId: %s. amount: %s"), *itemId, *amount);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Insufficient arguments. Usage: AddItem {itemId} {amount}"));
	}
}

void APlaygroundCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
}

void APlaygroundCharacter::SendNotification(Notification* notification) {
	if (notification->Type == ENotificationType::GAME_MESSAGE) {
		GameMessageNotification* game_message = static_cast<GameMessageNotification*>(notification);
		OnGameMessage.Broadcast(game_message->message);
	}
}

void APlaygroundCharacter::DisplayWidget(int WidgetID)
{
	if (WidgetID >= 0 && WidgetID < Interfaces.Num())
	{
		UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), Interfaces[WidgetID]);
		if (WidgetInstance)
		{
			WidgetInstance->AddToViewport();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlaygroundCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlaygroundCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlaygroundCharacter::Look);
	}
	else
	{
	}
}

void APlaygroundCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (MovementVector.Y > 0) {
		MovementDirection = (MovementVector.X - MovementVector.Y) == -1 ? EDirection::FORWARD : EDirection::FORWARD_DIAGONAL;
	} else if (MovementVector.Y < 0) {
		MovementDirection = (MovementVector.X - MovementVector.Y) == 1 ? EDirection::BACKWARD : EDirection::BACKWARD_DIAGONAL;
	} else if (MovementVector.X > 0) {
		MovementDirection = EDirection::LEFTWARD;
	} else if (MovementVector.X < 0) {
		MovementDirection = EDirection::RIGHTWARD;
	}
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
			float moveX = MovementVector.X;
			float moveY = MovementVector.Y;
			UE_LOG(LogTemp, Warning, TEXT("MoveX: %f, MoveY: %f"), moveX, moveY);
		StopCurrentTask();
	}
}

void APlaygroundCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

