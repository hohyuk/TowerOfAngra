// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ConstructorHelpers.h"
#include "towerofangracharacter.h"
#include "Engine.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	OpenDoor = false;

	//set Staticmesh 
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorR(TEXT("/Game/MultistoryDungeons/Meshes/FP_Update/Door_Bronze_01"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Frame(TEXT("StaticMesh'/Game/MultistoryDungeons/Meshes/FP_Door_Bronze_01.FP_Door_Bronze_01'"));
	
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrame->SetupAttachment(RootComponent);
	DoorFrame->SetStaticMesh(Frame.Object);
	DoorFrame->SetWorldRotation(FRotator(0, 0, 0));
	DoorFrame->SetRelativeLocation(FVector(-10, 10, -270));

	DoorRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorRight"));
	DoorRight->SetupAttachment(RootComponent);
	DoorRight->SetStaticMesh(DoorR.Object);
	DoorRight->SetWorldRotation(FRotator(0, 0, 0));
	DoorRight->SetRelativeLocation(FVector(-140, 50, 0));

	
	DoorLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorLeft"));
	DoorLeft->SetupAttachment(RootComponent);
	DoorLeft->SetStaticMesh(DoorR.Object);
	DoorLeft->SetRelativeLocation(FVector(120, 0, 0));
	DoorLeft->SetWorldRotation(FRotator(0, 181, 0));

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	OverlapVolume->SetupAttachment(RootComponent);
	OverlapVolume->SetBoxExtent(FVector(200, 200, 200));
	OverlapVolume->SetWorldLocation(FVector(0, 0, 0));
	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OverlapBegins);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if ( OpenDoor && DoorRight->GetComponentRotation().Yaw < 170)
	{
		DoorRight->AddWorldRotation(FRotator(0, 1, 0));
		DoorLeft->AddWorldRotation(FRotator(0, -1, 0));
		GEngine->AddOnScreenDebugMessage(1, 5, FColor::Red, DoorLeft->GetComponentRotation().ToString());
	}

}

void ADoor::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	ATowerofAngraCharacter * Character = Cast<ATowerofAngraCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComponent != nullptr) && (OtherActor == Character))
	{
		
		if (Character)
		{
			GEngine->AddOnScreenDebugMessage(1, 5, FColor::Red, DoorRight->GetComponentRotation().ToString());
			this->OpenDoor = true;
			//GEngine->AddOnScreenDebugMessage(1, 5, FColor::Red, TEXT("Get Item"));
		}
	}

}

