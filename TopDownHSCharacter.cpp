// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownHSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "TopDownHSPlayerController.h"
#include "Animations/PlayerAnimInstance.h"
#include "Components/BoxComponent.h"
#include "AI/EnemyBase.h"
#include "PCameraShake.h"
#include "AI/AIManager.h"
#include "TimerManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CastProj.h"
#include "Audio/CustomAudioComponent.h"
#include "Intermediate.h"
#include "SaveLoad/SaveLoadGameState.h"
#include "Audio/AudioManager.h"

ATopDownHSCharacter::ATopDownHSCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	attackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Attack Box"));
	attackBox->SetupAttachment(RootComponent);
	attackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	specialBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Special Box"));
	specialBox->SetupAttachment(RootComponent);
	specialBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	dashBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Dash Box"));
	dashBox->SetupAttachment(RootComponent);
	dashBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	revengeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Revenge Box"));
	revengeBox->SetupAttachment(RootComponent);
	revengeBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	

	trailParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail Particles"));
	trailParticleSystem->SetupAttachment(GetMesh());
	trailParticleSystem->SecondsBeforeInactive = 0.0f;

	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	widgetComp->SetupAttachment(RootComponent);

	audioComp = CreateDefaultSubobject<UCustomAudioComponent>(TEXT("Audio Component"));
	audioComp->SetupAttachment(RootComponent);

	fileReader = CreateDefaultSubobject<UExternalFileReader>(TEXT("File Reader"));

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	battleStats = FBattleStats();
	activeBoosts = FActiveBoosts();
	passiveBoosts = FPassiveBoosts();

	interactable = nullptr;
	bBusyWithUI = false;

	bCanInteract = false;
	bCanDash = true;
	bIframes = false;
	bHasInformedAudioOfLH = false;
	iFramesDuration = 1.2f;

	cameraShakeRate = 0.5f;
	currentCastCount = 0;



	
}


void ATopDownHSCharacter::BeginPlay()
{
	Super::BeginPlay();

	controller = Cast< ATopDownHSPlayerController>(GetController());
	if (controller)
		controller->SetPlayerRef(this);

	animInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	gameState = Cast< ASaveLoadGameState>(GetWorld()->GetGameState());
	if (gameState)
	{
		gameState->SetPlayerRef(this);
		gameState->LoadData();
	}

	if (attackBox)
		attackBox->OnComponentBeginOverlap.AddDynamic(this, &ATopDownHSCharacter::AttackBoxApplyDamage);
	if (specialBox)
		specialBox->OnComponentBeginOverlap.AddDynamic(this, &ATopDownHSCharacter::SpecialBoxApplyDamage);
	if (dashBox)
		dashBox->OnComponentBeginOverlap.AddDynamic(this, &ATopDownHSCharacter::DashBoxApplyDamage);
	if (revengeBox)
		revengeBox->OnComponentBeginOverlap.AddDynamic(this, &ATopDownHSCharacter::RevengeBoxApplyDamage);

	if (widgetComp)
		if (widgetComp->GetUserWidgetObject())
			widgetComp->GetUserWidgetObject()->AddToViewport();

		
}
void ATopDownHSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void ATopDownHSCharacter::SetAIManagerRef(AAIManager* aiRef_)
{
	aiManager = aiRef_;
}
void ATopDownHSCharacter::SetAudioManagerRef(AAudioManager* ref_)
{
	audioManager = ref_;
}

void ATopDownHSCharacter::HandleMoveForward(float rate_)
{
	if (!bBusyWithUI)
	{
		if (controller)
		{
			if (animInstance->attackIndex < 1 && !animInstance->bHit && !animInstance->bDeath && animInstance->bMoveable)
			{
				const FRotator Rotation = controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				// get forward vector
				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				AddMovementInput(Direction, rate_);
			}
		}
	}
}
void ATopDownHSCharacter::HandleMoveRight(float rate_)
{
	if (!bBusyWithUI)
	{
		if (controller)
		{
			if (animInstance->attackIndex < 1 && !animInstance->bHit && !animInstance->bDeath && animInstance->bMoveable)
			{
				const FRotator Rotation = controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				// get right vector 
				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				// add movement in that direction
				AddMovementInput(Direction, rate_);
			}
		}
	}
}


