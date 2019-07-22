// Fill out your copyright notice in the Description page of Project Settings.

#include "HGolem.h"
#include "HCharaterStateComponent.h"
#include "Components/WidgetComponent.h"


AHGolem::AHGolem()
{
	CurrentMonsterName = EMonsterName::GOLEM;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/TowerofAngra/Monster/Golem/Mesh/SK_Golem.SK_Golem"));
	if (SK_CARDBOARD.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	// Anim
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> GOLEM_ANIM(TEXT("/Game/TowerofAngra/Monster/Golem/Animations/GolemAnimBlueprint.GolemAnimBlueprint_C"));
	if (GOLEM_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(GOLEM_ANIM.Class);

	CharacterState->InitHP(200.f);

	// UI Pos
	HPBarWidget->SetupAttachment(GetMesh());
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	
	// °ñ·¥ ¼Óµµ
	GetCharacterMovement()->MaxWalkSpeed = 100.f;

	DetectRadius = 1500;
	AttackRadius = 150.f;
	fAttackPower = 25.f;
}
