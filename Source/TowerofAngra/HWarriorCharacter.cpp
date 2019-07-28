// Fill out your copyright notice in the Description page of Project Settings.

#include "HWarriorCharacter.h"
#include "HWarriorAnimInstance.h"
#include "HCharaterStateComponent.h"
#include "HMonster.h"

AHWarriorCharacter::AHWarriorCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WARRIOR(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Tusk.SK_CharM_Tusk"));
	if (SK_WARRIOR.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_WARRIOR.Object);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/TowerofAngra/Character/Warrior/Animations/Warrior_AnimBlueprint.Warrior_AnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);

	// Weapon
	FName WeaponSocket(TEXT("hand_rSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/TowerofAngra/Weapon/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight"));

		if (SK_WEAPON.Succeeded())
		{
			Weapon->SetSkeletalMesh(SK_WEAPON.Object);
		}

		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}

	//Set Particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PARTICLE(TEXT("/Game/VFX_Toolkit_V1/ParticleSystems/356Days/Par_KineticForce_01.Par_KineticForce_01"));
	SkillEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	if (PARTICLE.Succeeded())
	{
		SkillEffect->SetTemplate(PARTICLE.Object);
		SkillEffect->bAutoActivate = false;
	}

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundBase> JUMPSOUND(TEXT("/Game/HumanVocalizations/HumanMaleB/Wavs/voice_male_b_effort_jump_07.voice_male_b_effort_jump_07"));
	if (JUMPSOUND.Succeeded())
		JumpSound = JUMPSOUND.Object;

	InitCommon();
}

void AHWarriorCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHWarriorCharacter::InitCommon()
{
	Super::InitCommon();

	MaxCombo = 4;			// 콤보 개수
	fAttackPower = 50.f;			// 기본 공격력
	fSkillPower = 100.f;			// 스킬 공격력
	SkillMP = 20.f;					// 마나 소모
}

void AHWarriorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHWarriorCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	WarriorAnim = Cast<UHWarriorAnimInstance>(GetMesh()->GetAnimInstance());
	if (WarriorAnim == nullptr) return;

	WarriorAnim->OnMontageEnded.AddDynamic(this, &AHWarriorCharacter::OnAttackMontageEnded);
	WarriorAnim->OnMontageEnded.AddDynamic(this, &AHWarriorCharacter::OnSkillMontageEnded);

	WarriorAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			WarriorAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	WarriorAnim->OnAttackHitCheck.AddUObject(this, &AHWarriorCharacter::AttackCheck);
}

void AHWarriorCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AHWarriorCharacter::Attack);

	PlayerInputComponent->BindAction(TEXT("Skill"), EInputEvent::IE_Pressed, this, &AHWarriorCharacter::Skill);
}

void AHWarriorCharacter::Attack()
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
		WarriorAnim->PlayAttackMontage();
		WarriorAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AHWarriorCharacter::Skill()
{
	Super::Skill();

	if (IsSkilling) return;
	if (IsAttacking) return;
	if (CharacterState->GetMP() <= SkillMP)return;

	FinalMana += SkillMP;

	WarriorAnim->PlaySkillMontage();
	SkillEffect->Activate(true);
	// Effect 위치 다시 받기
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SkillEffect->Template, GetActorLocation(), GetActorRotation());
	UGameplayStatics::PlaySoundAtLocation(this, HowlSound, GetActorLocation());
	SkillCheck();

	IsSkilling = true;
}

void AHWarriorCharacter::OnAttackMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	// 공격이 끝나는 모션
	IsAttacking = false;

	AttackEndComboState();
}

void AHWarriorCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;

	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AHWarriorCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AHWarriorCharacter::AttackCheck()
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

void AHWarriorCharacter::OnSkillMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	IsSkilling = false;

	OnSkillEnd.Broadcast();
}

void AHWarriorCharacter::SkillCheck()
{
	// 충돌체크
	float AttackRadius = 500.f;

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