void ATopDownHSCharacter::HandleAttack()
{
	if (!bBusyWithUI)
	{
		if (animInstance)
			animInstance->AddToAttackIndex();
	}
}
void ATopDownHSCharacter::HandleSpecial()
{
	if (!bBusyWithUI)
	{
		if (animInstance)
			animInstance->Special();
	}
}
void ATopDownHSCharacter::HandleDash()
{
	if (!bBusyWithUI)
	{
		if (bCanDash)
		{
			bCanDash = false;
			bIframes = true;
			FVector destination = GetActorLocation() + (GetActorForwardVector() * battleStats.dashDistance);
			SetActorLocation(destination);

			if (animInstance)
				animInstance->Dash();

			FTimerHandle dashTimeHandle;
			GetWorld()->GetTimerManager().SetTimer(dashTimeHandle, this, &ATopDownHSCharacter::ReEnableDash, battleStats.dashCooldown, false);

			FTimerHandle iFramesTimeHandle;
			GetWorld()->GetTimerManager().SetTimer(iFramesTimeHandle, this, &ATopDownHSCharacter::DisableIFrames, iFramesDuration, false);
		}
	}
}


void ATopDownHSCharacter::ReEnableDash()
{
	bCanDash = true;
}

void ATopDownHSCharacter::DisableIFrames()
{
	bIframes = false;
}
void ATopDownHSCharacter::HandleCast()
{
	if (!bBusyWithUI)
	{
		if (animInstance->attackIndex < 4 && currentCastCount < battleStats.castMaxCount)
		{
			animInstance->CastProj(); //Play cast animation
		}
	}
}

void ATopDownHSCharacter::SpawnCast()
{
	currentCastCount++;
	if (currentCastCount > battleStats.castMaxCount)
		currentCastCount = battleStats.castMaxCount;

	//Spawn projectile
	int index_ = static_cast<int>(activeBoosts.castAffliction);
	if (index_ >= 0 && index_ < castProjs.Num())
	{
		if (castProjs[index_])
		{
			ACastProj* proj = GetWorld()->SpawnActor<ACastProj>(castProjs[index_], GetMesh()->GetSocketLocation("EffectSocket2"), GetActorRotation());

			if (proj)
			{
				float totalDamage_ = battleStats.baseCastDamage + battleStats.baseCastDamage * passiveBoosts.castDamageIncreasePercentage;

				//Check for critical
				if (battleStats.dashAndRevengeAndCastCriticalChance > 0.0f)
				{
					float critChance_ = FMath::RandRange(0.0f, 100.0f);
					if (critChance_ <= battleStats.dashAndRevengeAndCastCriticalChance * 100.0f)
						totalDamage_ *= 2.0f;
				}

				proj->UpdateProjStats(totalDamage_, activeBoosts.castAfflictionDamage, activeBoosts.castAfflictionPercentage);
				proj->UpdateVel(GetActorForwardVector() * 2000.0f);

				FTimerHandle castTimeHandle;
				GetWorld()->GetTimerManager().SetTimer(castTimeHandle, this, &ATopDownHSCharacter::RestoreOneCast, battleStats.castCooldown, false);
			}
		}
	}
}

void ATopDownHSCharacter::RestoreOneCast()
{
	currentCastCount--;
	if (currentCastCount < 0)
		currentCastCount = 0;
}

void ATopDownHSCharacter::SpawnWeapon()
{
	if (weapon)
	{
		FVector loc_ = GetActorLocation();
		loc_.X += 200.0f;
		FName weaponSocketName = TEXT("WeaponSocket");
		AActor* weaponActor = GetWorld()->SpawnActor<AActor>(weapon, loc_, FRotator::ZeroRotator);
		weaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, weaponSocketName);
	}
}

#pragma region Attack

void ATopDownHSCharacter::ActivateAttackBox(bool value_)
{
	if (value_)
		attackBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	else
	{
		attackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ATopDownHSCharacter::AttackBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
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
			float totalDamage_ = (battleStats.baseAttackDamage + battleStats.baseAttackDamage * passiveBoosts.attackDamageIncreasePercentage);
			//Apply damage
			if (battleStats.attackCriticalChance > 0.0f)
			{
				float critChance_ = FMath::RandRange(0.0f, 100.0f);
				if (critChance_ <= battleStats.attackCriticalChance * 100.0f)
					totalDamage_ *= 2.0f;

			}
			ApplyDamageFunctionSwitch(enemy, totalDamage_,0, activeBoosts.attackAffliction);
		}
	}
}

