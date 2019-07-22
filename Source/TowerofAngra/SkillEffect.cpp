// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "SkillEffect.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h" 
#include "Engine/World.h"
#include "Runtime/Engine/Public/WorldCollision.h"

// Sets default values
ASkillEffect::ASkillEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setup collision volume & Attach Root Component
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(25.0f);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASkillEffect::OverlapBegins);
	//Collision->collision
	RootComponent = Collision;

	//Skill Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectParticle(TEXT("ParticleSystem'/Game/VFX_Toolkit_V1/ParticleSystems/356Days/Par_TorchFire_2.Par_TorchFire_2'"));
	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));
	Particle->SetTemplate(EffectParticle.Object);
	Particle->SetupAttachment(Collision);
	Particle->SetWorldRotation(FRotator(0, 0, 0), false, NULL, ETeleportType::None);
	Particle->SetWorldScale3D(FVector(1.0, 1.0, 1.0));
	//Setup Projectile Setting
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(Collision);
	ProjectileMovementComponent->InitialSpeed = 0.0f;
	ProjectileMovementComponent->MaxSpeed = 500.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	
	Count = 0;
}

// Called when the game starts or when spawned
void ASkillEffect::BeginPlay()
{
	Super::BeginPlay();
	//DrawDebugSphere(GetWorld(), Collision->GetComponentLocation(), Collision->GetScaledSphereRadius(), 8.f, FColor::Red, 0);
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ASkillEffect::CallDestroy, 0.0167, true);
}

// Called every frame
void ASkillEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), Collision->GetScaledSphereRadius(), 5, FColor::Red, true, 1);
}

void ASkillEffect::CallDestroy()
{
	Count++;
	if (Count >= 120) {
		GEngine->AddOnScreenDebugMessage(1, 8, FColor::Red, TEXT("Destroy."));
		//Destroy();
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		ProjectileMovementComponent->Activate(false);
		Count = 0;
		ProjectileMovementComponent->InitialSpeed = 0;
		SetActorLocation(FVector(0, 0, 0));
		GetWorldTimerManager().ClearTimer(DestroyTimer);
	}
	//else
		//DrawDebugSphere(GetWorld(), Collision->GetComponentLocation(), Collision->GetScaledSphereRadius(), 8.f, FColor::Red, true,0.0167);
}

void ASkillEffect::CallConstruct()
{
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ASkillEffect::CallDestroy, 0.0167, true);
}

void ASkillEffect::FireInDirection(const FVector & ShootDirection)
{
	ProjectileMovementComponent->Velocity = (ShootDirection)* ProjectileMovementComponent->InitialSpeed;
	GEngine->AddOnScreenDebugMessage(1, 8, FColor::Red, ProjectileMovementComponent->Velocity.ToString());
	
}
                          
void ASkillEffect::OverlapBegins(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherbodyIdx, bool bFromSweep, const FHitResult & SweepHit)
{
	if (OtherActor != nullptr && OverlappedComponent != nullptr && OtherActor != this)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Black, TEXT("Overlapped"));
		
		//Destroy();
	}
}

