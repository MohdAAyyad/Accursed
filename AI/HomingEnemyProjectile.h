// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyProjectileBase.h"
#include "HomingEnemyProjectile.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API AHomingEnemyProjectile : public AEnemyProjectileBase
{
	GENERATED_BODY()
public:
	AHomingEnemyProjectile();

	void UpdateVel(FVector direciton_) override;

protected:
	void BeginPlay() override;
	
};