#pragma endregion


#pragma region Special
void ATopDownHSCharacter::ActivateSpecialBox(bool value_)
{
	if (value_)
		specialBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	else
	{
		specialBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	}
}

void ATopDownHSCharacter::SpecialBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
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
			float totalDamage_ = (battleStats.baseSpecialDamage + battleStats.baseSpecialDamage * passiveBoosts.specialDamageIncreasePercentage);
			//Apply damage
			if (battleStats.specialCriticalChance > 0.0f)
			{
				float critChance_ = FMath::RandRange(0.0f, 100.0f);
				if (critChance_ <= battleStats.specialCriticalChance * 100.0f)
					totalDamage_ *= 2.0f;

			}
			//Apply damage
			ApplyDamageFunctionSwitch(enemy, totalDamage_, 1, activeBoosts.specialAffliction);
		}
	}
}
#pragma endregion

#pragma region Dash
void ATopDownHSCharacter::ActivateDashBox(bool value_)
{
	if (value_ && (battleStats.dashDamage > 0.0f || activeBoosts.dashAffliction != EAfflictions::NONE))
	{
		dashBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		dashBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ATopDownHSCharacter::DashBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
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
			float totalDamage_ = (battleStats.dashDamage + battleStats.dashDamage * passiveBoosts.dashDamageIncreasePercentage);
			//Apply damage
			if (battleStats.dashAndRevengeAndCastCriticalChance > 0.0f)
			{
				float critChance_ = FMath::RandRange(0.0f, 100.0f);
				if (critChance_ <= battleStats.dashAndRevengeAndCastCriticalChance * 100.0f)
					totalDamage_ *= 2.0f;

			}
			//Apply damage
			ApplyDamageFunctionSwitch(enemy, totalDamage_, 2, activeBoosts.dashAffliction);
		}
	}
}
#pragma endregion

void ATopDownHSCharacter::ActivateRevengeBox(bool value_)
{
	//TODO
	// Particle effect of revenge
	if (value_ && (passiveBoosts.revengeDamage > 0.0f || passiveBoosts.revengeAffliction != EAfflictions::NONE))
	{
		revengeBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		revengeBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ATopDownHSCharacter::RevengeBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
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
			float totalDamage_ = passiveBoosts.revengeDamage;
			//Apply damage
			if (battleStats.dashAndRevengeAndCastCriticalChance > 0.0f)
			{
				float critChance_ = FMath::RandRange(0.0f, 100.0f);
				if (critChance_ <= battleStats.dashAndRevengeAndCastCriticalChance * 100.0f)
					totalDamage_ *= 2.0f;

			}
			//Apply damage
			ApplyDamageFunctionSwitch(enemy, totalDamage_, 4, passiveBoosts.revengeAffliction);
		}
	}
}

#pragma region Damage

