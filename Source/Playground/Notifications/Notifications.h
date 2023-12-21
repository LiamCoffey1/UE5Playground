// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Notifications.generated.h"

UENUM()
enum class ENotificationType : uint8 {
	NONE,
	GAME_MESSAGE,
	QUEST_COMPLETE
};

class Notification {
public:
	ENotificationType Type = ENotificationType::NONE;
};

class GameMessageNotification : public Notification {
public:
	FString message;
	GameMessageNotification() {
		Type = ENotificationType::GAME_MESSAGE;
	}
	GameMessageNotification(FString message) : message(message) {
		Type = ENotificationType::GAME_MESSAGE;
	}
};

class NotificationFactory {
public:
	Notification* CreateGameMessageNotification(FString message) {
		return new GameMessageNotification(message);
	}
};

UINTERFACE(Blueprintable)
class PLAYGROUND_API UNotifications : public UInterface {
	GENERATED_BODY()
};

class PLAYGROUND_API INotifications {
	GENERATED_BODY()
public:
	virtual void SendNotification(Notification* notification);
};
