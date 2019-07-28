// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiGamePlayerController.h"
#include "Blueprint/UserWidget.h"


void AMultiGamePlayerController::BeginPlay()
{
	Super::BeginPlay();


	UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);

	UIWidgetInstance->AddToViewport();

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