void ATopDownHSCharacter::ApplyDamageFunctionSwitch(AEnemyBase* targetEnemy, float damage_, int actionindex_, EAfflictions affliction_)
{
	if (targetEnemy != nullptr)
	{
		//Shake the camera
		if (cameraShake)
		{
			if (controller)
				controller->PlayerCameraManager->StartMatineeCameraShake(cameraShake, cameraShakeRate);
		}
		//Spawn the impact
		int impactIndex = static_cast<int>(affliction_);
		if (impactIndex >= 0 && impactIndex < afflictionImpacts.Num())
		{
			if (afflictionImpacts[impactIndex] != nullptr)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), afflictionImpacts[impactIndex], targetEnemy->GetActorLocation(), FRotator::ZeroRotator);
		}


		if (audioComp)
			audioComp->PlaySoundEffectAtIndex(0); //Dash, cast, hit, and death sound effects are play directly from the animation
		//Apply the affliction
		switch (affliction_)
		{
		case EAfflictions::NONE:
				targetEnemy->EnemyTakeDamageNoAfflictions(damage_);
			break;
		case EAfflictions::BRITTLE:
			if (actionindex_ == 0) //Attack
				targetEnemy->EnemyTakeDamageBrittle(damage_, activeBoosts.attackAfflictionPercentage, passiveBoosts.brittleIncreasedDamage);
			else if(actionindex_ == 1)
				targetEnemy->EnemyTakeDamageBrittle(damage_, activeBoosts.specialAfflictionPercentage, passiveBoosts.brittleIncreasedDamage);
			else if(actionindex_ == 2)
				targetEnemy->EnemyTakeDamageBrittle(damage_, activeBoosts.dashAfflictionPercentage, passiveBoosts.brittleIncreasedDamage);
			else if(actionindex_ == 4)
				targetEnemy->EnemyTakeDamageBrittle(damage_, passiveBoosts.revengePercentage, passiveBoosts.brittleIncreasedDamage);
			break;
		case EAfflictions::CURSE:
			if (actionindex_ == 0) //Attack
				targetEnemy->EnemyTakeDamageCursed(damage_, activeBoosts.attackAfflictionDamage);
			else if (actionindex_ == 1)
				targetEnemy->EnemyTakeDamageCursed(damage_, activeBoosts.specialAfflictionDamage);
			else if (actionindex_ == 2)
				targetEnemy->EnemyTakeDamageCursed(damage_, activeBoosts.dashAfflictionDamage);
			else if (actionindex_ == 4)
				targetEnemy->EnemyTakeDamageCursed(damage_, passiveBoosts.revengeDamage);
			break;
		case EAfflictions::DEFY:
			if (actionindex_ == 0) //Attack
				targetEnemy->EnemyTakeDamageDefied(damage_, activeBoosts.attackAfflictionPercentage);
			else if (actionindex_ == 1)
				targetEnemy->EnemyTakeDamageDefied(damage_, activeBoosts.specialAfflictionPercentage);
			else if (actionindex_ == 2)
				targetEnemy->EnemyTakeDamageDefied(damage_, activeBoosts.dashAfflictionPercentage);
			else if (actionindex_ == 4)
				targetEnemy->EnemyTakeDamageDefied(damage_, passiveBoosts.revengePercentage);
			break;
		case EAfflictions::POISON:
			if (actionindex_ == 0) //Attack
				targetEnemy->EnemyTakeDamagePoison(damage_, activeBoosts.attackAfflictionDamage, passiveBoosts.poisonIncreasedDamage);
			else if (actionindex_ == 1)
				targetEnemy->EnemyTakeDamagePoison(damage_, activeBoosts.specialAfflictionDamage, passiveBoosts.poisonIncreasedDamage);
			else if (actionindex_ == 2)
				targetEnemy->EnemyTakeDamagePoison(damage_, activeBoosts.dashAfflictionDamage, passiveBoosts.poisonIncreasedDamage);
			else if (actionindex_ == 4)
				targetEnemy->EnemyTakeDamagePoison(damage_, passiveBoosts.revengeDamage, passiveBoosts.poisonIncreasedDamage);
			break;
		case EAfflictions::ROOT:
			if (actionindex_ == 0) //Attack
				targetEnemy->EnemyTakeDamageRoot(damage_, activeBoosts.attackAfflictionPercentage, passiveBoosts.rootIncreasedDamage);
			else if (actionindex_ == 1)
				targetEnemy->EnemyTakeDamageRoot(damage_, activeBoosts.specialAfflictionPercentage, passiveBoosts.rootIncreasedDamage);
			else if (actionindex_ == 2)
				targetEnemy->EnemyTakeDamageRoot(damage_, activeBoosts.dashAfflictionPercentage, passiveBoosts.rootIncreasedDamage);
			else if (actionindex_ == 4)
				targetEnemy->EnemyTakeDamageRoot(damage_, passiveBoosts.revengePercentage, passiveBoosts.rootIncreasedDamage);
			break;
		}
	}
}


