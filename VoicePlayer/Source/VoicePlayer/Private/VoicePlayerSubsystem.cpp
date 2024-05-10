// Fill out your copyright notice in the Description page of Project Settings.


#include "VoicePlayerSubsystem.h"

#include "AudioThread.h"
#include "Kismet/GameplayStatics.h"

UVoicePlayerSubsystem::UVoicePlayerSubsystem()
{
}

void UVoicePlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UVoicePlayerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UVoicePlayerSubsystem::PlaySound(const UObject* WorldContextObject, USoundBase* Sound)
{
	UGameplayStatics::PlaySound2D(WorldContextObject, Sound);
	
	FAudioThread::RunCommandOnGameThread([this]()
	{
		this->TaskCompletedDelegate.Broadcast();
	});
}

void UVoicePlayerSubsystem::Stop()
{
}
