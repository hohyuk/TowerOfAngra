// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "TowerofAngra.h"
#include "GameFramework/Character.h"
#include "TowerofAngraCharacter.generated.h"

UCLASS(config=Game)
class ATowerofAngraCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	ATowerofAngraCharacter();
	
/* ================ HoHyuk ================ */

/* ---------------------- ī�޶� ---------------------- */
public:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;
protected:
	// ī�޶� ��� 
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
/* ---------------------- ī�޶� ---------------------- */

/* ---------------------- �Է� ---------------------- */
protected:
	void MoveForward(float NewAxisValue);
	void MoveRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
	void ViewChange();
public:
	void Jump();
/* ---------------------- �Է� ---------------------- */

/* ---------------------- ĳ���� ���� ���� ---------------------- */
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
	float fAttackPower;		// ���ݷ�

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	float fSkillPower;		// ��ų ���ݷ�

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	float SkillMP;		// ��ų �����Ҹ�

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

/* ---------------------- ĳ���� ���� ���� ---------------------- */

/* ---------------------- ĳ���� ������ �Լ� ---------------------- */
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
/* ---------------------- ĳ���� ������ �Լ� ---------------------- */

/* ---------------------- Init �Լ� ---------------------- */

/* ---------------------- Init �Լ� ---------------------- */

/* ---------------------- Tick �Լ� ---------------------- */
private:
	void SpringArmTick(float DeltaTime);
	void StateTick(float DeltaTime);

/* ---------------------- Tick �Լ� ---------------------- */

/* ---------------------- virtual �Լ� ---------------------- */
public:
	virtual void InitCommon();
	virtual void Attack();
	virtual void Skill();
/* ---------------------- virtual �Լ� ---------------------- */

/* ---------------------- ���� ���� ĳ���� �Լ� ---------------------- */
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = PlayerType, Meta = (AllowPrivateAccess = true))
	EPlayerType CurrentPlayerType{ EPlayerType::NONE };

/* ---------------------- ���� ���� ĳ���� �Լ� ---------------------- */

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
	// UI����
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

