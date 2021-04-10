
/// <Dev comment>
/// Some functions in non-management classes make calls that can be considered "game management" and that is not ideal, however, when considering the scope
/// and limited time for developing this project, I decided to go ahead with these non-ideal solutions.
/// </Dev comment>>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interactable.h"
#include "ExternalFileReader/ExternalFileReader.h"
#include "TopDownHSCharacter.generated.h"

UENUM(BlueprintType)
enum class EAfflictions : uint8
{
	NONE = 0,
	CURSE = 1,
	POISON = 2,
	BRITTLE = 3,
	ROOT = 4,
	DEFY = 5,
	CRITICAL = 6
};

USTRUCT(BlueprintType)
struct FBattleStats
{
	GENERATED_USTRUCT_BODY()
public:
	FBattleStats()
	{
		currentHP = maxHP = 50.0f;
		baseAttackDamage = 25.0f;
		baseSpecialDamage = 40.0f;
		baseCastDamage = 20.0f;
		dashDistance = 600.0f;
		dashDamage = 0.0f;
		dashCooldown = 1.0f;
		castCooldown = 1.5f;
		castMaxCount = 2;
		attackCriticalChance = specialCriticalChance = dashAndRevengeAndCastCriticalChance = 0.0f;
	}
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float maxHP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float currentHP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float baseAttackDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float baseSpecialDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float baseCastDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float castCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int castMaxCount; //How many can the user cast
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float dashDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float dashDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float dashCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float attackCriticalChance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float specialCriticalChance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float dashAndRevengeAndCastCriticalChance; //Only one boost "Everything deals critical" can affect these three

};


USTRUCT(BlueprintType)
struct FActiveBoosts
{
	GENERATED_USTRUCT_BODY()
public:
	FActiveBoosts()
	{
		attackAffliction = specialAffliction = dashAffliction = castAffliction = EAfflictions::NONE;
		attackAfflictionPercentage = attackAfflictionDamage = specialAfflictionPercentage = specialAfflictionDamage =
			dashAfflictionPercentage = dashAfflictionDamage = castAfflictionPercentage = castAfflictionDamage = 0.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EAfflictions attackAffliction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float attackAfflictionPercentage; //Brittle, defied, rooted
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float attackAfflictionDamage; //Cursed, poisoned
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EAfflictions specialAffliction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float specialAfflictionPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float specialAfflictionDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EAfflictions dashAffliction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float dashAfflictionPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float dashAfflictionDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EAfflictions castAffliction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float castAfflictionPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float castAfflictionDamage;

};

USTRUCT(BlueprintType)
struct FPassiveBoosts
{
	GENERATED_USTRUCT_BODY()
public:
	FPassiveBoosts()
	{
		damageReductionPercentage = attackDamageIncreasePercentage = specialDamageIncreasePercentage =
			moveSpeedIncreasePercentage = revengeDamage = dashDamageIncreasePercentage = castDamageIncreasePercentage= revengePercentage = brittleIncreasedDamage = poisonIncreasedDamage =
			rootIncreasedDamage = 0.0f;
		revengeAffliction = EAfflictions::NONE;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float damageReductionPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float attackDamageIncreasePercentage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float specialDamageIncreasePercentage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float dashDamageIncreasePercentage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float castDamageIncreasePercentage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float moveSpeedIncreasePercentage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float revengeDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float revengePercentage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EAfflictions revengeAffliction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float brittleIncreasedDamage; //Enemies inflicted with brittle take more damage

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float poisonIncreasedDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float rootIncreasedDamage;

};

UCLASS(Blueprintable)
class ATopDownHSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATopDownHSCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

protected:
#pragma region ComponentsAndRefs
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	class ATopDownHSPlayerController* controller;
	class UPlayerAnimInstance* animInstance;

	class AAIManager* aiManager;
	IInteractable* interactable;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Widget")
		class UWidgetComponent* widgetComp;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Audio")
		class UCustomAudioComponent* audioComp;


	UPROPERTY(EditAnywhere, Category = "FileReader")
		UExternalFileReader* fileReader;

	class ASaveLoadGameState* gameState;
	class AAudioManager* audioManager;

#pragma endregion

#pragma region Weapon And CameraShake
	UPROPERTY(EditAnywhere, Category = "Weapon")
		TSubclassOf<AActor> weapon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CastProj")
		TArray<TSubclassOf<class ACastProj>> castProjs;

