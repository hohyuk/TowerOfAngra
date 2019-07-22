// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterSpawner.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
// Sets default values
AMonsterSpawner::AMonsterSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 10.f;
}

// Called when the game starts or when spawned
void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMonsterSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SpawnInWorld();
	GLog->Log(FString::Printf(TEXT("%d Spawned"),SpawnMonster.Num()));
}

void AMonsterSpawner::SpawnInWorld()
{
	FActorSpawnParameters Parameter;
	Parameter.Owner = this;
	Parameter.Instigator = this->Instigator;

	SpawnMonster.Add(GetWorld()->SpawnActor<ACharacter>(ACharacter::StaticClass(), GetActorLocation(), GetActorRotation(), Parameter));
}

