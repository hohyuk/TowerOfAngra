// Fill out your copyright notice in the Description page of Project Settings.

#include "HDemon.h"
#include "HCharaterStateComponent.h"
#include "Components/WidgetComponent.h"

AHDemon::AHDemon()
{
	CurrentMonsterName = EMonsterName::DEMON;
	BodyCenter = 88.f;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/TowerofAngra/Demon/demon.demon"));
	if (SK_CARDBOARD.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, - BodyCenter), FRotator(0.f, -90.f, 0.f));

	// Anim
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> DEMON_ANIM(TEXT("/Game/TowerofAngra/Demon/Anim/DemonAnimBP.DemonAnimBP_C"));
	if (DEMON_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(DEMON_ANIM.Class);

	CharacterState->InitHP(300.f);

	// UI Pos
	HPBarWidget->SetupAttachment(GetMesh());
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 250.f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);

	// ¼Óµµ
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	DetectRadius = 1500;
	AttackRadius = 300.f;
	fAttackPower = 25.f;
}

