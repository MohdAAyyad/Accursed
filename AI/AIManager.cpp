// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"
#include "Components/WidgetComponent.h"
#include "EnemyBase.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "../Boosts/BoostManager.h"
#include "../TopDownHSCharacter.h"
#include "../Audio/AudioManager.h"
#include "../Intermediate.h"

// Sets default values
AAIManager::AAIManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	widgetComp->SetupAttachment(root);

	fileReader = CreateDefaultSubobject<UExternalFileReader>(TEXT("File Reader"));

	numOfEnemiesDefeatedInCurrentWave = numOfEnemiesSpawned = 0;
	waveInfo = FWaveInfo();
}

// Called when the game starts or when spawned
void AAIManager::BeginPlay()
{
	Super::BeginPlay();

	//Called from the player after the state is loaded so commented out here
	//FTimerHandle firstWaveTimeHandle;
	//GetWorld()->GetTimerManager().SetTimer(firstWaveTimeHandle, this, &AAIManager::NextWave, 0.2f, false);

	if (widgetComp)
		if (widgetComp->GetUserWidgetObject())
			widgetComp->GetUserWidgetObject()->AddToViewport();

	ATopDownHSCharacter* player_ = Cast<ATopDownHSCharacter>( GetWorld()->GetFirstPlayerController()->GetPawn());
	if (player_)
	{
		player_->SetAIManagerRef(this);
	}
	
}

void AAIManager::NextWave()
{

	numOfEnemiesDefeatedInCurrentWave = 0;
	numOfEnemiesSpawned = 0;
	Intermediate::GetInstance()->currentWaveID++;
	int waveId_ = Intermediate::GetInstance()->currentWaveID;
	if (waveId_ != 0)
	{
		if (audioManager)
			audioManager->WaveHasEnded(waveId_ % 4 == 0);
	}

	if (fileReader)
		waveInfo = fileReader->GetWaveInfoByID(0, waveId_);

	if (waveId_ > 0)
	{
		FTimerHandle nextWaveTimeHandle;
		GetWorld()->GetTimerManager().SetTimer(nextWaveTimeHandle, this, &AAIManager::SpawnEnemies, 0.2f, false);
	}
	/*else //Wave zero --> Spawn one boost
	{
		if (boostManagerRef)
			boostManagerRef->SpawnBoosts(1);
	}
	*/
	if (boostManagerRef)
		boostManagerRef->SetAIManagerRef(this);
	


}

void AAIManager::SpawnEnemies()
{
	int waveId_ = Intermediate::GetInstance()->currentWaveID;
	if (waveId_ % 3 == 0) //Every five waves, have the demon say sth. First wave as well
	{
		if (audioManager)
			audioManager->PlayDemonVoiceLine(2);
	}
	UE_LOG(LogTemp, Warning, TEXT("numOfEnemiesSpawned = %d and  waveInfo.numOfEnemiesToSpawnl = %d"), numOfEnemiesSpawned, waveInfo.numOfEnemiesToSpawn);
	//Spawn the number of enemies designtated by the wave info
	if (numOfEnemiesSpawned <= waveInfo.numOfEnemiesToSpawn)
	{
		TArray<int> prevIndexes = TArray<int>();

		//We want to spawn a number of enemies per interval (4 at a time for example)
		for (int i = 0; i < waveInfo.numOfEnemiesPerInterval;)
		{
			UE_LOG(LogTemp, Warning, TEXT("Entering spawn loop"));
			int locIndex_ = FMath::RandRange(0, enemySpawnLocations.Num() - 1);
			if (!prevIndexes.Contains(locIndex_))
			{
				//Spawn the enemies as indicated by the min and max indexes
				int enemyIndex_ = FMath::RandRange(waveInfo.enemyMinIndex, waveInfo.enemyMaxIndex);

				if (enemyIndex_ >= 0 && enemyIndex_ < enemies.Num())
				{
					AEnemyBase* enemy_ = GetWorld()->SpawnActor<AEnemyBase>(enemies[enemyIndex_], enemySpawnLocations[locIndex_], FRotator::ZeroRotator);
					if (enemy_)
					{
						enemy_->SetAIManager(this);
						prevIndexes.Push(locIndex_);
						numOfEnemiesSpawned++;
						i++;
						UE_LOG(LogTemp, Warning, TEXT("Spawned the damn thing"));
					}
				}
			}
		}
	}
}

AEnemyBase* AAIManager::SpawnEnemiesForParentEnemy(int enemyIndex_)
{

	//Called by bosses
	int locIndex_ = FMath::RandRange(0, enemySpawnLocations.Num() - 1);
		
	if (enemyIndex_ >= 0 && enemyIndex_ < enemies.Num())
	{
		 return (GetWorld()->SpawnActor<AEnemyBase>(enemies[enemyIndex_], enemySpawnLocations[locIndex_], FRotator::ZeroRotator));
	}

	return nullptr;
}

void AAIManager::HandleEnemyDeath()
{
	numOfEnemiesDefeatedInCurrentWave++;

	//UE_LOG(LogTemp, Warning, TEXT("numOfEnemiesDefeatedInCurrentWave = %d and  waveInfo.numOfEnemiesPerInterval = %d"), numOfEnemiesDefeatedInCurrentWave, waveInfo.numOfEnemiesPerInterval);
	if (numOfEnemiesDefeatedInCurrentWave >= waveInfo.numOfEnemiesToSpawn) //End wave
	{
		if (boostManagerRef)
			boostManagerRef->SpawnBoosts(waveInfo.numOfBoostsToSpawn);

		if (audioManager)
			audioManager->PlayBoostPiece();
	}
	else if(numOfEnemiesDefeatedInCurrentWave == waveInfo.numOfEnemiesPerInterval) //The interval has concluded so move to the next interval
	{
		//UE_LOG(LogTemp, Warning, TEXT("Gonna spawn the rest"));
		FTimerHandle nextIntervalTimeHandle;
		GetWorld()->GetTimerManager().SetTimer(nextIntervalTimeHandle, this, &AAIManager::SpawnEnemies, 0.2f, false);
	}
}

FVector AAIManager::GetRandomSpawnLoc() //Used by enemies that teleport around the map
{
	int locIndex_ = FMath::RandRange(0, enemySpawnLocations.Num() - 1);

	return enemySpawnLocations[locIndex_];
}

