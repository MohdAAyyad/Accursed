// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../ExternalFileReader/FBoostsTable.h"
#include "TDSSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API UTDSSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UTDSSaveGame();
	UPROPERTY(VisibleAnywhere, Category = Boosts)
		TArray<FBoostsTable> acquiredBoosts;
	UPROPERTY(VisibleAnywhere, Category = Waves)
		int waveID;
	
};
