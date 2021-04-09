// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingEnemyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AHomingEnemyProjectile::AHomingEnemyProjectile():AEnemyProjectileBase()
{
	InitialLifeSpan = 10.0f;
}

void AHomingEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	movement->HomingTargetComponent = GetWorld()->GetFirstPlayerController()->GetPawn()->GetRootComponent();
	movement->bIsHomingProjectile = true;
	movement->HomingAccelerationMagnitude = movement->Velocity.Size();
}

void AHomingEnemyProjectile::UpdateVel(FVector direciton_)
{
	movement->Velocity = direciton_ * movement->MaxSpeed;
	movement->HomingTargetComponent = GetWorld()->GetFirstPlayerController()->GetPawn()->GetRootComponent();
	movement->bIsHomingProjectile = true;
	movement->HomingAccelerationMagnitude = movement->Velocity.Size();
}