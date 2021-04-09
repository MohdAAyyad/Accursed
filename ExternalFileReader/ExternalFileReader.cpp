// Fill out your copyright notice in the Description page of Project Settings.


#include "ExternalFileReader.h"

// Sets default values for this component's properties
UExternalFileReader::UExternalFileReader()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UExternalFileReader::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

FBoostsTable UExternalFileReader::GetBoostByID(int tableIndex_, int id_)
{
	static const FString contextString(TEXT("Trying to get the boost from the table based on ID"));
	TArray<FName> rowNames;
	FBoostsTable boost;
	if (tableIndex_ >= 0 && tableIndex_ < tables.Num())
	{
		rowNames = tables[tableIndex_]->GetRowNames();

		for (auto n : rowNames)
		{
			FBoostsTable* row = tables[tableIndex_]->FindRow<FBoostsTable>(n, contextString, true);

			if (row->id == id_) //Is this the correct boost?
			{
				boost = *row;
				return boost;

			}

		}
	}

	return boost;
}


TArray<FBoostsTable> UExternalFileReader::GetRandomUnaquiredBoosts(int activeTableIndex_, int passiveTableIndex_)
{
	static const FString contextString(TEXT("Trying to get the Weapons from the table based on ID"));
	TArray<FBoostsTable> unacquiredBoosts;
	TArray<FBoostsTable> resultBoosts;
	if (activeTableIndex_ >= 0 && activeTableIndex_ < tables.Num()) //Get all unacquired active boosts
	{
		TArray<FName> rowNames;
		rowNames = tables[activeTableIndex_]->GetRowNames(); //Get


		for (auto n : rowNames)
		{
			FBoostsTable* row = tables[activeTableIndex_]->FindRow<FBoostsTable>(n, contextString, true);

			if (row->hasBeenAcquired == 0) //Make sure the boost picked has not been acquired yet
			{
				unacquiredBoosts.Push(*row);

			}

		}
	}

	if (passiveTableIndex_ >= 0 && passiveTableIndex_ < tables.Num()) //Get all unacquired passive boosts
	{
		TArray<FName> rowNames;
		rowNames = tables[passiveTableIndex_]->GetRowNames();


		for (auto n : rowNames)
		{
			FBoostsTable* row = tables[passiveTableIndex_]->FindRow<FBoostsTable>(n, contextString, true);

			if (row->hasBeenAcquired == 0) //Make sure the boost picked has not been acquired yet
			{
				unacquiredBoosts.Push(*row);

			}

		}
	}

	if (unacquiredBoosts.Num() > 3) //If there are more than 3 boosts available, make 3 available for choosing
	{
		for (int i = 0; i < 3; i++)
		{
			//Pick a boost at random and push to the result
			int index_ = FMath::RandRange(0, unacquiredBoosts.Num() - 1);
			resultBoosts.Push(unacquiredBoosts[index_]);
			unacquiredBoosts.RemoveAt(index_);
		}
	}
	else
	{
		for (int i = 0; i < unacquiredBoosts.Num(); i++)
		{
			//Pick a boost at random and push to the result
			int index_ = FMath::RandRange(0, unacquiredBoosts.Num() - 1);
			resultBoosts.Push(unacquiredBoosts[index_]);
			unacquiredBoosts.RemoveAt(index_);
		}
	}

	return resultBoosts;
}


FWaveInfo UExternalFileReader::GetWaveInfoByID(int tableIndex_, int id_)
{
	static const FString contextString(TEXT("Trying to get the wave info from the table based on ID"));
	TArray<FName> rowNames;
	FWaveInfo wave;
	if (tableIndex_ >= 0 && tableIndex_ < tables.Num())
	{
		rowNames = tables[tableIndex_]->GetRowNames();

		for (auto n : rowNames)
		{
			FWaveInfo* row = tables[tableIndex_]->FindRow<FWaveInfo>(n, contextString, true);

			if (row->id == id_) //Is this the correct boost?
			{
				wave = *row;
				return wave;

			}

		}
	}

	return wave;
}

void UExternalFileReader::AcquireBoost(int tableIndex_, int id_)
{
	static const FString contextString(TEXT("Trying to set the boost to acquired"));
	TArray<FName> rowNames;
	if (tableIndex_ >= 0 && tableIndex_ < tables.Num())
	{
		rowNames = tables[tableIndex_]->GetRowNames();

		for (auto n : rowNames)
		{
			FBoostsTable* row = tables[tableIndex_]->FindRow<FBoostsTable>(n, contextString, true);

			if (row->id == id_) //Is this the correct boost?
			{
				row->hasBeenAcquired = 1;
				break;

			}

		}
	}
}
