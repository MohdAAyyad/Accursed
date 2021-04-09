// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TopDownHSCharacter.h"
#include "CastProj.generated.h"

USTRUCT(BlueprintType)
struct FProjStats
{
	GENERATED_USTRUCT_BODY()
public:

	FProjStats()
	{
		projDamage = 20.0f;
		projAffliction = EAfflictions::NONE;
		afflictionDamage = 0.0f;
		afflictionPercentage = 0.0f;
	}

	UPROPERTY(EditAnywhere, Category = "Stats")
		float projDamage;
	UPROPERTY(EditAnywhere, Category = "Stats")
		EAfflictions projAffliction;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float afflictionDamage;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float afflictionPercentage;
};

UCLASS()
class TOPDOWNHS_API ACastProj : public AActor
{
	GENERATED_BODY()
	
public:	
	ACastProj();

protected:
	//The projectile detects enemies around it and attempts to rotate towards them

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;

	UPROPERTY(EditAnywhere, Category = "Detection Radius")
		class USphereComponent* detectionSphere;
	UPROPERTY(EditAnywhere, Category = "Damage sphere")
		USphereComponent* damageSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		class UProjectileMovementComponent* movement;
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystemComponent* particles;
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystem* impact;
	UPROPERTY(EditAnywhere, Category = "Stats")
		FProjStats projStats;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Audio")
		class UCustomAudioComponent* audioComp;

	FVector targetLoc;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void SphereApplyDamage(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult& sweepResult_);

	UFUNCTION()
		void DetectTarget(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult& sweepResult_);





public:	
	void UpdateProjStats(float projDamage_, float afflictionDamage_, float percentageDamage_);
	void UpdateVel(FVector direciton_);

};
