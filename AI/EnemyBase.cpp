// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIManager.h"
#include "EnemyAnimInstance.h"
#include "Components/WidgetComponent.h"
#include "DecisionComponentBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	currentAfflictions = FEnemyAfflictions();

	maxHP = currentHP = 50.0f;

	maxSpeed = GetCharacterMovement()->MaxWalkSpeed;

	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	widgetComp->SetupAttachment(RootComponent);
	widgetComp->SetVisibility(false);

	poisonDamageIntervals = 0.1f;
	curseDamageInterval = 1.0f;
	damage = 25.0f;
	bCanBeStaggered = true;

	decisionComp = CreateDefaultSubobject<UDecisionComponentBase>(TEXT("Decision Comp"));

	GetCapsuleComponent()->SetCollisionProfileName("EnemyPreset");
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	animInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (animInstance)
	{
		if (!animInstance->bHit)
		{
			if (decisionComp->currentState == EEnemyState::PURSUING) //If you're pursuing the target, rotate and move towards them
			{
				FVector Direction = decisionComp->target->GetActorLocation() - GetActorLocation();
				FVector normalizedDirection = Direction;
				FRotator rot_ = GetActorRotation();

				normalizedDirection.Normalize();
				rot_.Yaw = FMath::Lerp(GetActorRotation().Yaw, normalizedDirection.Rotation().Yaw, 0.05f);
				SetActorRotation(rot_); //Always face the player when pursuing
				if (Direction.Size() > decisionComp->attackingRadius)
				{
					AddMovementInput(normalizedDirection, 1.0f);
				}
				else //Once you're close enough, attack
				{
					GetCharacterMovement()->StopMovementImmediately();
					decisionComp->currentState = EEnemyState::ATTACKING;
					FTimerHandle attackTimeHandle;
					GetWorld()->GetTimerManager().SetTimer(attackTimeHandle, this, &AEnemyBase::BaseAttack, decisionComp->delayBeforeAttackingAgain, false);

				}
			}
		}
	}

}

#pragma region TakeDamageFunctions


void AEnemyBase::EnemyTakeHitCommonFunctionCalls()
{
	if (!widgetComp->IsVisible())
		widgetComp->SetVisibility(true);
	if (animInstance)
		animInstance->EnableHit();
	EnemyCheckStagger();
}
void AEnemyBase::EnemyCheckStagger()
{
	if (bCanBeStaggered) //Only stagger if the enemy is staggerable
	{
		animInstance->attackIndex = -1; //Stop your attack
		decisionComp->currentState = EEnemyState::STAGGERED;
		GetCharacterMovement()->StopMovementImmediately();
		FTimerHandle staggerTimeHandle;
		GetWorld()->GetTimerManager().SetTimer(staggerTimeHandle, decisionComp, &UDecisionComponentBase::ResetStateAfterStagger, decisionComp->staggerDelay, false);
	}
}

