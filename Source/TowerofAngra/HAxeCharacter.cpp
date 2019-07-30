// Fill out your copyright notice in the Description page of Project Settings.

#include "HAxeCharacter.h"
#include "HAxeAnimInstance.h"
#include "HCharaterStateComponent.h"
#include "HMonster.h"

AHAxeCharacter::AHAxeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_AXE(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Ram.SK_CharM_Ram"));
	if (SK_AXE.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_AXE.Object);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AXE_ANIM(TEXT("/Game/TowerofAngra/Character/AxeChar/Animations/HAxe_AnimBlueprint.HAxe_AnimBlueprint_C"));
	if (AXE_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(AXE_ANIM.Class);

	// Weapon
	FName WeaponSocket(TEXT("hand_rSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Axes/Blade_FissureBlade/SK_Blade_FissureBlade.SK_Blade_FissureBlade"));

		if (SK_WEAPON.Succeeded())
		{
			Weapon->SetSkeletalMesh(SK_WEAPON.Object);
		}

		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundBase> JUMPSOUND(TEXT("/Game/HumanVocalizations/HumanMaleB/Wavs/voice_male_b_effort_jump_07.voice_male_b_effort_jump_07"));
	if (JUMPSOUND.Succeeded())
		JumpSound = JUMPSOUND.Object;


	InitCommon();
}

void AHAxeCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHAxeCharacter::InitCommon()
{
	Super::InitCommon();
	MaxCombo = 4;			// �޺� ����

	fAttackPower = 50.f;			// �⺻ ���ݷ�
	fSkillPower = 200.f;			// ��ų ���ݷ�
	SkillMP = 20.f;					// ���� �Ҹ�

	IsServerSend_Attacking = false;
	AttackEndComboState();
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
	AxeAnim->OnMontageEnded.AddDynamic(this, &AHAxeCharacter::OnSkillMontageEnded);

	AxeAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			AxeAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	AxeAnim->OnAttackHitCheck.AddUObject(this, &AHAxeCharacter::AttackCheck);
	AxeAnim->OnSkillHitCheck.AddUObject(this, &AHAxeCharacter::SkillCheck);
}

void AHAxeCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AHAxeCharacter::Attack);

	PlayerInputComponent->BindAction(TEXT("Skill"), EInputEvent::IE_Pressed, this, &AHAxeCharacter::Skill);
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

	if (IsSkilling) return;
	if (IsAttacking) return;
	if (CharacterState->GetMP() <= SkillMP)return;

	FinalMana += SkillMP;

	AxeAnim->PlaySkillMontage();
	//SkillEffect->Activate(true);
	//// Effect ��ġ �ٽ� �ޱ�
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SkillEffect->Template, GetActorLocation(), GetActorRotation());

	//SkillCheck();

	IsSkilling = true;
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
	// �浹üũ
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

void AHAxeCharacter::OnSkillMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	IsSkilling = false;

	OnSkillEnd.Broadcast();
}

void AHAxeCharacter::SkillCheck()
{
	// �浹üũ
	float AttackRadius = 300.f;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	if (bResult)
	{
		for (auto OverlapResult : OverlapResults)
		{
			AHMonster* Monster = Cast<AHMonster>(OverlapResult.GetActor());
			if (Monster)
			{
				FDamageEvent DamageEvent;
				Monster->DamageAnim();
				OverlapResult.Actor->TakeDamage(fSkillPower, DamageEvent, GetController(), this);
			}
		}
	}
}
