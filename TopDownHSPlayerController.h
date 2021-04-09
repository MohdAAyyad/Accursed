// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TopDownHSPlayerController.generated.h"

UCLASS()
class ATopDownHSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATopDownHSPlayerController();

protected:
	class ATopDownHSCharacter* playerCharacter;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface
public:

	void SetPlayerRef(ATopDownHSCharacter* ref_);
	ATopDownHSCharacter* GetPlayerRef();
};

//TODO
/*
//Create interactable interface and have boosts inherit from it.
// When boosts collide with player, they should pass themselves to the player
// Pressing the interact button should add the boost UI to view port
// Add UI functionality to get the boosts
// Update the animations based on the boosts
// Add the ability to randomize boosts and make sure a boost is not presented when the player already owns it 
// To do that, get an array of all the available boosts and pick up to 3 of them at random
// Add a max number for each boost type and if that number if met, do not spawn boosts of that type
*/