void AEnemyBase::EnemyTakeDamageNoAfflictions(float damage_)
{
	currentHP -= (damage_ + damage_ * currentAfflictions.enemyBrittleDamageIncrease +
		damage_ * currentAfflictions.enemyPoisonDamageIncrease +
		damage_ * currentAfflictions.enemyRootDamageIncrease);
	if (currentHP <= 0.0f && decisionComp->currentState != EEnemyState::DEAD)
	{
		decisionComp->currentState = EEnemyState::DEAD;
		if (animInstance)
			animInstance->EnableDeath();
		//Death animation
	}
	else if (decisionComp->currentState != EEnemyState::DEAD)
	{
		EnemyTakeHitCommonFunctionCalls();
	}
}
void AEnemyBase::EnemyTakeDamageBrittle(float damage_, float damageReduction_, float brittleDamageIncrease_)
{

	currentHP -= (damage_ + damage_ * currentAfflictions.enemyBrittleDamageIncrease + 
				  damage_ * currentAfflictions.enemyPoisonDamageIncrease + 
		          damage_ * currentAfflictions.enemyRootDamageIncrease);

	if(brittleDamageIncrease_ > 0.0f) //Ignore if minus, used with projectiles
		currentAfflictions.enemyBrittleDamageIncrease = brittleDamageIncrease_;
	if (currentHP <= 0.0f && decisionComp->currentState != EEnemyState::DEAD)
	{
		decisionComp->currentState = EEnemyState::DEAD;
		//Death
		if (animInstance)
			animInstance->EnableDeath();
	}
	else if(decisionComp->currentState != EEnemyState::DEAD)
	{
		EnemyTakeHitCommonFunctionCalls();

		if (currentAfflictions.enemyBrittleStack < currentAfflictions.enemyBrittleMaxStack)
		{
			currentAfflictions.enemyBrittleStack++;
			currentAfflictions.enemyBrittlePercentage += damageReduction_;
			//If this is the first time this affliction is afflicted, then make sure you call the check for it
			if (currentAfflictions.enemyBrittleStack == 1)
			{
				FTimerHandle brittleTimeHandle;
				GetWorld()->GetTimerManager().SetTimer(brittleTimeHandle, this, &AEnemyBase::EnemyCheckBrittle, currentAfflictions.enemyBrittleTimeSpan, false);
			}
		}
	}
}
void AEnemyBase::EnemyTakeDamageRoot(float damage_, float speedReduction_, float rootDamageIncrease_)
{
	currentHP -= (damage_ + damage_ * currentAfflictions.enemyBrittleDamageIncrease +
				  damage_ * currentAfflictions.enemyPoisonDamageIncrease +
				  damage_ * currentAfflictions.enemyRootDamageIncrease);

	if (rootDamageIncrease_ > 0.0f) //Ignore if minus, used with projectiles
		currentAfflictions.enemyRootDamageIncrease = rootDamageIncrease_;
	if (currentHP <= 0.0f && decisionComp->currentState != EEnemyState::DEAD)
	{
		decisionComp->currentState = EEnemyState::DEAD;
		//Death
		if (animInstance)
			animInstance->EnableDeath();
	}
	else if (decisionComp->currentState != EEnemyState::DEAD)
	{
		EnemyTakeHitCommonFunctionCalls();

		if (currentAfflictions.enemyRootStack < currentAfflictions.enemyRootMaxStack)
		{
			currentAfflictions.enemyRootStack++;
			currentAfflictions.enemyRootPercentage += speedReduction_;
			GetCharacterMovement()->MaxWalkSpeed = maxSpeed * currentAfflictions.enemyRootPercentage;
			//If this is the first time this affliction is afflicted, then make sure you call the check for it
			if (currentAfflictions.enemyRootStack == 1)
			{
				FTimerHandle rootTimeHandle;
				GetWorld()->GetTimerManager().SetTimer(rootTimeHandle, this, &AEnemyBase::EnemyCheckRoot, currentAfflictions.enemyRootTimeSpan, false);
			}
		}
	}
}
void AEnemyBase::EnemyTakeDamagePoison(float damage_, float poisonDamage_, float poisonDamageIncrease_)
{
	currentHP -= (damage_ + damage_ * currentAfflictions.enemyBrittleDamageIncrease +
		damage_ * currentAfflictions.enemyPoisonDamageIncrease +
		damage_ * currentAfflictions.enemyRootDamageIncrease);

	if (poisonDamageIncrease_ > 0.0f) //Ignore if minus, used with projectiles
		currentAfflictions.enemyPoisonDamageIncrease = poisonDamageIncrease_;
	if (currentHP <= 0.0f && decisionComp->currentState != EEnemyState::DEAD)
	{
		decisionComp->currentState = EEnemyState::DEAD;
		//Death
		if (animInstance)
			animInstance->EnableDeath();
	}
	else if (decisionComp->currentState != EEnemyState::DEAD)
	{
		EnemyTakeHitCommonFunctionCalls();

		if (currentAfflictions.enemyPoisonStack < currentAfflictions.enemyPoisonMaxStack)
		{
			currentAfflictions.enemyPoisonStack++;
			currentAfflictions.enemyPoisonedDamage = poisonDamage_ *100.0f;
			//If this is the first time this affliction is afflicted, then make sure you call the check for it
			if (currentAfflictions.enemyPoisonStack == 1)
			{
				FTimerHandle poisonTimeHandle;
				GetWorld()->GetTimerManager().SetTimer(poisonTimeHandle, this, &AEnemyBase::EnemyCheckPoison, currentAfflictions.enemyPoisonedTimeSpan, false);
				InflictPoisonDamage();

			}
		}
	}
}
void AEnemyBase::EnemyTakeDamageDefied(float damage_, float defiedPercentage_)
{
	currentHP -= (damage_ + damage_ * currentAfflictions.enemyBrittleDamageIncrease +
		damage_ * currentAfflictions.enemyPoisonDamageIncrease +
		damage_ * currentAfflictions.enemyRootDamageIncrease);
	if (currentHP <= 0.0f && decisionComp->currentState != EEnemyState::DEAD)
	{
		decisionComp->currentState = EEnemyState::DEAD;
		//Death
		if (animInstance)
			animInstance->EnableDeath();
	}
	else if (decisionComp->currentState != EEnemyState::DEAD)
	{
		EnemyTakeHitCommonFunctionCalls();

		if (!currentAfflictions.bDefied)
		{
			currentAfflictions.bDefied = true;
			currentAfflictions.defiedDamagePercentage = defiedPercentage_ * 100.0f;
		}
	}
}
void AEnemyBase::EnemyTakeDamageCursed(float damage_, float cursedDamage_)
{
	currentHP -= (damage_ + damage_ * currentAfflictions.enemyBrittleDamageIncrease +
		damage_ * currentAfflictions.enemyPoisonDamageIncrease +
		damage_ * currentAfflictions.enemyRootDamageIncrease);
	if (currentHP <= 0.0f && decisionComp->currentState != EEnemyState::DEAD)
	{
		decisionComp->currentState = EEnemyState::DEAD;
		//Death
		if (animInstance)
			animInstance->EnableDeath();
	}
	else if (decisionComp->currentState != EEnemyState::DEAD)
	{
		EnemyTakeHitCommonFunctionCalls();

		if (!currentAfflictions.bCursed)
		{
			currentAfflictions.bCursed = true;
			currentAfflictions.cursedDamage = cursedDamage_ * 100.0f; //All damages are written in percentage format

			FTimerHandle cursedTimeHandle;
			GetWorld()->GetTimerManager().SetTimer(cursedTimeHandle, this, &AEnemyBase::InflictCursedDamage, curseDamageInterval, false);
			InflictPoisonDamage();
		}
	}
}
#pragma endregion


