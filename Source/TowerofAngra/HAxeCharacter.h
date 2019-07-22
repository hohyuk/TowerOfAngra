// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "TowerofAngraCharacter.h"
#include "HAxeCharacter.generated.h"

/**
 * 
 */
UCLASS()
class TOWEROFANGRA_API AHAxeCharacter : public ATowerofAngraCharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AHAxeCharacter();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Attack() override;
	virtual void Skill() override;

public:
	UPROPERTY()
	class UHAxeAnimInstance* AxeAnim;

private:
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackStartComboState();
	void AttackEndComboState();

	void AttackCheck();		// 충돌체크
};