void ATopDownHSCharacter::PlayerTakeDamage(float damage_)
{
	if (!bIframes && !animInstance->bDeath)
	{
		if (cameraShake)
		{
			if (controller)
				controller->PlayerCameraManager->StartMatineeCameraShake(cameraShake, cameraShakeRate);
		}
		damage_ -= damage_ * passiveBoosts.damageReductionPercentage;
		battleStats.currentHP -= damage_;

		if (battleStats.currentHP <= 0.0f)
		{
			if (audioManager)
				audioManager->PlayDemonVoiceLine(0);
			//Death animation
			if (animInstance)
				animInstance->EnableDeath();

			if (gameState)
				gameState->ResetCurrentGameState();
			
			bCanDash = false;

		}
		else
		{
			if (battleStats.currentHP <= 0.7f * battleStats.maxHP && !bHasInformedAudioOfLH)
			{
				if (audioManager)
					audioManager->PlayDemonVoiceLine(1);
				bHasInformedAudioOfLH = true;

				FTimerHandle lhVoiceTimeHandle;
				GetWorld()->GetTimerManager().SetTimer(lhVoiceTimeHandle, this, &ATopDownHSCharacter::ReEnableLHVoiceLine, 360.0f, false); //After 6 minutes, re-enable the voice lines
			}
			if (animInstance)
				animInstance->EnableHit();

			ActivateRevengeBox(true); //False will be called from the animation
			//Give the player a breathing room
			bIframes = true;
			FTimerHandle iFramesTimeHandle;
			GetWorld()->GetTimerManager().SetTimer(iFramesTimeHandle, this, &ATopDownHSCharacter::DisableIFrames, 0.1f, false);
		}
	}
}

#pragma endregion

#pragma region Interactales

void ATopDownHSCharacter::UpdateInteractableRef(IInteractable* ref_)
{
	interactable = ref_;

	if (interactable)
	{
		bCanInteract = true;
	}
	else
	{
		bCanInteract = false;
	}
}

void ATopDownHSCharacter::InteractWithInteractable()
{
	if (interactable)
	{
		interactable->Interact();
		bBusyWithUI = true;
		bCanInteract = false;
	}
}

#pragma endregion

#pragma region Boosts
void ATopDownHSCharacter::EquipBoost(FBoostsTable boost_)
{

	//Called from the game state when loading in boosts
	if (fileReader && boost_.affliction != 0) //NONE is used for hp upgrades and these don't get "acquired"
		fileReader->AcquireBoost(GetTableIndex(boost_.affliction,boost_.activeOrPassive), boost_.id);

	if (boost_.activeOrPassive == 0) //Active
	{
		switch (boost_.boostIndex)
		{
		case 1:
			activeBoosts.attackAffliction = static_cast<EAfflictions>(boost_.affliction);
			activeBoosts.attackAfflictionPercentage = boost_.boostValue;
			passiveBoosts.attackDamageIncreasePercentage = boost_.boostValue;
			break;
		case 2:
			activeBoosts.attackAffliction = static_cast<EAfflictions>(boost_.affliction);
			activeBoosts.attackAfflictionDamage = boost_.boostValue;
			passiveBoosts.attackDamageIncreasePercentage = boost_.boostValue;
			break;

		case 4:
			activeBoosts.specialAffliction = static_cast<EAfflictions>(boost_.affliction);
			activeBoosts.specialAfflictionPercentage = boost_.boostValue;
			passiveBoosts.specialDamageIncreasePercentage = boost_.boostValue;
			break;
		case 5:
			activeBoosts.specialAffliction = static_cast<EAfflictions>(boost_.affliction);
			activeBoosts.specialAfflictionDamage = boost_.boostValue;
			passiveBoosts.specialDamageIncreasePercentage = boost_.boostValue;
			break;

		case 7:
			activeBoosts.dashAffliction = static_cast<EAfflictions>(boost_.affliction);
			activeBoosts.dashAfflictionPercentage = boost_.boostValue;
			passiveBoosts.dashDamageIncreasePercentage = boost_.boostValue;
			break;
		case 8:
			activeBoosts.dashAffliction = static_cast<EAfflictions>(boost_.affliction);
			activeBoosts.dashAfflictionDamage = boost_.boostValue;
			passiveBoosts.dashDamageIncreasePercentage = boost_.boostValue;
			break;

		case 10:
			activeBoosts.castAffliction = static_cast<EAfflictions>(boost_.affliction);
			activeBoosts.castAfflictionPercentage = boost_.boostValue;
			passiveBoosts.castDamageIncreasePercentage = boost_.boostValue;
			break;
		case 11:
			activeBoosts.castAffliction = static_cast<EAfflictions>(boost_.affliction);
			activeBoosts.castAfflictionDamage = boost_.boostValue;
			passiveBoosts.castDamageIncreasePercentage = boost_.boostValue;
			break;
		case 12:
			battleStats.attackCriticalChance += boost_.boostValue;
			break;
		case 13:
			battleStats.specialCriticalChance += boost_.boostValue;
			break;
		case 14:
			battleStats.attackCriticalChance += boost_.boostValue;
			battleStats.specialCriticalChance += boost_.boostValue;
			battleStats.dashAndRevengeAndCastCriticalChance += boost_.boostValue;
			break;
		}
	}
	else //Passive
	{
		switch (boost_.boostIndex)
		{
		case 0:
			battleStats.maxHP += boost_.boostValue * 100.0f;
			battleStats.currentHP += boost_.boostValue * 100.0f;
			break;
		case 1:
			battleStats.currentHP += boost_.boostValue;
			break;
		case 2:
			passiveBoosts.damageReductionPercentage = boost_.boostValue;
			break;
		case 5:
			passiveBoosts.moveSpeedIncreasePercentage = boost_.boostValue;
			GetCharacterMovement()->MaxWalkSpeed += GetCharacterMovement()->MaxWalkSpeed * boost_.boostValue;
			break;
		case 6:
			passiveBoosts.revengeAffliction = static_cast<EAfflictions>(boost_.affliction);
			passiveBoosts.revengeDamage = boost_.boostValue;
			break;
		case 7: 
			passiveBoosts.revengeAffliction = static_cast<EAfflictions>(boost_.affliction);
			passiveBoosts.revengePercentage = boost_.boostValue;
			break;
		case 8:
			passiveBoosts.brittleIncreasedDamage = boost_.boostValue;
			break;
		case 9:
			passiveBoosts.poisonIncreasedDamage = boost_.boostValue;
			break;
		case 10:
			passiveBoosts.rootIncreasedDamage = boost_.boostValue;
			break;
		}

	}

	bBusyWithUI = false;
	bCanInteract = false; //Remove the interact icon from the UI
}

