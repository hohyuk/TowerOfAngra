// Fill out your copyright notice in the Description page of Project Settings.

#include "HWarriorCharacter.h"


AHWarriorCharacter::AHWarriorCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WARRIOR(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Tusk.SK_CharM_Tusk"));
	if (SK_WARRIOR.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_WARRIOR.Object);
	}
}

void AHWarriorCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHWarriorCharacter::InitCommon()
{
	Super::InitCommon();
}

void AHWarriorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHWarriorCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AHWarriorCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
