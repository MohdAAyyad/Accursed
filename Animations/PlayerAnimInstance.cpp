// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "../TopDownHSCharacter.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
	attackIndex = -1;
	bDash = false;
	bHit = false;
	bDeath = false;
	bMoveable = false;
}
void UPlayerAnimInstance::UpdateAnimInstanceProperties()
{
	if (!owner)
	{
		owner = Cast<ATopDownHSCharacter>(TryGetPawnOwner());
	}
	else
	{
		forwardSpeed = owner->GetVelocity().Size();
	}
}

void UPlayerAnimInstance::AddToAttackIndex()
{
	if(attackIndex<2)
		attackIndex++;
}

void UPlayerAnimInstance::Special()
{
	bHit = false;
	attackIndex = 3;
}

void UPlayerAnimInstance::Dash()
{
	attackIndex = -1;
	bDash = true;
}

void UPlayerAnimInstance::EnableHit()
{
	if (attackIndex < 3) //Casting and special cannot be interrupted
	{
		attackIndex = -1;
		bDash = false;
		bHit = true;
	}
}

void UPlayerAnimInstance::EnableDeath()
{
	attackIndex = -1;
	bDash = false;
	bHit = false;
	bDeath = true;
}

void UPlayerAnimInstance::CastProj()
{
	if (attackIndex < 4)
	{
		bDash = false;
		bHit = false;
		attackIndex = 4;
	}
}