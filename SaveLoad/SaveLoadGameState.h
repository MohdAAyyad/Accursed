// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TDSSaveGame.h"
#include "SaveLoadGameState.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API ASaveLoadGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ASaveLoadGameState();
	void SetPlayerRef(class ATopDownHSCharacter* ref_);
	void AddBoost(FBoostsTable boost_);

	UFUNCTION(BlueprintCallable)
		void SaveCurrentGameState();

	UFUNCTION(BlueprintCallable)
		void ResetCurrentGameState();

	void FinishSaving(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	UFUNCTION(BlueprintCallable)
		void LoadData();

	void UpdatePlayer(const FString& slotName_, const int32 userIndex_, USaveGame* saveRef_);

protected:
	TArray<FBoostsTable> acquiredBoosts;
	ATopDownHSCharacter* playerRef;
	
};