	UPROPERTY(EditAnywhere, Category = "Camera Shake")
		TSubclassOf<class UPCameraShake> cameraShake;
	UPROPERTY(EditAnywhere, Category = "Camera Shake")
		float cameraShakeRate;
#pragma endregion

#pragma region Stats
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		FBattleStats battleStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		FActiveBoosts activeBoosts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		FPassiveBoosts passiveBoosts;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
		int currentCastCount; //How many have we shot so far?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		float iFramesDuration;

#pragma endregion

#pragma region Effects
	UPROPERTY(EditAnywhere, Category = "Effects")
		TArray<class UParticleSystem*> afflictionEffects;

	UPROPERTY(EditAnywhere, Category = "Effects")
		TArray<class UParticleSystem*> afflictionImpacts;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Effects")
		class UParticleSystemComponent* trailParticleSystem;

#pragma endregion

#pragma region Boxes
	UPROPERTY(EditAnywhere, Category = "Box")
		class UBoxComponent* attackBox;

	UPROPERTY(EditAnywhere, Category = "Box")
		 UBoxComponent* specialBox;

	UPROPERTY(EditAnywhere, Category = "Box")
		 UBoxComponent* dashBox;
	UPROPERTY(EditAnywhere, Category = "Box")
		UBoxComponent* revengeBox;
#pragma endregion

#pragma region Booleans
	UPROPERTY(BlueprintReadOnly, Category = "Box")
		bool bCanInteract; //Read by the UI to know if "interact" should apper on screen

	bool bBusyWithUI;
	bool bCanDash;
	bool bIframes;
	bool bHasInformedAudioOfLH; //When at low health, play a demon voice line
#pragma endregion

#pragma region ProtectedFunctions
	void BeginPlay() override;

	UFUNCTION()
		void AttackBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult& sweepResult_);

	UFUNCTION()
		void SpecialBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult& sweepResult_);

	UFUNCTION()
		void DashBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult& sweepResult_);

	UFUNCTION()
		void RevengeBoxApplyDamage(UPrimitiveComponent* overlappedComponent_,
			AActor* otherActor_,
			UPrimitiveComponent* otherComp_,
			int32 otherBodyIndex_,
			bool bFromSweep_,
			const FHitResult& sweepResult_);

	UFUNCTION(BlueprintCallable) //Called from the animation
		void ActivateAttackBox(bool value_);
	UFUNCTION(BlueprintCallable) //Called from the animation
		void ActivateSpecialBox(bool value_);
	UFUNCTION(BlueprintCallable) //Called from the animation
		void ActivateDashBox(bool value_);
	UFUNCTION(BlueprintCallable) //Called from the animation
		void ActivateRevengeBox(bool value_);
	UFUNCTION(BlueprintCallable) //Called from the animation
		void SpawnCast();


	void ApplyDamageFunctionSwitch(class AEnemyBase* targetEnemy, float damage_, int actionindex_, EAfflictions affliction_);

	UFUNCTION(BlueprintCallable) //Called from the animation
		void ActivateAfflictionEffect(int actionIndex_, FName socketName_);


	UFUNCTION(BlueprintCallable)
		void EndTrails(); //Called from the animation
	

	void ReEnableDash();
	void DisableIFrames();
	void ReEnableLHVoiceLine();
	void RestoreOneCast();
	int GetTableIndex(int affliction_, int activePassive_);


#pragma endregion

public:
#pragma region PublicFunctions
	void HandleMoveForward(float rate_);
	void HandleMoveRight(float rate_);
	void HandleAttack();
	void HandleSpecial();
	void HandleDash();
	void HandleCast();

	UFUNCTION(BlueprintCallable)
		void SpawnWeapon();

	void UpdateInteractableRef(IInteractable* ref_); //Called from the objects the player can interact with to update the IInteractable pointer
	void InteractWithInteractable(); //Calls the Interact function on the interactable
	void EquipBoost(FBoostsTable boost_);

	void SetAIManagerRef(AAIManager* aiRef_);
	void SetAudioManagerRef(AAudioManager* ref_);

	void PlayerTakeDamage(float damage_);

	UFUNCTION(BlueprintCallable)
		float GetHPPercentage();

	void LeaveCurrentMap(); //Called after acquiring boost

	void FinishedLoadingData();


#pragma endregion
};

