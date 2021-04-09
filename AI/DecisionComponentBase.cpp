// Fill out your copyright notice in the Description page of Project Settings.


#include "DecisionComponentBase.h"
#include "../TopDownHSCharacter.h"
#include "EnemyBase.h"

// Sets default values for this component's properties
UDecisionComponentBase::UDecisionComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	staggerDelay = delayBeforeAttackingAgain = attackingRadius = 0.0f;
	currentState = EEnemyState::RELOADING;
	stateAfterStagger = EEnemyState::PURSUING;
	ownerEnemy = nullptr;
	// ...
}


// Called when the game starts
void UDecisionComponentBase::BeginPlay()
{
	Super::BeginPlay();

	target = Cast<ATopDownHSCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	// ...
	
}


// Called every frame
void UDecisionComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDecisionComponentBase::ResetStateAfterStagger()
{
	currentState = stateAfterStagger;
	if (ownerEnemy)
	{
		if (currentState == EEnemyState::ATTACKING)
		{
			ownerEnemy->BaseAttack();
		}
	}
}

void UDecisionComponentBase::SetOwnerEnemy(AEnemyBase* ref_)
{
	ownerEnemy = ref_;
}

void UDecisionComponentBase::EndReloading()
{
	currentState = EEnemyState::PURSUING; //Back to pursuing
}

