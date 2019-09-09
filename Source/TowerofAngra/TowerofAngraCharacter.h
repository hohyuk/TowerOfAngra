// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "TowerofAngra.h"
#include "GameFramework/Character.h"
#include "TowerofAngraCharacter.generated.h"

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	NONE,
	COMMONSKILL,
	SKILL,
};

UCLASS(config=Game)
class ATowerofAngraCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	ATowerofAngraCharacter();
	
/* ================ HoHyuk ================ */
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
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
	void Cheat();
public:
	void Jump();
/* ---------------------- �Է� ---------------------- */

/* ---------------------- ĳ���� ���� ���� ---------------------- */
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsServerSend_Attacking;	// ������ �������� �����Ǻ�

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
	float fCommonSkillPower;		// ���뽺ų ���ݷ�

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	float CommonSkillMP;		// ���뽺ų �����Ҹ�

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	float SkillMP;		// ��ų �����Ҹ�

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill, Meta = (AllowPrivateAccess = true))
	ESkillType SKILL_TYPE;		// ������ �����ִ� ��ų Ÿ��

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	float CurrentHP;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	bool IsFillHP;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	bool IsFillMP;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	float FillHP_Limit;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = HP, Meta = (AllowPrivateAccess = true))
	float FillMP_Limit;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	class UParticleSystemComponent* AttackDamageEffect;
/* ---------------------- ĳ���� ���� ���� ---------------------- */

/* ---------------------- ĳ���� ������ �Լ� ---------------------- */
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
/* ---------------------- ĳ���� ������ �Լ� ---------------------- */

/* ---------------------- ���� ���� ---------------------- */
public:
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	UStaticMeshComponent* Weapon;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	UStaticMeshComponent* SubWeapon;

/* ---------------------- ���� ���� ---------------------- */

/* ---------------------- Tick �Լ� ---------------------- */
private:
	void SpringArmTick(float DeltaTime);
	void StateTick(float DeltaTime);

/* ---------------------- Tick �Լ� ---------------------- */

/* ---------------------- virtual �Լ� ---------------------- */
public:
	virtual void InitCommon();
	virtual void Attack();
	virtual void OtherPlayerAttack(int AttackCount);
	virtual void Skill();
	virtual void CommonSkill();
	virtual void CommonSkillCheck();		// ��ų �浹üũ
	virtual void ServerRecvSkillCheck(ESkillType skill_type);
	virtual void ServerSetHP(float hp);
	virtual void WeaponChange();
/* ---------------------- virtual �Լ� ---------------------- */

/* ---------------------- ���� ���� ĳ���� �Լ� ---------------------- */
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = PlayerType, Meta = (AllowPrivateAccess = true))
	EPlayerType CurrentPlayerType{ EPlayerType::NONE };

/* ---------------------- ���� ���� ĳ���� �Լ� ---------------------- */

/* ---------------------- UCameraShake ---------------------- */
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> CameraShake;
/* ---------------------- UCameraShake ---------------------- */
/* ================ HoHyuk ================ */

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

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* OtherPlayerHPBarWidget;

	// Sound
public:
	class USoundBase* HowlSound;
	class USoundBase* AtkSound;
	class USoundBase* JumpSound;
	class USoundBase* EatSound;
/* ======= Server ======= */
public:
	int SessionId;
/* ======= Server ======= */
};

