// Fill out your copyright notice in the Description page of Project Settings.

#include "HLizard.h"
#include "HCharaterStateComponent.h"
#include "Components/WidgetComponent.h"

AHLizard::AHLizard()
{
	CurrentMonsterName = EMonsterName::LIZARD;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/TowerofAngra/Monster/LizardBeast/Meshes/SK_LizardBeast.SK_LizardBeast"));
	if (SK_CARDBOARD.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	// Anim
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> LIZARD_ANIM(TEXT("/Game/TowerofAngra/Monster/LizardBeast/Animations/LizardAnimBlueprint.LizardAnimBlueprint_C"));
	if (LIZARD_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(LIZARD_ANIM.Class);

	CharacterState->InitHP(300.f);

	// UI Pos
	HPBarWidget->SetupAttachment(GetMesh());
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);

	// ¼Óµµ
	GetCharacterMovement()->MaxWalkSpeed = 500.f;

	DetectRadius = 1000;
	AttackRadius = 80.f;
	AttackRange = 80.f;
	fAttackPower = 15.f;
}