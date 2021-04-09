// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudioManager.generated.h"

UCLASS()
class TOPDOWNHS_API AAudioManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAudioManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Root")
		USceneComponent* root;
	UPROPERTY(EditAnywhere, Category = "AudioComponent")
		class UCustomAudioComponent* musicComponent;
	UPROPERTY(EditAnywhere, Category = "DemonVoiceAudioComponent")
		UCustomAudioComponent* demonVoiceAudioComponent;
	UPROPERTY(EditAnywhere, Category = "DemonVoiceAudioComponent")
		class ULevelSequence* demonSequence;

	int battleMusicIndex;
	int demonVoiceIndex;
	float fadeOutTime;

	//UFUNCTION()
	//void ReEnableMusic(); //Called  when the demon audio component finishes playing

	void DelayedFindPlayer(); //Called from begin play

public:
	void SwitchToBattleMusic();
	void WaveHasEnded(bool fiveWaves_);
	void PlayNextBattlePiece();
	void PlayBoostPiece();
	UFUNCTION(BlueprintCallable)
		void FadeOutCurrentMusic();
	void PlayDemonVoiceLine(int index_);
	void DelayedPlayDemonVoiceLine();


};
