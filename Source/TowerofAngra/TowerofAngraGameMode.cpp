// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "TowerofAngraGameMode.h"
#include "HKayaCharacter.h"
#include "HAxeCharacter.h"
#include "CharacterPlayerController.h"
#include "Blueprint/UserWidget.h"

ATowerofAngraGameMode::ATowerofAngraGameMode()
{
	DefaultPawnClass = AHKayaCharacter::StaticClass();
	//DefaultPawnClass = AHAxeCharacter::StaticClass();

	// set default character controller class by c++
	PlayerControllerClass = ACharacterPlayerController::StaticClass();
	
	// set set default character class by c++

	
}

void ATowerofAngraGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATowerofAngraGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
