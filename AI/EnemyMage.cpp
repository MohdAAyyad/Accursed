// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMage.h"
#include "EnemyProjectileBase.h"
#include "../TopDownHSCharacter.h"
#include "DecisionComponentBase.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "EnemyAnimInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AIManager.h"

AEnemyMage::AEnemyMage():AEnemyBase()
{
	numberOfProjectilesFired = 0;
	numberOfProjectilesToFirePerInterval = 3;
	projSpawnLocOffset = FVector(200.0f, 0.0f, 200.0f);
	bCanBeStaggered = false;
}


void AEnemyMage::ActivateAttack(int index_)
{
	if (numberOfProjectilesFired < numberOfProjectilesToFirePerInterval)
	{
		if (projectile)
		{
			AEnemyProjectileBase* proj_ = GetWorld()->SpawnActor<AEnemyProjectileBase>(projectile, GetActorLocation() + projSpawnLocOffset, FRotator::ZeroRotator);
			
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
void AEnemyMage::EndAttack(int index_)
{
	animInstance->attackIndex = -1;
	decisionComp->currentState = EEnemyState::RELOADING;
	FTimerHandle attackAgainTimeHandle;
	GetWorld()->GetTimerManager().SetTimer(attackAgainTimeHandle, decisionComp, &UDecisionComponentBase::EndReloading, decisionComp->delayBeforeAttackingAgain, false);
}
void AEnemyMage::ReactToHit()
{
	FVector resultLoc_ = FVector::ZeroVector;
	if (aiManager)
		resultLoc_ = aiManager->GetRandomSpawnLoc();

	resultLoc_.Z = GetActorLocation().Z;

	if (runAwayParticles)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), runAwayParticles, GetActorLocation(), FRotator::ZeroRotator);

	SetActorLocation(resultLoc_);

	if (runAwayParticles)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), runAwayParticles, GetActorLocation(), FRotator::ZeroRotator);

	//Pick a random axis of the player and use it to determine the target location. Spawn particles where you start and where you end up
	/*int axisIndex_ = FMath::RandRange(0, 2);

	FVector resultLoc_ = FVector();
	

	switch (axisIndex_)
	{
	case 0:	//Behind the player
		if(runAwayParticles)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), runAwayParticles, GetActorLocation(), FRotator::ZeroRotator);

		resultLoc_ = -decisionComp->target->GetActorForwardVector() * decisionComp->attackingRadius;
		resultLoc_.Z = GetActorLocation().Z;
		SetActorLocation(resultLoc_);

		if (runAwayParticles)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), runAwayParticles, GetActorLocation(), FRotator::ZeroRotator);
		break;
	case 1: //To the right of the player
		if (runAwayParticles)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), runAwayParticles, GetActorLocation(), FRotator::ZeroRotator);

		resultLoc_ = decisionComp->target->GetActorRightVector() * decisionComp->attackingRadius;
		resultLoc_.Z = GetActorLocation().Z;
		SetActorLocation(resultLoc_);

		if (runAwayParticles)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), runAwayParticles, GetActorLocation(), FRotator::ZeroRotator);
		break;
	case 2: //To the left of the player
		if (runAwayParticles)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), runAwayParticles, GetActorLocation(), FRotator::ZeroRotator);

		resultLoc_ = -decisionComp->target->GetActorRightVector() * decisionComp->attackingRadius;
		resultLoc_.Z = GetActorLocation().Z;
		SetActorLocation(resultLoc_);

		if (runAwayParticles)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), runAwayParticles, GetActorLocation(), FRotator::ZeroRotator);
		break;
	}
	*/
	animInstance->bHit = false;
	numberOfProjectilesFired = 0;
	decisionComp->currentState = EEnemyState::PURSUING; //Disable stagger
}
