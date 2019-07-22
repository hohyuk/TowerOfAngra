// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SaveGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API USaveGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USaveGameInstance(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Save)
		int32 InterLevelPersistentValue;
	
};
