// Fill out your copyright notice in the Description page of Project Settings.

#include "Fence.h"
#include "ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "UnrealMathUtility.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
// Sets default values
AFence::AFence()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
		Fence = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Fence;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FMesh1(TEXT("StaticMesh'/Game/MultistoryDungeons/Meshes/FP_Update/Damaged_Bars_01.Damaged_Bars_01'"));
	Fence->SetStaticMesh(FMesh1.Object);
	Overlap = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Overlap->InitBoxExtent(FVector(100, 100, 200));
	Overlap->SetupAttachment(Fence);
	Overlap->SetRelativeLocation(FVector(0, 0, 150));
	Overlap->OnComponentBeginOverlap.AddDynamic(this, &AFence::OverlapBegins);

}

// Called when the game starts or when spawned
void AFence::BeginPlay()
{
	Super::BeginPlay();
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> FMesh1(TEXT("StaticMesh'/Game/MultistoryDungeons/Meshes/FP_Update/Damaged_Bars_01.Damaged_Bars_01'"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> FMesh2(TEXT("StaticMesh'/Game/MultistoryDungeons/Meshes/FP_Update/Damaged_Bars_02.Damaged_Bars_02'"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> FMesh3(TEXT("StaticMesh'/Game/MultistoryDungeons/Meshes/FP_Update/Damaged_Bars_03.Damaged_Bars_03'"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> FMesh4(TEXT("StaticMesh'/Game/MultistoryDungeons/Meshes/FP_Update/Damaged_Bars_04.Damaged_Bars_04'"));

	//if (FenceType == EFenceType::TYPE_FENCE1)
	//{
	//	Fence->SetStaticMesh(FMesh1.Object);
	//}
	//else if (FenceType == EFenceType::TYPE_FENCE2)
	//{
	//	Fence->SetStaticMesh(FMesh2.Object);
	//}
	//else if (FenceType == EFenceType::TYPE_FENCE3)
	//{
	//	Fence->SetStaticMesh(FMesh3.Object);
	//}
	//else if (FenceType == EFenceType::TYPE_FENCE4)
	//{
	//	Fence->SetStaticMesh(FMesh4.Object);
	//}
}

// Called every frame
void AFence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFence::LayingDown()
{
	Count++;
	if (Count<=100)
	{
		AddActorLocalOffset(FVector(0, 0, 10));
		//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Black, TEXT("Overlapped"));
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandler);
	}
}

void AFence::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, 
	int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{

	if (OtherActor != nullptr && OverlappedComponent != nullptr && OtherActor != this)
	{

		//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Black, TEXT("Overlapped"));
		GetWorldTimerManager().SetTimer(TimerHandler, this, &AFence::LayingDown, 0.0167, true);
		//Destroy();
	}
}