#pragma region CheckAfflictionsFunctions
void AEnemyBase::EnemyCheckBrittle()
{
	//Every brittle time span, decrease the stack by 1 and when the stacks are gone, negate the effect
	currentAfflictions.enemyBrittleStack--;
	if (currentAfflictions.enemyBrittleStack <= 0)
	{
		currentAfflictions.enemyBrittleStack = 0;
		currentAfflictions.enemyBrittlePercentage = 0.0f;
		currentAfflictions.enemyBrittleDamageIncrease = 0.0f;
	}
	else
	{
		FTimerHandle brittleTimeHandle;
		GetWorld()->GetTimerManager().SetTimer(brittleTimeHandle, this, &AEnemyBase::EnemyCheckBrittle, currentAfflictions.enemyBrittleTimeSpan, false);
	}
}
void AEnemyBase::EnemyCheckRoot()
{
	currentAfflictions.enemyRootStack--;
	if (currentAfflictions.enemyRootStack <= 0)
	{
		currentAfflictions.enemyRootStack = 0;
		currentAfflictions.enemyRootPercentage = 0.0f;
		currentAfflictions.enemyRootDamageIncrease = 0.0f;
		GetCharacterMovement()->MaxWalkSpeed = maxSpeed;
	}
	else
	{
		FTimerHandle rootTimeHandle;
		GetWorld()->GetTimerManager().SetTimer(rootTimeHandle, this, &AEnemyBase::EnemyCheckRoot, currentAfflictions.enemyRootTimeSpan, false);
	}
}
void AEnemyBase::EnemyCheckPoison()
{
	currentAfflictions.enemyPoisonStack--;
	if (currentAfflictions.enemyPoisonStack <= 0)
	{
		currentAfflictions.enemyPoisonStack = 0;
		currentAfflictions.enemyPoisonedDamage = 0.0f;
		currentAfflictions.enemyPoisonDamageIncrease = 0.0f;
	}
	else
	{
		FTimerHandle rootTimeHandle;
		GetWorld()->GetTimerManager().SetTimer(rootTimeHandle, this, &AEnemyBase::EnemyCheckRoot, currentAfflictions.enemyRootTimeSpan, false);
	}
}
#pragma endregion

