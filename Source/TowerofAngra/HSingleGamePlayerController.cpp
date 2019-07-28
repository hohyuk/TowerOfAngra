// Fill out your copyright notice in the Description page of Project Settings.

#include "HSingleGamePlayerController.h"


AHSingleGamePlayerController::AHSingleGamePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHSingleGamePlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AHSingleGamePlayerController::Possess(APawn * aPawn)
{
	Super::Possess(aPawn);
}

void AHSingleGamePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AHSingleGamePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHSingleGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AHSingleGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
