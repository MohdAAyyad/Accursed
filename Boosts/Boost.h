// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ExternalFileReader/ExternalFileReader.h"
#include "../Interactable.h"
#include "Boost.generated.h"

UCLASS()
class TOPDOWNHS_API ABoost : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;

	UPROPERTY(EditAnywhere, Category = "Mesh")
		class UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere, Category = "Collision")
		class USphereComponent* sphere;

	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystemComponent* particles;

	UPROPERTY(EditAnywhere, Category = "Spawn Particles")
		class UParticleSystem* spawnParticles;

	UPROPERTY(EditAnywhere, Category = "Despawn Particles")
		UParticleSystem* despawnParticles;

	UPROPERTY(EditAnywhere, Category = "FileReader")
		UExternalFileReader* fileReader;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widget")
		class UWidgetComponent* widgetComp;

	class ATopDownHSCharacter* playerRef;
	class ATopDownHSPlayerController* ctrl;
	class ABoostManager* boostManager;

	UFUNCTION()
		void OnBeginOverlapWithPlayer(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult& sweepResult_);

	UFUNCTION()
	void OnEndOverlapWithPlayer(UPrimitiveComponent* overlappedComponent_,
		AActor* otherActor_,
		UPrimitiveComponent* otherComp_,
		int32 otherBodyIndex_);


public:	

	void SetBoostManagerRef(ABoostManager* ref_);

	UFUNCTION(BlueprintCallable)
		TArray<FBoostsTable> GetPossibleBoosts();

	void Interact() override;

	UFUNCTION(BlueprintCallable)
		void EquipBoost(FBoostsTable boost_); //Called from the UI

	void BoostDestroy();

};
