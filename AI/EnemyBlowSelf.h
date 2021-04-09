// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyBlowSelf.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API AEnemyBlowSelf : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AEnemyBlowSelf();

protected:

	void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Explosion")
		class USphereComponent* explosionSphere;
	UPROPERTY(EditAnywhere, Category = "Explosion")
		class UParticleSystem* explosionParticles;

	void ActivateAttack(int index_) override;
	void DelayedDestroy();

	UFUNCTION()
		void ExplosionBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult& sweepResult_);

};
