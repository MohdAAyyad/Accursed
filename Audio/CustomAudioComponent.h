// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "CustomAudioComponent.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNHS_API UCustomAudioComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	UCustomAudioComponent();
	void InitValues(float volume_, float pitch_);
	void PlaySoundEffectAtIndex(int index_);
	void FadeInSoundEffectAtIndex(int index_);

protected:
	UPROPERTY(EditAnywhere, Category = "SoundEffects")
		TArray<class USoundBase*> soundEffects;
	UPROPERTY(EditAnywhere, Category = "SoundEffects")
		float fadeInDuration;

	
};