void ATopDownHSCharacter::LeaveCurrentMap()
{
	if (animInstance)
		animInstance->bMoveable = false; //After interacting with a boost, leave to the next area
}

#pragma endregion


void ATopDownHSCharacter::ActivateAfflictionEffect(int actionIndex_, FName socketName_)
{
	int effectIndex = -1;

	switch (actionIndex_)
	{
	case 0: //Attack
		effectIndex = static_cast<int>(activeBoosts.attackAffliction);
		break;
	case 1: //Special
		effectIndex = static_cast<int>(activeBoosts.specialAffliction);
		break;
	case 2: //Dash
		effectIndex = static_cast<int>(activeBoosts.dashAffliction);
		break;
	case 3: //Cast
		effectIndex = static_cast<int>(activeBoosts.castAffliction);
		break;
	case 4: //Revenge
		effectIndex = static_cast<int>(passiveBoosts.revengeAffliction);
		break;
	}

	if (effectIndex >= 0 && effectIndex < afflictionEffects.Num())
	{
		trailParticleSystem->Template = afflictionEffects[effectIndex];
		trailParticleSystem->BeginTrails(FName(TEXT("EffectSocket1")), FName(TEXT("EffectSocket2")), ETrailWidthMode::ETrailWidthMode_FromFirst, 3.0f);
	}
}

void ATopDownHSCharacter::FinishedLoadingData()//Called by the game state after the loading of data has concluded
{
	if (Intermediate::GetInstance()->currentWaveID >= 4)
	{
		if (animInstance)
			animInstance->bMoveable = false; //End
	}
	else
	{
		if (aiManager && !animInstance->bDeath) //Need to check for death here as we save data when the player dies which ends up calling this function
			aiManager->NextWave();
	}
}


float ATopDownHSCharacter::GetHPPercentage()
{
	return (battleStats.currentHP / battleStats.maxHP);
}

void ATopDownHSCharacter::EndTrails()
{
	trailParticleSystem->EndTrails();
}

void ATopDownHSCharacter::ReEnableLHVoiceLine()
{
	bHasInformedAudioOfLH = false;
}

int ATopDownHSCharacter::GetTableIndex(int affliction_, int activePassive_)
{
	switch (affliction_)
	{
	case 1:
		return (0 + activePassive_);
	case 2:
		return (2 + activePassive_);
	case 3:
		return (4 + activePassive_);
	case 4:
		return (6 + activePassive_);
	case 5:
		return 8;
	case 6:
		return 10;
	}
	return -1;
}
