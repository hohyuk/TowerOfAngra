// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "GameFramework/GameStateBase.h"
#include "HTOAGameState.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API AHTOAGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AHTOAGameState();
	
public:
	void SetGameCleared();
	bool IsGameCleared() const;

private:
	UPROPERTY(Transient)
	bool bGameCleared;
};