// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharger.h"
#include "Components/BoxComponent.h"
#include "DecisionComponentBase.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../TopDownHSCharacter.h"
#include "EnemyAnimInstance.h"
#include "AIManager.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AEnemyCharger::AEnemyCharger() :AEnemyBase()
{
	chargeDuration = 2.0f;
	chargeVelocity = 700.0f;
	chargeDistance = 700.0f;
	chargeDestination = FVector();

	attackBox = CreateDefaultSubobject< UBoxComponent>(TEXT("Attack box"));
	attackBox->SetupAttachment(RootComponent);
	attackBox->SetCollisionProfileName("EnemyAttackPreset");
	attackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	attackBox->SetGenerateOverlapEvents(true);
	spawnedAttackIndicator = nullptr;
	bCanBeStaggered = false;
}

void AEnemyCharger::BeginPlay()
{
	Super::BeginPlay();
	if(attackBox)
		attackBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharger::AttackBoxApplyDamage);

	originalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void AEnemyCharger::Tick(float DeltaTime)
{
	ACharacter::Tick(DeltaTime);
		if (decisionComp->currentState == EEnemyState::PURSUING) //If you're pursuing the target, rotate and move towards them
		{
			FVector Direction = decisionComp->target->GetActorLocation() - GetActorLocation();
			FVector normalizedDirection = Direction;
			FRotator rot_ = GetActorRotation();

			normalizedDirection.Normalize();
			rot_.Yaw = FMath::Lerp(GetActorRotation().Yaw, normalizedDirection.Rotation().Yaw, 0.05f);
			SetActorRotation(rot_); //Always face the player when pursuing
			if (Direction.Size() > decisionComp->attackingRadius)
			{
				AddMovementInput(normalizedDirection, 1.0f);
			}
			else //Once you're close enough, attack
			{
				decisionComp->currentState = EEnemyState::ATTACKING;
				if (animInstance)
					animInstance->attackIndex = 0; //Ready to run animation starts playing
				GetCharacterMovement()->MaxWalkSpeed = chargeVelocity;
				FTimerHandle attackTimeHandle;
				GetWorld()->GetTimerManager().SetTimer(attackTimeHandle, this, &AEnemyCharger::BaseAttack, decisionComp->delayBeforeAttackingAgain, false); // A bit of delay before charging
				if (attackIndicator)
				{
					//Don't leave any artifacts behind
					if (spawnedAttackIndicator)
					{
						spawnedAttackIndicator->Destroy();
						spawnedAttackIndicator = nullptr;
					}
					FVector spawnLoc_ = GetActorLocation();
					spawnLoc_.Z -= 80.0f;
					spawnedAttackIndicator = GetWorld()->SpawnActor<AActor>(attackIndicator, spawnLoc_, normalizedDirection.Rotation()); //Spawn the indicator
				}

				chargeDestination = GetActorForwardVector() * chargeDistance; //This is where we're charging towards
				chargeDestination.Normalize();

			}
		}
		else if (decisionComp->currentState == EEnemyState::ATTACKING && animInstance->attackIndex == 1) //Make sure we're charging before moving
		{
			AddMovementInput(chargeDestination, 1.0f); //Charge towards the destination
		}
		else if (decisionComp->currentState == EEnemyState::RELOADING)
		{
			//Rotate towards the player between attacks
			FVector Direction = decisionComp->target->GetActorLocation() - GetActorLocation();
			FVector normalizedDirection = Direction;
			FRotator rot_ = GetActorRotation();

			normalizedDirection.Normalize();
			rot_.Yaw = FMath::Lerp(GetActorRotation().Yaw, normalizedDirection.Rotation().Yaw, 0.05f);
			SetActorRotation(rot_); //Always face the player when pursuing
		}
}

void AEnemyCharger::EndCharge()
{
	if (decisionComp->currentState != EEnemyState::STAGGERED) //Make sure we have not hit a wall
	{
		animInstance->attackIndex = -1;
		decisionComp->currentState = EEnemyState::RELOADING;
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->MaxWalkSpeed = originalMaxWalkSpeed;
		FTimerHandle attackAgainTimeHandle;
		GetWorld()->GetTimerManager().SetTimer(attackAgainTimeHandle, decisionComp, &UDecisionComponentBase::EndReloading, decisionComp->delayBeforeAttackingAgain, false);
	}
}

void AEnemyCharger::EndStagger()
{
	animInstance->bHit = false;
	animInstance->attackIndex = -1;
	decisionComp->currentState = EEnemyState::RELOADING; //Reloading state will allow the charger to lock onto the player again
	FTimerHandle attackAgainTimeHandle;
	GetWorld()->GetTimerManager().SetTimer(attackAgainTimeHandle, decisionComp, &UDecisionComponentBase::EndReloading, decisionComp->delayBeforeAttackingAgain, false);
}

void AEnemyCharger::AttackBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult& sweepResult_)
{
	if (otherActor_ != nullptr && otherComp_ != nullptr && otherActor_ != this)
	{
		ATopDownHSCharacter* player_ = Cast<ATopDownHSCharacter>(otherActor_);
		if (player_)
		{
			player_->PlayerTakeDamage(damage);
			attackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else //Charger box can only overlap with the player or with world statics
		{
			animInstance->attackIndex = 2;
			decisionComp->currentState = EEnemyState::STAGGERED;

			GetCharacterMovement()->MaxWalkSpeed = originalMaxWalkSpeed;
			attackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			FTimerHandle endStaggerTimeHandle;
			GetWorld()->GetTimerManager().SetTimer(endStaggerTimeHandle, this, &AEnemyCharger::EndStagger, decisionComp->staggerDelay, false); // A bit of delay before charging

		}
	}
}


void AEnemyCharger::BaseAttack()
{
	if (decisionComp->currentState == EEnemyState::ATTACKING)
	{
		if (currentAfflictions.bDefied)
		{
			if (delayedImpacts.Num() > 0)
				if (delayedImpacts[0] != nullptr)
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), delayedImpacts[0], GetActorLocation(), FRotator::ZeroRotator);

			currentHP -= damage * currentAfflictions.defiedDamagePercentage;
			if (currentHP <= 0.0f && decisionComp->currentState != EEnemyState::DEAD)
			{
				animInstance->attackIndex = -1;
				decisionComp->currentState = EEnemyState::DEAD;
				//Death
				if (animInstance)
					animInstance->EnableDeath();
			}
			currentAfflictions.bDefied = false;
		}

		if (animInstance)
			animInstance->attackIndex = 1; //Charge animation starts playing

		attackBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		if (spawnedAttackIndicator)
		{
			spawnedAttackIndicator->Destroy();
			spawnedAttackIndicator = nullptr;
		}

		FTimerHandle endAttackTimeHandle;
		GetWorld()->GetTimerManager().SetTimer(endAttackTimeHandle, this, &AEnemyCharger::EndCharge, chargeDuration, false); // A bit of delay before charging

	}
}

void AEnemyCharger::EndDeath()
{
	if (aiManager)
		aiManager->HandleEnemyDeath();
	if (parentEnemy)
		parentEnemy->HandleEnemyChildDeath(this);
	if (spawnedAttackIndicator)
	{
		spawnedAttackIndicator->Destroy();
		spawnedAttackIndicator = nullptr;
	}

	Destroy();
}