// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TowerofAngra.h"
#include "Blueprint/UserWidget.h"
#include "HResultUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API UHResultUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
};
