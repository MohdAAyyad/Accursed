// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UEnemyAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class AEnemyBase* enmOwner;

	UFUNCTION(BlueprintCallable)
		void UpdateAnimInstanceProperties();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float forwardSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDeath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int attackIndex;

	
	void EnableHit();
	void EnableDeath();
	
};
