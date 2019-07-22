// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
		USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, Category = Door)
		UStaticMeshComponent* DoorLeft;
	UPROPERTY(VisibleAnywhere, Category = Door)
		UStaticMeshComponent* DoorRight;
	UPROPERTY(VisibleAnywhere, Category = Door)
		UStaticMeshComponent* DoorFrame;
	UPROPERTY(VisibleAnywhere, Category = Door)
		class UBoxComponent* OverlapVolume;
	bool OpenDoor;
	UFUNCTION()
		void OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit);
};
