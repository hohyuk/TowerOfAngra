// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "TowerofAngraGameMode.h"
#include "HKayaCharacter.h"
#include "HAxeCharacter.h"
#include "HWarriorCharacter.h"
#include "CharacterPlayerController.h"
#include "HSingleGamePlayerController.h"
#include "HTOAGameState.h"

ATowerofAngraGameMode::ATowerofAngraGameMode()
{
	// ¿¬½À¿ë
	if (PlayerType == EPlayerType::NONE && CurrentGameMode == EGameMode::NONE)
	{
		DefaultPawnClass = AHWarriorCharacter::StaticClass();
		PlayerControllerClass = AHSingleGamePlayerController::StaticClass();
	}

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
	else if (CurrentGameMode == EGameMode::SINGLE_GAME)
	{
		PlayerControllerClass = AHSingleGamePlayerController::StaticClass();
	}

	GameStateClass = AHTOAGameState::StaticClass();
	TOALOG(Warning, TEXT("CurrentGameMode : %d"), int(CurrentGameMode));
}

void ATowerofAngraGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TOAGameState = Cast<AHTOAGameState>(GameState);
}

void ATowerofAngraGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
