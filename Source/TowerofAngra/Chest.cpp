// Fill out your copyright notice in the Description page of Project Settings.

#include "Chest.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TowerofAngraCharacter.h"
#include "Engine.h"
// Sets default values
AChest::AChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ChestBody(TEXT("/Game/MultistoryDungeons/Meshes/Props/Treasure_Chest_Base_02"));
	
	body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("body"));
	body->SetStaticMesh(ChestBody.Object);
	body->SetupAttachment(RootComponent);
	body->SetRelativeLocation(FVector(0, 0, 0));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ChestHead(TEXT("/Game/MultistoryDungeons/Meshes/Props/Treasure_Chest_lid_01"));

	head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("head"));
	head->SetStaticMesh(ChestHead.Object);
	head->SetupAttachment(RootComponent);
	head->SetRelativeLocation(FVector(0, -34, 40));

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetRelativeLocation(FVector(0, 0, 100));
	CollisionBox->InitBoxExtent(FVector(200, 200, 100));
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AChest::OverlapBegins);
	/*CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AChest::OnOverlapEnd);*/
	//Open = false;


}

// Called when the game starts or when spawned
void AChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Open && head->RelativeRotation.Roll>-130)
	{
		head->AddRelativeRotation(FRotator(0, 0, -1));
		GEngine->AddOnScreenDebugMessage(1, 5, FColor::Blue,FString::Printf(TEXT("Chest head angle : %f"), head->RelativeRotation.Roll));
	}

}

void AChest::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	ACharacter * Character = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if ((OtherActor == Character) && (OtherActor != this) && (OtherComponent != nullptr) )
	{

		GEngine->AddOnScreenDebugMessage(1, 5, FColor::Blue, TEXT("Open Chest!"));

		Open = true;
	}
}

void AChest::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if ((OtherActor == Character) && (OtherActor != this) && (OtherComp != nullptr))
	{
		GEngine->AddOnScreenDebugMessage(1, 5, FColor::Blue, TEXT("Close Chest!"));
		Open = false;
	}

}