#pragma region InflictFunctions
void AEnemyBase::InflictPoisonDamage()
{
	if (currentAfflictions.enemyPoisonStack > 0)
	{
		currentHP -= currentAfflictions.enemyPoisonedDamage;
		if (delayedImpacts.Num() > 2)
			if (delayedImpacts[2] != nullptr)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), delayedImpacts[2], GetActorLocation(), FRotator::ZeroRotator);

		if (currentHP <= 0.0f && decisionComp->currentState != EEnemyState::DEAD)
		{
			decisionComp->currentState = EEnemyState::DEAD;
			//Death
			if (animInstance)
				animInstance->EnableDeath();
		}
		else if (decisionComp->currentState != EEnemyState::DEAD)
		{
			FTimerHandle poisonDamageHandle;
			GetWorld()->GetTimerManager().SetTimer(poisonDamageHandle, this, &AEnemyBase::InflictPoisonDamage, poisonDamageIntervals, false);
		}
	}
}

void AEnemyBase::InflictCursedDamage()
{
	if (currentAfflictions.bCursed)
	{
		currentHP -= currentAfflictions.cursedDamage;
		currentAfflictions.bCursed = false;
		currentAfflictions.cursedDamage = 0.0f;

		if (delayedImpacts.Num() > 1)
			if (delayedImpacts[1] != nullptr)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), delayedImpacts[1], GetActorLocation(), FRotator::ZeroRotator);

		if (currentHP <= 0.0f && decisionComp->currentState != EEnemyState::DEAD)
		{
			decisionComp->currentState = EEnemyState::DEAD;
			//Death
			if (animInstance)
				animInstance->EnableDeath();
		}
	}

}
#pragma endregion

void AEnemyBase::SetAIManager(AAIManager* aiMgrRef_)
{
	aiManager = aiMgrRef_;
}

void AEnemyBase::EndDeath() //Called from the animation instance
{
	if (aiManager)
		aiManager->HandleEnemyDeath();

	if (parentEnemy)
		parentEnemy->HandleEnemyChildDeath(this);

	Destroy();
}

float AEnemyBase::GetHPPercentage()
{
	return (currentHP / maxHP);
}

void AEnemyBase::BaseAttack()
{
	if (decisionComp->currentState == EEnemyState::ATTACKING)
	{
		if (currentAfflictions.bDefied)
		{
			if(delayedImpacts.Num() > 0)
				if (delayedImpacts[0] != nullptr)
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), delayedImpacts[0], GetActorLocation(), FRotator::ZeroRotator);

			currentHP -= currentAfflictions.defiedDamagePercentage;
			if (currentHP <= 0.0f && decisionComp->currentState != EEnemyState::DEAD)
			{
				animInstance->attackIndex = -1;
				decisionComp->currentState = EEnemyState::DEAD;
				//Death
				if (animInstance)
					animInstance->EnableDeath();
			}
			currentAfflictions.bDefied = false;
		}

		if (animInstance)
			animInstance->attackIndex = 0;
	}
}


void AEnemyBase::WakeUp()
{
	decisionComp->currentState = EEnemyState::PURSUING;
}

void AEnemyBase::SetParentEnemy(AEnemyBase* enm_)
{
	parentEnemy = enm_;
}