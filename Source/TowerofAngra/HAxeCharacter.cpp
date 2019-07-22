// Fill out your copyright notice in the Description page of Project Settings.

#include "HAxeCharacter.h"
#include "HAxeAnimInstance.h"

AHAxeCharacter::AHAxeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_AXE(TEXT("/Game/TowerofAngra/Character/AxeChar/Axer.Axer"));
	if (SK_AXE.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_AXE.Object);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AXE_ANIM(TEXT("/Game/TowerofAngra/Character/AxeChar/Anim/HAxe_AnimBlueprint.HAxe_AnimBlueprint_C"));
	if (AXE_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(AXE_ANIM.Class);

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundBase> JUMPSOUND(TEXT("/Game/HumanVocalizations/HumanMaleB/Wavs/voice_male_b_effort_jump_07.voice_male_b_effort_jump_07"));
	if (JUMPSOUND.Succeeded())
		JumpSound = JUMPSOUND.Object;


	MaxCombo = 3;			// 콤보 개수
}

void AHAxeCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHAxeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHAxeCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AxeAnim = Cast<UHAxeAnimInstance>(GetMesh()->GetAnimInstance());
	if (AxeAnim == nullptr) return;

	AxeAnim->OnMontageEnded.AddDynamic(this, &AHAxeCharacter::OnAttackMontageEnded);

	AxeAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			AxeAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	AxeAnim->OnAttackHitCheck.AddUObject(this, &AHAxeCharacter::AttackCheck);
}

void AHAxeCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AHAxeCharacter::Attack);
}

void AHAxeCharacter::Attack()
{
	Super::Attack();

	if (IsSkilling) return;

	if (IsAttacking)
	{
		if (CanNextCombo)
			IsComboInputOn = true;
	}
	else
	{
		AttackStartComboState();
		AxeAnim->PlayAttackMontage();
		AxeAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AHAxeCharacter::Skill()
{
	Super::Skill();
}

void AHAxeCharacter::OnAttackMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	IsAttacking = false;

	AttackEndComboState();
}

void AHAxeCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;

	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AHAxeCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AHAxeCharacter::AttackCheck()
{
	// 충돌체크
	float FinalAttackRange = 80.f;
	float AttackRadius = 50.f;
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

#if ENABLE_DRAW_DEBUG

	/*FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 3.f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);*/

#endif

	if (bResult)
	{
		if (HitResult.Actor.IsValid())
		{
			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(fAttackPower, DamageEvent, GetController(), this);
		}
	}
}
