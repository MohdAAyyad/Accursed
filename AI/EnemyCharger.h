// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyCharger.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API AEnemyCharger : public AEnemyBase
{
	GENERATED_BODY()
public:
	AEnemyCharger();

protected:

	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, Category = "Attack")
		float chargeDuration;
	UPROPERTY(EditAnywhere, Category = "Attack")
		float chargeVelocity;
	UPROPERTY(EditAnywhere, Category = "Attack")
		float chargeDistance;
	UPROPERTY(EditAnywhere, Category = "Attack")
		class UBoxComponent* attackBox;
	UPROPERTY(EditAnywhere, Category = "Attack")
		TSubclassOf<AActor> attackIndicator;

	AActor* spawnedAttackIndicator;
	FVector chargeDestination;

	float originalMaxWalkSpeed;

	void EndCharge();
	void EndStagger();
	void BaseAttack() override;
	void EndDeath() override;

	void EnemyTakeHitCommonFunctionCalls() override;
	

	UFUNCTION()
		void AttackBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult& sweepResult_);
};
