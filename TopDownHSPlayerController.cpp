// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownHSPlayerController.h"
#include "TopDownHSCharacter.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "TopDownHSCharacter.h"
#include "Engine/World.h"

ATopDownHSPlayerController::ATopDownHSPlayerController()
{
	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	playerCharacter = nullptr;
}

void ATopDownHSPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ATopDownHSPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
}

void ATopDownHSPlayerController::SetPlayerRef(ATopDownHSCharacter* ref_)
{
	playerCharacter = ref_;

	if (playerCharacter)
	{
	 //Movement binding

		InputComponent->BindAxis("MoveForward", playerCharacter, &ATopDownHSCharacter::HandleMoveForward);
		InputComponent->BindAxis("MoveRight", playerCharacter, &ATopDownHSCharacter::HandleMoveRight);
		InputComponent->BindAction("Attack", IE_Pressed, playerCharacter, &ATopDownHSCharacter::HandleAttack);
		InputComponent->BindAction("Special", IE_Pressed, playerCharacter, &ATopDownHSCharacter::HandleSpecial);
		InputComponent->BindAction("Dash", IE_Pressed, playerCharacter, &ATopDownHSCharacter::HandleDash);
		InputComponent->BindAction("Cast", IE_Pressed, playerCharacter, &ATopDownHSCharacter::HandleCast);
		InputComponent->BindAction("Interact", IE_Pressed, playerCharacter, &ATopDownHSCharacter::InteractWithInteractable);
	}
}

ATopDownHSCharacter* ATopDownHSPlayerController::GetPlayerRef()
{
	return playerCharacter;
}