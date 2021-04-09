// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FBoostsTable.h"
#include "FWaveInfo.h"
#include "ExternalFileReader.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOPDOWNHS_API UExternalFileReader : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UExternalFileReader();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TArray<UDataTable*> tables;

public:	

	FBoostsTable GetBoostByID(int tableIndex_, int id_);
	TArray<FBoostsTable> GetRandomUnaquiredBoosts(int activeTableIndex_, int passiveTableIndex_);
	void AcquireBoost(int tableIndex_, int id_);

	FWaveInfo GetWaveInfoByID(int tableIndex_, int id_);

		
};
