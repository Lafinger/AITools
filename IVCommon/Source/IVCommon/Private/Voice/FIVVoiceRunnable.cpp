// Fill out your copyright notice in the Description page of Project Settings.


#include "Voice/FIVVoiceRunnable.h"

#include "Voice/IVAudioCaptureSubsystem.h"


FIVVoiceRunnable::FIVVoiceRunnable(FString InThreadName)
	:ThreadName(InThreadName)
{
}

FIVVoiceRunnable::~FIVVoiceRunnable()
{
}

bool FIVVoiceRunnable::Init()
{
	if(UIVAudioCaptureSubsystem* Subsystem = GEngine->GetEngineSubsystem<UIVAudioCaptureSubsystem>())
	{
		AudioCaptureSubsystem = Subsystem;
		return true;
	}

	return false;
}

uint32 FIVVoiceRunnable::Run()
{
	bIsRunning = true;
	
	while (true)
	{
		FPlatformProcess::Sleep(0.04);

		TArray<float> VoiceData;


		bool bRightVoice = AudioCaptureSubsystem->GetVoiceData(VoiceData);


		// FScopeLock RunnableLcoak(&CriticalSection);

		if (!bIsRunning)
		{
			break;
		}

		if (bRightVoice)
		{
			OnAudioBufferResult.ExecuteIfBound(VoiceData);
		}


	}
	return 0;
}

void FIVVoiceRunnable::Stop()
{
	bIsRunning = false;
	AudioCaptureSubsystem->ClearVoiceData();
}

void FIVVoiceRunnable::Exit()
{
	AudioCaptureSubsystem->ClearVoiceData();
}
