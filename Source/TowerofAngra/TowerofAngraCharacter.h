// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "TowerofAngraCharacter.generated.h"

UCLASS(config=Game)
class ATowerofAngraCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	ATowerofAngraCharacter();
	
/* ================ HoHyuk ================ */

/* ---------------------- 카메라 ---------------------- */
public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;
protected:
	// 카메라 모드 
	enum class ECAMMode
	{
		DYNAMIC_CAM,
		STATIC_CAM,
	};

	void SetCamMode(ECAMMode NewCamMode);
	ECAMMode CurrentCamMode = ECAMMode::STATIC_CAM;
	FVector DirectionToMove = FVector::ZeroVector;

	float ArmLengthTo = 0.f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.f;
	float ArmRotationSpeed = 0.f;
/* ---------------------- 카메라 ---------------------- */

/* ---------------------- 입력 ---------------------- */
protected:
	void MoveForward(float NewAxisValue);
	void MoveRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
	void ViewChange();
public:
	void Jump();
/* ---------------------- 입력 ---------------------- */

/* ---------------------- 캐릭터 공통 변수 ---------------------- */
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	bool IsSkilling;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Damage, Meta = (AllowPrivateAccess = true))
	float FinalDamage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Mana, Meta = (AllowPrivateAccess = true))
	float FinalMana;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float fAttackPower;		// 공격력

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	float fSkillPower;		// 스킬 공격력

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	float SkillMP;		// 스킬 마나소모

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

/* ---------------------- 캐릭터 공통 변수 ---------------------- */

/* ---------------------- 캐릭터 데미지 함수 ---------------------- */
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
/* ---------------------- 캐릭터 데미지 함수 ---------------------- */

/* ---------------------- Init 함수 ---------------------- */

/* ---------------------- Init 함수 ---------------------- */

/* ---------------------- Tick 함수 ---------------------- */
private:
	void SpringArmTick(float DeltaTime);
	void StateTick(float DeltaTime);

/* ---------------------- Tick 함수 ---------------------- */

/* ---------------------- virtual 함수 ---------------------- */
public:
	virtual void InitCommon();
	virtual void Attack();
	virtual void Skill();
/* ---------------------- virtual 함수 ---------------------- */

/* ================ HoHyuk ================ */

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	class ASkillEffect* Effect;

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
protected:
	// UI관리
	UPROPERTY(VisibleAnywhere, Category = State)
	class UHCharaterStateComponent* CharacterState;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* UI_Widget;

	// Sound
protected:
	class USoundBase* HowlSound;
	class USoundBase* AtkSound;
	class USoundBase* JumpSound;

/* ======= Server ======= */
public:
	int SessionId;
/* ======= Server ======= */
};

