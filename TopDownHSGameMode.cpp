// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownHSGameMode.h"
#include "TopDownHSPlayerController.h"
#include "TopDownHSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Intermediate.h"

ATopDownHSGameMode::ATopDownHSGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATopDownHSPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Custom/Player/Blueprints/Player")); //D:/Unreal/OGJam/TopDownHS/Content/Custom/Player/Blueprints/Player.uasset
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ATopDownHSGameMode::SwitchLevel(FName levelName_)
{
	UGameplayStatics::OpenLevel(GetWorld(), levelName_);
}

void ATopDownHSGameMode::LoadNextWaveMap()
{
	int waveId_ = Intermediate::GetInstance()->currentWaveID + 1;
	FString mapName = "/Game/beffio/Medieval_Kingdom/Scenes/MapScenes/Map_" + FString::FromInt(waveId_);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *mapName);
	FName levelName_ = FName(*mapName);
	SwitchLevel(levelName_);
}