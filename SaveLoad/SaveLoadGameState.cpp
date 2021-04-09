// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveLoadGameState.h"
#include "SaveGameSystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "../TopDownHSGameMode.h"
#include "../TopDownHSCharacter.h"
#include "../Intermediate.h"

ASaveLoadGameState::ASaveLoadGameState() : AGameStateBase()
{
	acquiredBoosts = TArray<FBoostsTable>();
}

void ASaveLoadGameState::SetPlayerRef(class ATopDownHSCharacter* ref_)
{
	playerRef = ref_;
}
void ASaveLoadGameState::AddBoost(FBoostsTable boost_) //Called by the player once a boost is acquired
{
	acquiredBoosts.Push(boost_);
}
void ASaveLoadGameState::SaveCurrentGameState() //Called by the player when switching scenes
{
	//Save the data and when you're done tell the game mode to go to the next map
	UTDSSaveGame* saveInstance_ = Cast<UTDSSaveGame>(UGameplayStatics::CreateSaveGameObject(UTDSSaveGame::StaticClass()));
	if (saveInstance_)
	{
		saveInstance_->acquiredBoosts = acquiredBoosts;
		saveInstance_->waveID = Intermediate::GetInstance()->currentWaveID;
		FAsyncSaveGameToSlotDelegate saveDelegate;
		saveDelegate.BindUObject(this, &ASaveLoadGameState::FinishSaving);
		UGameplayStatics::AsyncSaveGameToSlot(saveInstance_, TEXT("Save0"), 0, saveDelegate);
	}
}

void ASaveLoadGameState::ResetCurrentGameState() //Called when initiating a new game
{
	acquiredBoosts = TArray<FBoostsTable>();
	Intermediate::GetInstance()->currentWaveID = 0;
	SaveCurrentGameState();
}
void ASaveLoadGameState::FinishSaving(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	//Called after saving is done
	//ATopDownHSGameMode* gm_ = Cast<ATopDownHSGameMode>(GetWorld()->GetAuthGameMode());
//	if (gm_)
	//{
	//	gm_->LoadNextWaveMap();
	//}

	if (playerRef)
		playerRef->FinishedLoadingData();
}
void ASaveLoadGameState::LoadData()
{
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindUObject(this, &ASaveLoadGameState::UpdatePlayer);

	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("Save0"), 0, LoadedDelegate);
}

void ASaveLoadGameState::UpdatePlayer(const FString& slotName_, const int32 userIndex_, USaveGame* saveRef_) //Called by the load data delegate
{
	//Called after loading in the data. Equips the saved boosts.
	UTDSSaveGame* loadInstance_ = Cast<UTDSSaveGame>(saveRef_);
	if (loadInstance_)
	{
		Intermediate::GetInstance()->currentWaveID = loadInstance_->waveID;
		acquiredBoosts = loadInstance_->acquiredBoosts;
		if (playerRef)
		{
			for (int i = 0; i < acquiredBoosts.Num(); i++)
			{
				playerRef->EquipBoost(acquiredBoosts[i]);
			}

			playerRef->FinishedLoadingData(); //Ready to roll
		}

	}
}