// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "../TopDownHSCharacter.h"

// Sets default values
AEnemyProjectileBase::AEnemyProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = root;

	sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//sphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
	sphere->SetGenerateOverlapEvents(true);
	sphere->SetCollisionProfileName(TEXT("EnemyProj"));
	sphere->SetupAttachment(root);

	movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	movement->UpdatedComponent = root;
	movement->InitialSpeed = 150.0f;
	movement->MaxSpeed = 150.0f;
	movement->bRotationFollowsVelocity = true;
	movement->bShouldBounce = false;

	projDamage = 0.0f;

	particles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	particles->SetupAttachment(root);

}

// Called when the game starts or when spawned
void AEnemyProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	if(sphere)
		sphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectileBase::SphereApplyDamage);
	
}

void AEnemyProjectileBase::SphereApplyDamage(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult& sweepResult_)
{
	if (otherActor_ != nullptr && otherComp_ != nullptr && otherActor_ != this)
	{
		//If you overlap with anything, die. If you overlap with the player, make sure they take damage first.
		ATopDownHSCharacter* player_ = Cast<ATopDownHSCharacter>(otherActor_);
		if (player_)
		{
			player_->PlayerTakeDamage(projDamage);
		}

		if (impact)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impact, GetActorLocation(), FRotator::ZeroRotator);

		Destroy();
	}
}

void AEnemyProjectileBase::UpdateVel(FVector direciton_)
{
	movement->Velocity = direciton_ * movement->MaxSpeed;
}

