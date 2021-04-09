// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FWaveInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TOPDOWNHS_API FWaveInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FWaveInfo()
	{
		id = 0;
		numOfBoostsToSpawn = numOfEnemiesToSpawn = numOfEnemiesPerInterval = enemyMinIndex = enemyMaxIndex = 0;
	}

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int id;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int numOfBoostsToSpawn;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int numOfEnemiesToSpawn;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int numOfEnemiesPerInterval; //Not all the enemies will be spawned at the same time
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int enemyMinIndex; //The min index of the enemies to spawn from the array
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int enemyMaxIndex;
};
