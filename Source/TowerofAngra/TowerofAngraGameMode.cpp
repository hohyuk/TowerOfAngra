// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "TowerofAngraGameMode.h"
#include "HKayaCharacter.h"
#include "HAxeCharacter.h"
#include "HWarriorCharacter.h"
#include "CharacterPlayerController.h"
#include "HSingleGamePlayerController.h"
ATowerofAngraGameMode::ATowerofAngraGameMode()
{
	// set set default character class by c++
	if (PlayerType  == EPlayerType::WARRIOR)
	{
		DefaultPawnClass = AHWarriorCharacter::StaticClass();
	}
	else if(PlayerType == EPlayerType::AXE)
	{
		DefaultPawnClass = AHAxeCharacter::StaticClass();
	}

	// set default character controller class by c++
	if (CurrentGameMode == EGameMode::MULTI_GAME)
	{
		PlayerControllerClass = ACharacterPlayerController::StaticClass();
	}
	else
	{
		PlayerControllerClass = AHSingleGamePlayerController::StaticClass();
	}
}

void ATowerofAngraGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ATowerofAngraGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
