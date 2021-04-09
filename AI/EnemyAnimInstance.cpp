// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "EnemyBase.h"


UEnemyAnimInstance::UEnemyAnimInstance()
{
	enmOwner = nullptr;
	forwardSpeed = 0.0f;
	bHit = bDeath = false;
	attackIndex = -1;
}

void UEnemyAnimInstance::UpdateAnimInstanceProperties()
{
	if (!enmOwner)
	{
		enmOwner = Cast<AEnemyBase>(TryGetPawnOwner());
	}
	else
	{
		forwardSpeed = enmOwner->GetVelocity().Size();
	}
}

void UEnemyAnimInstance::EnableHit()
{
	if (!bHit)
	{
		forwardSpeed = 0.0f;
		bDeath = false;
		attackIndex = -1;
		bHit = true;
	}
}
void UEnemyAnimInstance::EnableDeath()
{
	if (!bDeath)
	{
		forwardSpeed = 0.0f;
		attackIndex = -1;
		bHit = false;
		bDeath = true;
	}
}