// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoostManager.generated.h"

UCLASS()
class TOPDOWNHS_API ABoostManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoostManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;

	UPROPERTY(EditAnywhere, Category = "Boosts")
		TArray<TSubclassOf<class ABoost>> boostPool;

	TArray< ABoost*> spawnedBoosts;

	class AAIManager* aiManager;
public:	

	void SpawnBoosts(int numberOfBoostsToSpawn_);
	void BoostHasBeenEquipped();
	void SetAIManagerRef(AAIManager* ref_);
};
