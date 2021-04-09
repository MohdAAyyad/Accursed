// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


protected:
	
	UPROPERTY(BlueprintReadOnly)
		class ATopDownHSCharacter* owner;
public:
	UPlayerAnimInstance();
	UFUNCTION(BlueprintCallable)
		void UpdateAnimInstanceProperties();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float forwardSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int attackIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDeath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bMoveable; //When the player first shows up, they shouldn't be able to move and they shouldn't be able to move after they get a boost


	void EnableHit();
	void EnableDeath();
	void AddToAttackIndex();
	void Special();
	void Dash();
	void CastProj();
	
};
