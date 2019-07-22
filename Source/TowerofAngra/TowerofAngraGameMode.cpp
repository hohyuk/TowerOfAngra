// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "TowerofAngraGameMode.h"
#include "HKayaCharacter.h"
#include "HAxeCharacter.h"
#include "CharacterPlayerController.h"
#include "Blueprint/UserWidget.h"

void ATowerofAngraGameMode::BeginPlay()
{
	//As Soon as the player enter the game apply huid to the screen.
}

ATowerofAngraGameMode::ATowerofAngraGameMode()
{
	// set default character controller class by c++
	PlayerControllerClass = ACharacterPlayerController::StaticClass();
	
	// set set default character class by c++

	DefaultPawnClass = AHKayaCharacter::StaticClass();
	//DefaultPawnClass = AHAxeCharacter::StaticClass();
}

void ATowerofAngraGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATowerofAngraGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
