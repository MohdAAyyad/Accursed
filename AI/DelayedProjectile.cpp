// Fill out your copyright notice in the Description page of Project Settings.


#include "DelayedProjectile.h"
#include "Components/SphereComponent.h"
#include "../TopDownHSCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"

ADelayedProjectile::ADelayedProjectile():AEnemyProjectileBase()
{
	delayUntilActivation = 3.0f;
	sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InitialLifeSpan = 0.0f;
}

void ADelayedProjectile::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle enableCollisionHandle;
	GetWorld()->GetTimerManager().SetTimer(enableCollisionHandle, this, &ADelayedProjectile::EnableProjectileCollision, delayUntilActivation, false);
}

void ADelayedProjectile::EnableProjectileCollision()
{
	//Enable the collision and spawn the impact
	if (impact)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impact, GetActorLocation(), FRotator::ZeroRotator);
	sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	FTimerHandle delayedDestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(delayedDestroyHandle, this, &ADelayedProjectile::DelayedDestroy, 0.2f, false);
}
void ADelayedProjectile::DelayedDestroy()
{
	Destroy();
}

void ADelayedProjectile::SphereApplyDamage(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult& sweepResult_)
{
	if (otherActor_ != nullptr && otherComp_ != nullptr && otherActor_ != this)
	{
		//If you overlap with anything, die. If you overlap with the player, make sure they take damage first.
		ATopDownHSCharacter* player_ = Cast<ATopDownHSCharacter>(otherActor_);
		if (player_)
		{
			player_->PlayerTakeDamage(projDamage);
			Destroy();
		}
	}
}