// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillEffect.generated.h"

UCLASS()
class TOWEROFANGRA_API ASkillEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkillEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

		class UParticleSystemComponent* Particle;


		FTimerHandle DestroyTimer;
		void CallDestroy();
		void CallConstruct();
	UPROPERTY(VisibleDefaultsOnly, Category = projectile)
		class USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere, Category = Movement)
		class UProjectileMovementComponent* ProjectileMovementComponent;

	UFUNCTION(BlueprintCallable)
		void FireInDirection(const FVector& ShootDirection);

	UFUNCTION()
		void OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor,
			UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit);

	int Count;


	FCollisionShape SkillColSphere;
	
};
