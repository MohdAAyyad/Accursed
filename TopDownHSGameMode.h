// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TopDownHSGameMode.generated.h"

UCLASS(minimalapi)
class ATopDownHSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATopDownHSGameMode();

	UFUNCTION(BlueprintCallable)
		void SwitchLevel(FName levelName_);

	UFUNCTION(BlueprintCallable)
		void LoadNextWaveMap();
};



