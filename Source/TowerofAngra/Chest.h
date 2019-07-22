// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chest.generated.h"

UCLASS()
class 
	AChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* body;
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* head;
	UPROPERTY(EditAnywhere)
		class USceneComponent* Root;
	UPROPERTY(EditAnywhere)
		class UBoxComponent* CollisionBox;
	UFUNCTION()
		void OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor,
			UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	bool Open;
};
