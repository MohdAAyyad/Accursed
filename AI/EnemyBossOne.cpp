// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBossOne.h"
#include "EnemyAnimInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DecisionComponentBase.h"
#include "EnemyProjectileBase.h"
#include "AIManager.h"
#include "Components/CapsuleComponent.h"

AEnemyBossOne::AEnemyBossOne():AEnemyMage()
{
	reloadTimePhase2 = 1.5f;
	reloadTimePhase3 = 1.0f;
	numOfSupportingEnemies = 2;
	phaseIndex = 1;
}

void AEnemyBossOne::ActivateAttack(int index_)
{
	if (numberOfProjectilesFired < numberOfProjectilesToFirePerInterval)
	{
		FVector spawnLoc_ = FVector::ZeroVector;

		if (bSpawnAttackAtPlayerLoc)
			spawnLoc_ = decisionComp->target->GetActorLocation();
		else
			spawnLoc_ = GetActorLocation() + projSpawnLocOffset;
		if (projectiles[pickedAttack])
		{
			AEnemyProjectileBase* proj_ = GetWorld()->SpawnActor<AEnemyProjectileBase>(projectiles[pickedAttack], spawnLoc_, FRotator::ZeroRotator);

			if (proj_)
			{
				FVector direction_ = decisionComp->target->GetActorLocation() - proj_->GetActorLocation();
				direction_.Normalize();
				proj_->UpdateVel(direction_);
				proj_->projDamage = damage;
				numberOfProjectilesFired++;

				if (numberOfProjectilesFired >= numberOfProjectilesToFirePerInterval)
				{
					numberOfProjectilesFired = 0;
					EndAttack(index_);
				}
			}
		}
	}
}

void AEnemyBossOne::BaseAttack()
{
	Super::BaseAttack();

	pickedAttack = FMath::RandRange(0, projectiles.Num() - 1);

	bSpawnAttackAtPlayerLoc = false;

	if (pickedAttack == 0) //Thunder attacks
	{
		bSpawnAttackAtPlayerLoc = true;
		numberOfProjectilesToFirePerInterval = 6;
	}
	else if (pickedAttack == 1) //Tornadoes
	{
		numberOfProjectilesToFirePerInterval = 3;
	}

	
}
void AEnemyBossOne::ReactToHit()
{
	Super::ReactToHit();

	if (currentHP <= 0.3f * maxHP && phaseIndex == 2) //Enter phase 3
	{
		decisionComp->delayBeforeAttackingAgain = reloadTimePhase3;
		numOfSupportingEnemies = 4;
		phaseIndex = 3;
		if (mages.Num() < numOfSupportingEnemies)
			AskAIManagerToSpawnEnemies();

	}
	else if (currentHP <= 0.7f * maxHP && phaseIndex == 1) //Enter phase 2
	{
		decisionComp->delayBeforeAttackingAgain = reloadTimePhase2;
		phaseIndex = 2;

		if (mages.Num() < numOfSupportingEnemies)
			AskAIManagerToSpawnEnemies();
	}
}

void AEnemyBossOne::EnemyTakeHitCommonFunctionCalls()
{
	if (animInstance)
		animInstance->EnableHit();
	EnemyCheckStagger();
}

void AEnemyBossOne::AskAIManagerToSpawnEnemies()
{
	if (aiManager)
	{
		AEnemyBase* enm = aiManager->SpawnEnemiesForParentEnemy(2);
		mages.Push(enm);
		if (enm)
			enm->SetParentEnemy(this);
	}

	if (mages.Num() < numOfSupportingEnemies)
	{
		FTimerHandle spawnMoreEnemiesHandle;
		GetWorld()->GetTimerManager().SetTimer(spawnMoreEnemiesHandle, this, &AEnemyBossOne::AskAIManagerToSpawnEnemies, 3.0f, false);
	}
}

void AEnemyBossOne::EndDeath()
{
	if (aiManager)
		aiManager->HandleEnemyDeath();

	//Hide the enemy
	GetMesh()->SetHiddenInGame(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	for (int i = 0; i < mages.Num(); i++)
	{
		if (mages[i] != nullptr)
			mages[i]->EnemyTakeDamageNoAfflictions(1000.0f);
	}

	
	FTimerHandle deathHandle;
	GetWorld()->GetTimerManager().SetTimer(deathHandle, this, &AEnemyBossOne::DelayedDestroy, 8.0f, false);
}

void AEnemyBossOne::DelayedDestroy()
{
	Destroy();
}

void AEnemyBossOne::HandleEnemyChildDeath(AEnemyBase* enm_)
{
	if (mages.Contains(enm_))
	{
		mages.Remove(enm_);
	}
}