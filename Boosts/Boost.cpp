// Fill out your copyright notice in the Description page of Project Settings.


#include "Boost.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "../TopDownHSCharacter.h"
#include "../TopDownHSPlayerController.h"
#include "BoostManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "../SaveLoad/SaveLoadGameState.h"

// Sets default values
ABoost::ABoost()
{
	PrimaryActorTick.bCanEverTick = false;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(root);
	mesh->SetGenerateOverlapEvents(true);
	mesh->SetCollisionProfileName("OverlapAll");

	sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	sphere->SetupAttachment(root);
	sphere->SetGenerateOverlapEvents(true);
	sphere->SetCollisionProfileName("OverlapAll");

	particles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	particles->SetupAttachment(root);

	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	widgetComp->SetupAttachment(root);

	fileReader = CreateDefaultSubobject<UExternalFileReader>(TEXT("File Reader"));

	playerRef = nullptr;
	boostManager = nullptr;

}

// Called when the game starts or when spawned
void ABoost::BeginPlay()
{
	Super::BeginPlay();
	
	ctrl = Cast<ATopDownHSPlayerController>(GetWorld()->GetFirstPlayerController());


	if (spawnParticles)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), spawnParticles, GetActorLocation(), GetActorRotation());

	if (sphere)
	{
		sphere->OnComponentBeginOverlap.AddDynamic(this, &ABoost::OnBeginOverlapWithPlayer);
		sphere->OnComponentEndOverlap.AddDynamic(this, &ABoost::OnEndOverlapWithPlayer);
	}
	
}


void ABoost::SetBoostManagerRef(ABoostManager* ref_)
{
	boostManager = ref_;
}
TArray<FBoostsTable> ABoost::GetPossibleBoosts()
{
	TArray<FBoostsTable> boosts_ = fileReader->GetRandomUnaquiredBoosts(0, 1); //Get up to 3 unacquired boosts
	
	for (int i = 0; i < boosts_.Num(); i++) //Determine the category of the each boost and update the value accordingly
	{
		int catChance_ = FMath::RandRange(0, 101);

		if (catChance_ <= 10)
		{
			boosts_[i].category = 3;
			boosts_[i].boostValue += 0.4f; //Elite
		}
		else if (catChance_ <= 25)
		{
			boosts_[i].category = 2;
			boosts_[i].boostValue += 0.3f; //Rare
		}
		else if (catChance_ <= 50)
		{ 
			boosts_[i].category = 1;
			boosts_[i].boostValue += 0.2f; //Uncommon
		}
		else
		{
			boosts_[i].category = 0; //Common
		}
	}
	
	return boosts_;
}


void ABoost::Interact()
{
	if (widgetComp)
	{
		if (widgetComp->GetUserWidgetObject())
		{
			if(!widgetComp->GetUserWidgetObject()->IsInViewport())
					widgetComp->GetUserWidgetObject()->AddToViewport();
		}
	}
}


void ABoost::EquipBoost(FBoostsTable boost_) //Called from the UI
{
	if (playerRef)
		playerRef->EquipBoost(boost_);

	ASaveLoadGameState* gameState_ = Cast< ASaveLoadGameState>(GetWorld()->GetGameState());

	
	if (widgetComp)
	{
		if (widgetComp->GetUserWidgetObject())
		{
			widgetComp->GetUserWidgetObject()->RemoveFromViewport();
		}
	}

	if (boostManager)
		boostManager->BoostHasBeenEquipped();

	if (gameState_)
	{
		gameState_->AddBoost(boost_);
		gameState_->SaveCurrentGameState();
	}
}

void ABoost::BoostDestroy()
{
	//Spawn a particle effect then die
	if (despawnParticles)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), despawnParticles, GetActorLocation(), GetActorRotation());

	Destroy();
}


void ABoost::OnBeginOverlapWithPlayer(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult& sweepResult_)
{
	if (otherActor_ != nullptr && otherComp_ != nullptr && otherActor_ != this)
	{
		if (!playerRef)
		{
			playerRef = Cast<ATopDownHSCharacter>(otherActor_);

			if (playerRef)
			{
				playerRef->UpdateInteractableRef(this);
			}
		}
	}
}

void ABoost::OnEndOverlapWithPlayer(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_)
{
	if (otherActor_ != nullptr && otherComp_ != nullptr && otherActor_ != this)
	{
		ATopDownHSCharacter* player_ = Cast<ATopDownHSCharacter>(otherActor_);

		if (player_)
		{
			playerRef->UpdateInteractableRef(nullptr);
			playerRef = nullptr;
		}
	}
}

