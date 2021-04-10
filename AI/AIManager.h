// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ExternalFileReader/ExternalFileReader.h"
#include "AIManager.generated.h"

UCLASS()
class TOPDOWNHS_API AAIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;

	UPROPERTY(EditAnywhere, Category = "FileReader")
		UExternalFileReader* fileReader;

	UPROPERTY(EditAnywhere, Category = "Enemies")
		TArray<TSubclassOf<class AEnemyBase>> enemies;

	UPROPERTY(EditAnywhere, Category = "Spawning")
		TArray<FVector> enemySpawnLocations;

	UPROPERTY(EditAnywhere, Category = "Boost Manager")
		class ABoostManager* boostManagerRef;
	UPROPERTY(EditAnywhere, Category = "Audio Manager")
		class AAudioManager* audioManager;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widget")
		class UWidgetComponent* widgetComp;

	UPROPERTY(BlueprintReadOnly)
		FWaveInfo waveInfo;

	UPROPERTY(EditAnywhere, Category = "Enemies")
		float enemyHealthIncrease; //Enemy health increases every 2 waves by this value

	float baseEnemyHealthIncrease; //The actual value passed on to increase the enemy health


	int numOfEnemiesDefeatedInCurrentWave;
	int numOfEnemiesSpawned;


	void SpawnEnemies();


public:

	void NextWave();
	void HandleEnemyDeath();
	AEnemyBase* SpawnEnemiesForParentEnemy(int enemyIndex_);//The same as spawn enemies 
	FVector GetRandomSpawnLoc();

};
