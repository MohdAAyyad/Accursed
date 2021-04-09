// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySword.h"
#include "Components/BoxComponent.h"
#include "../TopDownHSCharacter.h"
#include "DecisionComponentBase.h"
#include "EnemyAnimInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AIManager.h"

AEnemySword::AEnemySword() : AEnemyBase()
{
	attackBox = CreateDefaultSubobject< UBoxComponent>(TEXT("Attack box"));
	attackBox->SetupAttachment(RootComponent);
	attackBox->SetCollisionProfileName("EnemyAttack");
	attackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	attackBox->SetGenerateOverlapEvents(true);



}

void AEnemySword::BeginPlay()
{
	Super::BeginPlay();

	if (attackBox)
	{
		attackBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemySword::AttackBoxApplyDamage);
	}
}
void AEnemySword::ActivateAttack(int index_)
{
	if(attackBox)
		attackBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AEnemySword::EndAttack(int index_)
{
	if (attackBox)
		attackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	animInstance->attackIndex = -1;
	decisionComp->currentState = EEnemyState::RELOADING;
	FTimerHandle attackAgainTimeHandle;
	GetWorld()->GetTimerManager().SetTimer(attackAgainTimeHandle, decisionComp, &UDecisionComponentBase::EndReloading, decisionComp->delayBeforeAttackingAgain, false);

}

void AEnemySword::AttackBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
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
	}
}

void AEnemySword::WakeUp()
{
	Super::WakeUp();
	if (weapon)
	{
		FVector loc_ = GetActorLocation();
		//loc_.X += 200.0f;
		FName weaponSocketName = TEXT("SwordSocket");
		spawnedWeapon = GetWorld()->SpawnActor<AActor>(weapon, loc_, FRotator::ZeroRotator);
		spawnedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, weaponSocketName);
	}
}

void AEnemySword::EndDeath()
{
	if (spawnedWeapon)
		spawnedWeapon->Destroy();

	if (parentEnemy)
		parentEnemy->HandleEnemyChildDeath(this);

	if (aiManager)
		aiManager->HandleEnemyDeath();

	Destroy();
}