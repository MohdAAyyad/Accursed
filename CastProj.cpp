// Fill out your copyright notice in the Description page of Project Settings.


#include "CastProj.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "AI/EnemyBase.h"
#include "Audio/CustomAudioComponent.h"

// Sets default values
ACastProj::ACastProj()
{
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = root;

	detectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Detection sphere"));
	detectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	detectionSphere->SetGenerateOverlapEvents(true);
	detectionSphere->SetCollisionProfileName(TEXT("PlayerProj"));
	detectionSphere->SetupAttachment(root);

	damageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Damage sphere"));
	damageSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//sphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
	damageSphere->SetGenerateOverlapEvents(true);
	damageSphere->SetCollisionProfileName(TEXT("PlayerProj"));
	damageSphere->SetupAttachment(root);

	movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	movement->UpdatedComponent = root;
	movement->InitialSpeed = 700.0f;
	movement->MaxSpeed = 700.0f;
	movement->bRotationFollowsVelocity = true;
	movement->bShouldBounce = false;
	movement->ProjectileGravityScale = 0.0f;

	particles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particles"));
	particles->SetupAttachment(root);
	particles->SecondsBeforeInactive = 0.0f;

	audioComp = CreateDefaultSubobject<UCustomAudioComponent>(TEXT("Audio Component"));
	audioComp->SetupAttachment(root);

	targetLoc = FVector::ZeroVector;
	InitialLifeSpan = 2.0f;
}

// Called when the game starts or when spawned
void ACastProj::BeginPlay()
{
	Super::BeginPlay();

	if(damageSphere)
		damageSphere->OnComponentBeginOverlap.AddDynamic(this, &ACastProj::SphereApplyDamage);

	if(detectionSphere)
		detectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ACastProj::DetectTarget);
	
}

// Called every frame
void ACastProj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (targetLoc.Size() > 0.0f)
	{
		FVector Direction = targetLoc - GetActorLocation();
		if (Direction.Size() > 5.0f)
		{
			FVector normalizedDirection = Direction;
			FRotator rot_ = GetActorRotation();

			normalizedDirection.Normalize();
			rot_.Yaw = FMath::Lerp(GetActorRotation().Yaw, normalizedDirection.Rotation().Yaw, 0.05f);

			SetActorRotation(rot_); //Always face the player when pursuing
			movement->Velocity = GetActorForwardVector() * movement->MaxSpeed; //Always move in the direction you're facing
		}
	}
	*/

}

void ACastProj::SphereApplyDamage(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult& sweepResult_)
{
	if (otherActor_ != nullptr && otherComp_ != nullptr && otherActor_ != this)
	{
		AEnemyBase* enemy = Cast<AEnemyBase>(otherActor_);

		if (enemy)
		{
			switch (projStats.projAffliction)
			{
			case EAfflictions::NONE:
				enemy->EnemyTakeDamageNoAfflictions(projStats.projDamage);
				break;
			case EAfflictions::BRITTLE:
				enemy->EnemyTakeDamageBrittle(projStats.projDamage, projStats.afflictionPercentage, -1.0f); //-1 means no changes to the damage increase
				break;
			case EAfflictions::CURSE:
				enemy->EnemyTakeDamageCursed(projStats.projDamage, projStats.afflictionDamage);
				break;
			case EAfflictions::DEFY:
				enemy->EnemyTakeDamageDefied(projStats.projDamage, projStats.afflictionDamage);
				break;
			case EAfflictions::POISON:
				enemy->EnemyTakeDamagePoison(projStats.projDamage, projStats.afflictionDamage, -1.0f);
				break;
			case EAfflictions::ROOT:
				enemy->EnemyTakeDamageRoot(projStats.projDamage, projStats.afflictionPercentage, -1.0f);
				break;
			}
		}

		if (impact)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), impact, GetActorLocation(), FRotator::ZeroRotator);

		if (audioComp)
			audioComp->PlaySoundEffectAtIndex(0); 

		Destroy();
	}
}

void ACastProj::DetectTarget(UPrimitiveComponent* overlappedComponent_,
	AActor* otherActor_,
	UPrimitiveComponent* otherComp_,
	int32 otherBodyIndex_,
	bool bFromSweep_,
	const FHitResult& sweepResult_)
{
	if (otherActor_ != nullptr && otherComp_ != nullptr && otherActor_ != this)
	{
		AEnemyBase* enemy = Cast<AEnemyBase>(otherActor_);

		if (enemy)
		{
			targetLoc = enemy->GetActorLocation();
			FVector direction_ = targetLoc - GetActorLocation();
			direction_.Normalize();
			movement->Velocity = direction_ * movement->MaxSpeed;
			detectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); //No need to look for any other target
		}
	}
}

//Called by the player when spawning the projectile
void ACastProj::UpdateProjStats(float projDamage_, float afflictionDamage_, float percentageDamage_)
{
	projStats.projDamage = projDamage_;
	projStats.afflictionDamage = afflictionDamage_;
	projStats.afflictionPercentage = percentageDamage_;
}

//Called by the player when spawning the projectile
void ACastProj::UpdateVel(FVector direciton_)
{	
	SetActorRotation(direciton_.Rotation());
	movement->Velocity = GetActorForwardVector() * movement->MaxSpeed;
	targetLoc = GetActorLocation() + direciton_;

}
