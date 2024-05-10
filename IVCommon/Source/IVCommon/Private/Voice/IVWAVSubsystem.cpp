// Fill out your copyright notice in the Description page of Project Settings.


#include "Voice/IVWAVSubsystem.h"
#include "Components/AudioComponent.h"
#include "IVCommon.h"

void UIVWAVSubsystem::SetAudioComponent(UAudioComponent* InAudioComponent)
{
	if(InAudioComponent)
	{
		AudioComponent=InAudioComponent;
		AudioComponent->OnAudioFinishedNative.AddUObject(this,&UIVWAVSubsystem::PlayAudioFinish);
	}

}

void UIVWAVSubsystem::InsertSoundWaveQueue( USoundWave* const SoundWave)
{
	if(SoundWave)
	{
		SoundWaveArray.Add(SoundWave);
		SoundWave->AddToRoot();
	}
}

void UIVWAVSubsystem::InitializeQueueExecution()
{
	if(IsValidAudioComponent() &&SoundWaveArray[0])
	{
		AudioComponent->SetSound(SoundWaveArray[0]);
		AudioComponent->Play();
		SoundWaveArray[0]->RemoveFromRoot();
		SoundWaveArray.RemoveAt(0);
	}
}

void UIVWAVSubsystem::StopSoundWaveQueueExecution()
{
	for(USoundWave* Wave:SoundWaveArray)
	{
		Wave->RemoveFromRoot();
	}
	
	SoundWaveArray.Empty();
	if(AudioComponent->IsPlaying())
	{
		AudioComponent->Stop();
	}
	
}


bool UIVWAVSubsystem::IsValidAudioComponent() 
{
	return AudioComponent->IsValidLowLevel();
}

void UIVWAVSubsystem::PlayAudioFinish(UAudioComponent* InAudioComponent)
{
	if(SoundWaveArray.IsValidIndex(0)&&SoundWaveArray[0])
	{
		InAudioComponent->SetSound(SoundWaveArray[0]);
		InAudioComponent->Play();
		SoundWaveArray[0]->RemoveFromRoot();
		SoundWaveArray.RemoveAt(0);
	}
	else
	{
		StopSoundWaveQueueExecution();
	}
}



