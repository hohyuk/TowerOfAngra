// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Fence.generated.h"


UCLASS()
class TOWEROFANGRA_API AFence : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFence();
	FTimerHandle TimerHandler;
	class UBoxComponent* Overlap;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	int Count;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class UStaticMeshComponent* Fence;

	void LayingDown();

	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor,
		UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit);



	
};
