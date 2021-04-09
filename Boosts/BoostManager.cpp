// Fill out your copyright notice in the Description page of Project Settings.


#include "BoostManager.h"
#include "Boost.h"
#include "Engine/World.h"
#include "../TopDownHSCharacter.h"
#include "../AI/AIManager.h"

// Sets default values
ABoostManager::ABoostManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

}

// Called when the game starts or when spawned
void ABoostManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABoostManager::SpawnBoosts(int numberOfBoostsToSpawn_)
{
	//Called by the wave manager after a wave is finished
	TArray<int> prevIndexes = TArray<int>();
	ATopDownHSCharacter* playerRef_ = Cast<ATopDownHSCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	FVector spawnLoc_ = GetActorLocation();


	for (int i = 0; i < numberOfBoostsToSpawn_; ) //Make sure only one boost of each type is spawned
	{
		int index_ = FMath::RandRange(0, boostPool.Num() - 1);
	
		if (!prevIndexes.Contains(index_))
		{
			ABoost* boost_ = GetWorld()->SpawnActor<ABoost>(boostPool[index_], spawnLoc_, FRotator::ZeroRotator);
			spawnLoc_.X += 300.0f;
			boost_->SetBoostManagerRef(this);
			spawnedBoosts.Push(boost_);
			i++;
			prevIndexes.Push(index_);
		}

	}
}


void ABoostManager::SetAIManagerRef(AAIManager* ref_)
{
	aiManager = ref_;
}

void ABoostManager::BoostHasBeenEquipped()
{
	//Called by the boost that gets equipped
	for (int i = 0; i < spawnedBoosts.Num(); i++)
	{
		spawnedBoosts[i]->BoostDestroy();
	}

	spawnedBoosts.Empty();
}


