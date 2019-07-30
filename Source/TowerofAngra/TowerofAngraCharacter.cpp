// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TowerofAngraCharacter.h"
#include "HCharaterStateComponent.h"
#include "Components/WidgetComponent.h"
#include "HHUDWidget.h"
#include "HOtherPlayerHPWidget.h"


//////////////////////////////////////////////////////////////////////////
// ATowerofAngraCharacter

ATowerofAngraCharacter::ATowerofAngraCharacter()
{
	/* ======= Server ======= */
	SessionId = -1;
	/* ======= Server ======= */

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("TOACharater"));			// 충돌체크할때 캐릭터의 이름을 찾는다.

	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	SetCamMode(ECAMMode::DYNAMIC_CAM);
	
	// 캐릭터 UI 및 상태 =======================
	// UI
	UI_Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
	static ConstructorHelpers::FClassFinder<UUserWidget> UI(TEXT("/Game/TowerofAngra/UI/UI_HUD.UI_HUD_C"));
	if (UI.Succeeded())
	{
		UI_Widget->SetWidgetClass(UI.Class);
		UI_Widget->SetVisibility(false);
	}

	CharacterState = CreateDefaultSubobject<UHCharaterStateComponent>(TEXT("CHARACTERSTATE"));	// 캐릭터 상태

	CharacterState->InitHP(100.f);

	// HpUI
	OtherPlayerHPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OTHERHPBARWIDGET"));

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_OtherPlayerHPBar(TEXT("/Game/TowerofAngra/UI/UI_PlayerHpBar.UI_PlayerHpBar_C"));
	if (UI_OtherPlayerHPBar.Succeeded())
	{
		OtherPlayerHPBarWidget->SetWidgetClass(UI_OtherPlayerHPBar.Class);
		OtherPlayerHPBarWidget->SetDrawSize(FVector2D(150.f, 50.f));
		// UI Pos
		OtherPlayerHPBarWidget->SetupAttachment(GetMesh());
		OtherPlayerHPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
		OtherPlayerHPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
		//IsHP_Show = false;
	}
	
	// =========================================
}

void ATowerofAngraCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsPlayerControlled()) return;

	// HUD
	auto CharacterWidget = Cast<UHHUDWidget>(UI_Widget->GetUserWidgetObject());

	if (nullptr != CharacterWidget)
	{
		CharacterWidget->AddToViewport(1);
		CharacterWidget->BindCharacterState(CharacterState);
	}

	auto OtherCharacterWidget = Cast<UHOtherPlayerHPWidget>(OtherPlayerHPBarWidget->GetUserWidgetObject());
	if (nullptr != OtherCharacterWidget)
	{
		OtherCharacterWidget->BindCharacterState(CharacterState);
	}
}

void ATowerofAngraCharacter::InitCommon()
{
	CurrentPlayerType = PlayerType;		// 내 캐릭 타입

	ArmLengthSpeed = 3.f;
	ArmRotationSpeed = 10.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;	// 점프 높이
	FinalDamage = 0.f;
	FinalMana = 0.f;
	IsAttacking = false;
	IsSkilling = false;
	fAttackPower = 50.f;			// 기본 공격력
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATowerofAngraCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATowerofAngraCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATowerofAngraCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATowerofAngraCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATowerofAngraCharacter::Turn);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ATowerofAngraCharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &ATowerofAngraCharacter::ViewChange);

	TOALOG_S(Warning);
}

void ATowerofAngraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArmTick(DeltaTime);

	StateTick(DeltaTime);
}

void ATowerofAngraCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ATowerofAngraCharacter::SetCamMode(ECAMMode NewCamMode)
{
	CurrentCamMode = NewCamMode;
	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
		ArmLengthTo = 500.f;
		ArmRotationTo = FRotator(-15.f, 0.f, 0.f);
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		break;
	case ECAMMode::STATIC_CAM:
		ArmLengthTo = 800.f;
		ArmRotationTo = FRotator(-45.f, 0.f, 0.f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		break;
	}
}

float ATowerofAngraCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (IsPlayerControlled())
		FinalDamage += Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return FinalDamage;
}

void ATowerofAngraCharacter::SpringArmTick(float DeltaTime)
{
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
	case ECAMMode::STATIC_CAM:
		SpringArm->RelativeRotation = FMath::RInterpTo(SpringArm->RelativeRotation, ArmRotationTo, DeltaTime, ArmRotationSpeed);
		break;
	}

	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
	case ECAMMode::STATIC_CAM:
		if (DirectionToMove.SizeSquared() > 0.f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
		break;
	}
}

void ATowerofAngraCharacter::StateTick(float DeltaTime)
{
	// 마나 채우기
	CharacterState->SetMP_FILL(DeltaTime);

	// HP 데미지
	if (FinalDamage > 0.f)
	{
		float DamageSpeed = DeltaTime * 40.f;
		CharacterState->SetDamage(DamageSpeed);
		FinalDamage -= DamageSpeed;
	}
	else
		FinalDamage = 0.f;
	// 스킬 사용시 마나 감소
	if (FinalMana > 0.f)
	{
		float ManaSpeed = DeltaTime * 50.f;
		CharacterState->SetMP(ManaSpeed);
		FinalMana -= ManaSpeed;
	}
	else
		FinalMana = 0.f;
}

void ATowerofAngraCharacter::Attack()
{
}

void ATowerofAngraCharacter::Skill()
{
}

void ATowerofAngraCharacter::MoveForward(float NewAxisValue)
{
	if (IsSkilling) return;
	if (IsAttacking) return;
	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	case ECAMMode::STATIC_CAM:
		DirectionToMove.X = NewAxisValue;
		break;
	}
}

void ATowerofAngraCharacter::MoveRight(float NewAxisValue)
{
	if (IsSkilling) return;
	if (IsAttacking) return;
	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
	case ECAMMode::STATIC_CAM:
		DirectionToMove.Y = NewAxisValue;
		break;
	}
}

void ATowerofAngraCharacter::LookUp(float NewAxisValue)
{
	//switch (CurrentCamMode)
	//{
	//case ECAMMode::DYNAMIC_CAM:
	//	//AddControllerPitchInput(NewAxisValue);
	//	break;
	//}
}

void ATowerofAngraCharacter::Turn(float NewAxisValue)
{
	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
		AddControllerYawInput(NewAxisValue);
		break;
	}
}

void ATowerofAngraCharacter::Jump()
{
	if (IsSkilling) return;
	Super::Jump();
	UGameplayStatics::PlaySoundAtLocation(this, JumpSound, GetActorLocation());
}

void ATowerofAngraCharacter::ViewChange()
{
	switch (CurrentCamMode)
	{
	case ECAMMode::DYNAMIC_CAM:
		//GetController()->SetControlRotation(GetActorRotation());
		SetCamMode(ECAMMode::STATIC_CAM);
		break;
	case ECAMMode::STATIC_CAM:
		GetController()->SetControlRotation(SpringArm->RelativeRotation);
		SetCamMode(ECAMMode::DYNAMIC_CAM);
		break;
	}
}
