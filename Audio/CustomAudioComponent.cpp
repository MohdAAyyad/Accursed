// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomAudioComponent.h"


UCustomAudioComponent::UCustomAudioComponent() : UAudioComponent()
{
	fadeInDuration = 0.8f;
}

void UCustomAudioComponent::InitValues(float volume_, float pitch_)
{
	VolumeMultiplier = volume_;
	PitchMultiplier = pitch_;
}

void UCustomAudioComponent::PlaySoundEffectAtIndex(int index_)
{
	if (index_ >= 0 && index_ < soundEffects.Num())
	{
		if (soundEffects[index_])
		{
			Sound = soundEffects[index_];
			Play();
		}
	}
}

void UCustomAudioComponent::FadeInSoundEffectAtIndex(int index_)
{
	if (index_ >= 0 && index_ < soundEffects.Num())
	{
		if (soundEffects[index_])
		{
			Sound = soundEffects[index_];
			FadeIn(fadeInDuration, VolumeMultiplier);
		}
	}
}

USoundBase* UCustomAudioComponent::GetSoundAtIndex(int index_)
{
	if (index_ >= 0 && index_ < soundEffects.Num())
		return soundEffects[index_];

	return nullptr;
}
