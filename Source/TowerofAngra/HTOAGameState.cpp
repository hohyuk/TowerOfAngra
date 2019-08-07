// Fill out your copyright notice in the Description page of Project Settings.

#include "HTOAGameState.h"



AHTOAGameState::AHTOAGameState()
{
	bGameCleared = false;
}

void AHTOAGameState::SetGameCleared()
{
	bGameCleared = true;
}

bool AHTOAGameState::IsGameCleared() const
{
	return bGameCleared;
}
