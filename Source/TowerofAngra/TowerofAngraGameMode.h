// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TowerofAngraGameMode.generated.h"

UCLASS(minimalapi)
class ATowerofAngraGameMode : public AGameModeBase
{
	GENERATED_BODY()

	ATowerofAngraGameMode();
public:
	
	//FName LoadFirstLevel;

public:
	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
};



