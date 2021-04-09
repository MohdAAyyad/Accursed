// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyProjectileBase.h"
#include "DelayedProjectile.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API ADelayedProjectile : public AEnemyProjectileBase
{
	GENERATED_BODY()
public:
	ADelayedProjectile();

	void UpdateVel(FVector direciton_) override {}; //Doesn't move
	UPROPERTY(EditAnywhere, Category = "Stats")
		float delayUntilActivation;
protected:

	void BeginPlay() override;

	void EnableProjectileCollision();
	void DelayedDestroy();

	void SphereApplyDamage(UPrimitiveComponent* overlappedComponent_,
		AActor* otherActor_,
		UPrimitiveComponent* otherComp_,
		int32 otherBodyIndex_,
		bool bFromSweep_,
		const FHitResult& sweepResult_) override;

	
};
