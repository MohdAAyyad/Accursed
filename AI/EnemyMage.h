// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyMage.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API AEnemyMage : public AEnemyBase
{
	GENERATED_BODY()
public:
	AEnemyMage();
protected:
	UPROPERTY(EditAnywhere, Category ="Projectile")
		TSubclassOf<class AEnemyProjectileBase> projectile;

	UPROPERTY(EditAnywhere, Category = "Attack")
		int numberOfProjectilesToFirePerInterval;
	UPROPERTY(EditAnywhere, Category = "Attack")
		FVector projSpawnLocOffset;

	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystem* runAwayParticles;

		int numberOfProjectilesFired;

	virtual void ActivateAttack(int index_) override;
	virtual void EndAttack(int index_) override;
	virtual void ReactToHit() override;
	
};
