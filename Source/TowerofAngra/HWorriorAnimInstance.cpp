// Fill out your copyright notice in the Description page of Project Settings.

#include "HWorriorAnimInstance.h"



UHWorriorAnimInstance::UHWorriorAnimInstance()
{
	CurrentPawnSpeed = 0.f;
	IsInAir = false;
	IsDead = false;

}

void UHWorriorAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		// Jump
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

void UHWorriorAnimInstance::PlayAttackMontage()
{
}

void UHWorriorAnimInstance::PlaySkillMontage()
{
}

void UHWorriorAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
}

void UHWorriorAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UHWorriorAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UHWorriorAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
