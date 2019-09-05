// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "HWeaponItem.generated.h"

UCLASS()
class TOWEROFANGRA_API AHWeaponItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHWeaponItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Item)
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = Item)
	UStaticMeshComponent* Item;
	
	UPROPERTY(VisibleAnywhere, CAtegory = Effect)
	UParticleSystemComponent* Effect;

	UPROPERTY(VisibleAnywhere)
	class URotatingMovementComponent* Movement;
private:
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* PSystem);

private:
	UPROPERTY(EditAnywhere, Category = State, Meta = (AllowPrivateAccess = true))
	float RotateSpeed;
};
