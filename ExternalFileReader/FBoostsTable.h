// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FBoostsTable.generated.h"

/**
 * public FTableRowBase
 */
USTRUCT(BlueprintType)
struct TOPDOWNHS_API FBoostsTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FBoostsTable()
	{
		id = activeOrPassive = affliction = boostIndex = hasBeenAcquired = category = 0;
		boostValue = 0.0f;
		name = description = "";
	}

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int id;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FString name; 
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FString description;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int activeOrPassive; //0 active, 1 passive
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int affliction; //Casted later on to the enum
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int boostIndex; //Each index matches a property in the passive or active boosts
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float boostValue;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int hasBeenAcquired; //0 available, 1 acquired
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		int category; // 0 Common, 1 Uncommon, 2 Rare, 3 Elite, 4 Duo
};
