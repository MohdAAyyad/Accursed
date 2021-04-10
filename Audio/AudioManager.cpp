// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "CustomAudioComponent.h"
#include "LevelSequencePlayer.h"
#include "../TopDownHSCharacter.h"

AAudioManager::AAudioManager()
{
	PrimaryActorTick.bCanEverTick = false;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = root;

	musicComponent = CreateDefaultSubobject<UCustomAudioComponent>(TEXT("Music Component"));
	musicComponent->SetupAttachment(root);

	demonVoiceAudioComponent = CreateDefaultSubobject<UCustomAudioComponent>(TEXT("Demon voice Audio Component"));
	demonVoiceAudioComponent->SetupAttachment(root);

	battleMusicIndex = 1;
	demonVoiceIndex = -1;
	fadeOutTime = 1.5f;
}

void AAudioManager::BeginPlay()
{
	Super::BeginPlay();
	if (musicComponent)
	{
		musicComponent->FadeInSoundEffectAtIndex(0);
	}

	/*if (demonVoiceAudioComponent)
		demonVoiceAudioComponent->OnAudioFinished.AddDynamic(this, &AAudioManager::ReEnableMusic);*/

	FTimerHandle findPlayerTimeHandle;
	GetWorld()->GetTimerManager().SetTimer(findPlayerTimeHandle, this, &AAudioManager::DelayedFindPlayer, 0.1f, false);
}

void AAudioManager::DelayedFindPlayer()
{
	ATopDownHSCharacter* playerRef_ = Cast<ATopDownHSCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (playerRef_)
		playerRef_->SetAudioManagerRef(this);
}
void AAudioManager::SwitchToBattleMusic()
{
	if (musicComponent->IsPlaying()) //If playing fade out first
	{
		FTimerHandle fadeOutTimeHandle;
		musicComponent->FadeOut(fadeOutTime, 0.0f);
		GetWorld()->GetTimerManager().SetTimer(fadeOutTimeHandle, this, &AAudioManager::PlayNextBattlePiece, fadeOutTime, false);
	}
	else //Otherwise, start playing
	{
		PlayNextBattlePiece();
	}

}

void AAudioManager::WaveHasEnded(bool fiveWaves_)
{
	if (fiveWaves_) //Every five waves, change the music
		battleMusicIndex++;

	FTimerHandle fadeOutTimeHandle;
	musicComponent->FadeOut(fadeOutTime, 0.0f);
	GetWorld()->GetTimerManager().SetTimer(fadeOutTimeHandle, this, &AAudioManager::PlayNextBattlePiece, fadeOutTime, false);

}
void AAudioManager::PlayNextBattlePiece()
{
	if (musicComponent)
		musicComponent->FadeInSoundEffectAtIndex(battleMusicIndex);
}

void AAudioManager::PlayBoostPiece()
{
	if (musicComponent)
		musicComponent->FadeInSoundEffectAtIndex(0);
}

void AAudioManager::FadeOutCurrentMusic()
{
	musicComponent->FadeOut(1.2f, 0.0f);
}

void AAudioManager::PlayDemonVoiceLine(int index_)
{

	if (demonSequence) //Create and play the demon voice sequence
	{
		FMovieSceneSequencePlaybackSettings settings_ = FMovieSceneSequencePlaybackSettings();
		ALevelSequenceActor* sequenceActor_;
		ULevelSequencePlayer* sequencePlayer_ = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), demonSequence, settings_, sequenceActor_);
		sequencePlayer_->Play();
	}

	switch (index_)
	{
	case 0: //Death
		demonVoiceIndex = FMath::RandRange(0, 4);
		break;
	case 1: //Low health
		demonVoiceIndex = FMath::RandRange(5, 7);
		break;
	case 2: //Neutral
		demonVoiceIndex = FMath::RandRange(8, 12);
		break;
	}

	//We need the delay  so we can play the intro sound effect first (played in the level sequence)
	FTimerHandle demonVoiceTimeHandle;
	GetWorld()->GetTimerManager().SetTimer(demonVoiceTimeHandle, this, &AAudioManager::DelayedPlayDemonVoiceLine, 0.09f, false);
}

void AAudioManager::DelayedPlayDemonVoiceLine()
{
	if (demonVoiceAudioComponent)
	{
		if(!demonVoiceAudioComponent->IsPlaying())
			demonVoiceAudioComponent->PlaySoundEffectAtIndex(demonVoiceIndex);
	}
}

