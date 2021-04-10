// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../TopDownHSCharacter.h"
#include "EnemyBase.generated.h"

USTRUCT(BlueprintType)
struct FEnemyAfflictions
{
	GENERATED_USTRUCT_BODY()
public:
	FEnemyAfflictions()
	{
		cursedDamage = defiedDamagePercentage = enemyPoisonedDamage = enemyRootPercentage = enemyBrittlePercentage = 
			enemyBrittleDamageIncrease = enemyPoisonDamageIncrease = enemyRootDamageIncrease = 0.0f;
		bCursed = bPoisoned = bDefied = false;
		enemyPoisonStack = enemyRootStack =  enemyBrittleStack = 0;
		enemyBrittleTimeSpan = 5.0f; //Check every 5 seconds if the brittle stack is larger than 0 and if not, reduce the percentage back to zero
		enemyRootTimeSpan = 5.0f;
		enemyPoisonedTimeSpan = 3.0f;
		enemyRootMaxStack = enemyBrittleMaxStack = 5;
		enemyPoisonMaxStack = 6;
	}

	float enemyBrittlePercentage;
	float enemyRootPercentage;
	float enemyPoisonedDamage;
	float defiedDamagePercentage;
	float cursedDamage;

	int enemyBrittleStack;
	int enemyBrittleMaxStack;
	int enemyRootStack;
	int enemyRootMaxStack;
	int enemyPoisonStack;
	int enemyPoisonMaxStack;

	bool bDefied;
	bool bPoisoned;
	bool bCursed;

	float enemyBrittleTimeSpan;
	float enemyRootTimeSpan;
	float enemyPoisonedTimeSpan;

	float enemyBrittleDamageIncrease;
	float enemyPoisonDamageIncrease;
	float enemyRootDamageIncrease;


};

UCLASS()
class TOPDOWNHS_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		float maxHP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		float currentHP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		float damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		bool bCanBeStaggered;

	UPROPERTY(EditAnywhere, Category = "Decisions")
		class UDecisionComponentBase* decisionComp;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widget")
		class UWidgetComponent* widgetComp;

	UPROPERTY(EditAnywhere, Category = "Delayed Imapcts")
		TArray<class UParticleSystem*> delayedImpacts; //Such as defied and cursed

	float maxSpeed;

	float poisonDamageIntervals;
	float curseDamageInterval;

	class UEnemyAnimInstance* animInstance;

	FEnemyAfflictions currentAfflictions;

	class AAIManager* aiManager;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	virtual void EnemyTakeHitCommonFunctionCalls();
	void EnemyCheckStagger();
	void EnemyCheckBrittle();
	void EnemyCheckRoot();
	void EnemyCheckPoison();


	void InflictPoisonDamage();
	void InflictCursedDamage();

	UFUNCTION(BlueprintCallable)
		virtual void EndDeath();

	UFUNCTION(BlueprintCallable)
		float GetHPPercentage();


	UFUNCTION(BlueprintCallable)
		virtual void ActivateAttack(int index_) {}; //Called from the animation to activate the attack. Overriden in children
	UFUNCTION(BlueprintCallable)
		virtual void EndAttack(int index_) {};//Called from the animation to end the attack. Overriden in children
	UFUNCTION(BlueprintCallable)
		virtual void ReactToHit() {}; //Called from the animation when getting hit. Overriden in children

	UFUNCTION(BlueprintCallable)
		virtual void WakeUp();

	AEnemyBase* parentEnemy; //Used when enemies should die when another enemy dies;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void EnemyTakeDamageNoAfflictions(float damage_);
	void EnemyTakeDamageBrittle(float damage_,float damageReduction_, float brittleDamageIncrease_);
	void EnemyTakeDamageRoot(float damage_, float speedReduction_, float rootDamageIncrease_);
	void EnemyTakeDamagePoison(float damage_, float poisonDamage_, float poisonDamageIncrease_);
	void EnemyTakeDamageDefied(float damage_, float defiedPercentage_);
	void EnemyTakeDamageCursed(float damage_, float cursedDamage_);

	void SetAIManager(AAIManager* aiMgrRef_);

	void IncreaseEnemyHealth(float hp_);

	UFUNCTION(BlueprintCallable)
		virtual void BaseAttack(); //Used by common enemies who only have one attack

	void SetParentEnemy(AEnemyBase* enm_);
	virtual void HandleEnemyChildDeath(AEnemyBase* enm_) {}; //Overriden when needed






};
