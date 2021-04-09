// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBlowSelf.h"
#include "Components/SphereComponent.h"
#include "DecisionComponentBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AIManager.h"
#include "../TopDownHSCharacter.h"


AEnemyBlowSelf::AEnemyBlowSelf() : AEnemyBase()
{
	explosionSphere = CreateDefaultSubobject< USphereComponent>(TEXT("Sphere Explosion"));
	explosionSphere->SetupAttachment(RootComponent);
	explosionSphere->SetCollisionProfileName("EnemyAttack");
	explosionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	explosionSphere->SetGenerateOverlapEvents(true);

}

void AEnemyBlowSelf::BeginPlay()
{
	Super::BeginPlay();
	if (explosionSphere)
	{
		explosionSphere->SetSphereRadius(decisionComp->attackingRadius);
		explosionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBlowSelf::ExplosionBoxApplyDamage);
	}
}

void AEnemyBlowSelf::ActivateAttack(int index_)
{
	decisionComp->currentState = EEnemyState::DEAD;
	FTimerHandle destroyTimeHandle;
	GetWorld()->GetTimerManager().SetTimer(destroyTimeHandle, this, &AEnemyBlowSelf::DelayedDestroy, 0.2f, false);
	if (explosionParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionParticles, GetActorLocation(),FRotator::ZeroRotator);
	}
	explosionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetHiddenInGame(true);
}

void AEnemyBlowSelf::DelayedDestroy()
{
	if (aiManager)
		aiManager->HandleEnemyDeath();

	Destroy();
}

void AEnemyBlowSelf::ExplosionBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
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
			explosionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}