// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyProjectileBase.generated.h"

UCLASS()
class TOPDOWNHS_API AEnemyProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyProjectileBase();
	virtual void UpdateVel(FVector direciton_);
	float projDamage; //Updated by the enemy to match the decision component damage

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;
	UPROPERTY(EditAnywhere, Category = "Sphere")
		class USphereComponent* sphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		class UProjectileMovementComponent* movement;
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystemComponent* particles;
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystem* impact;
	UPROPERTY(EditAnywhere, Category = "Audio Component")
		class UCustomAudioComponent* impactAudioComponent;


	UFUNCTION()
	virtual void SphereApplyDamage(UPrimitiveComponent* overlappedComponent_,
		AActor* otherActor_,
		UPrimitiveComponent* otherComp_,
		int32 otherBodyIndex_,
		bool bFromSweep_,
		const FHitResult& sweepResult_);
};
