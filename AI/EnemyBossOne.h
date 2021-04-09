// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyMage.h"
#include "EnemyBossOne.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API AEnemyBossOne : public AEnemyMage
{
	GENERATED_BODY()
public:
	AEnemyBossOne();

protected:
	UPROPERTY(EditAnywhere, Category = "Projectiles")
		TArray<TSubclassOf<class AEnemyProjectileBase>> projectiles;


	TArray< AEnemyBase*> mages;

	UPROPERTY(EditAnywhere, Category = "SmallerMages")
		int numOfSupportingEnemies;

	UPROPERTY(EditAnywhere, Category = "Stats")
		float reloadTimePhase2;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float reloadTimePhase3;

	int pickedAttack;
	int phaseIndex;
	bool bSpawnAttackAtPlayerLoc;

	void BaseAttack() override;
	void ActivateAttack(int index_) override;
	void EndDeath() override;
	void ReactToHit() override;
	void AskAIManagerToSpawnEnemies();

	void DelayedDestroy(); //This enemy's destroy call is a bit delayed to allow any remaining children to be destroyed first

	void EnemyTakeHitCommonFunctionCalls() override;

public:

	void HandleEnemyChildDeath(AEnemyBase* enm_) override;
	
};
