// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemySword.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API AEnemySword : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemySword();

protected:

	AActor* spawnedWeapon;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		TSubclassOf<AActor> weapon;
	UPROPERTY(EditAnywhere, Category = "Attack")
		class UBoxComponent* attackBox;

	void ActivateAttack(int index_) override;
	void EndAttack(int index_) override;

	void BeginPlay() override;
	void EndDeath() override;

	UFUNCTION()
	void AttackBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
		AActor* otherActor_,
		UPrimitiveComponent* otherComp_,
		int32 otherBodyIndex_,
		bool bFromSweep_,
		const FHitResult& sweepResult_);

	virtual void WakeUp() override;
	
};
