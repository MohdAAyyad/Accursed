// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DecisionComponentBase.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	SPAWNING = 0,
	PURSUING = 1,
	ATTACKING = 2,
	RELOADING = 3,
	STAGGERED = 4,
	DEAD = 5
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOPDOWNHS_API UDecisionComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDecisionComponentBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	class AEnemyBase* ownerEnemy;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category="Stats")
		float attackingRadius;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float delayBeforeAttackingAgain;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float delayBeforePursuingAgain;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float staggerDelay;

	class ATopDownHSCharacter* target;

	EEnemyState currentState;

	EEnemyState stateAfterStagger; //Some enemies may attack immediately after being staggered

	void ResetStateAfterStagger(); //Needs to be a function so that it's attached to a delay call
	void EndReloading();

	void SetOwnerEnemy(AEnemyBase* ref_);

		
};
