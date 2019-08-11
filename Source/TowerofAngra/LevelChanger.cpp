// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "TowerofAngraGameMode.h"
#include "LevelChanger.h"
#include "Engine.h"
// Sets default values
ALevelChanger::ALevelChanger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelChanger::OverlapBegins);
	RootComponent = OverlapVolume;
}

// Called when the game starts or when spawned
void ALevelChanger::BeginPlay()
{
	Super::BeginPlay();
	FVector SetLocation;
	SetLocation = FVector(0, 0, 150);

	GameMode = GetWorld()->GetAuthGameMode();
}

// Called every frame
void ALevelChanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALevelChanger::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	ACharacter* MyChar = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (OtherActor == MyChar && LoadLevelName != "")
	{
		FLatentActionInfo ActionInfo;

		UGameplayStatics::LoadStreamLevel(this, LoadLevelName, true, true, ActionInfo);
		++CurrentStageLevel;
		TOALOG(Warning, TEXT("CurrentStageLevel : %d"), CurrentStageLevel);

		if (CurrentStageLevel == 2)
		{
			auto GameMode = Cast<ATowerofAngraGameMode>(GetWorld()->GetAuthGameMode());

			GameMode->AddScore();
		}
	}

	if (OtherActor == MyChar && OpenLevelName != "")
	{
		GameMode->SetPlayerDefaults(MyChar);
		UGameplayStatics::OpenLevel(this, OpenLevelName);
	}
}

