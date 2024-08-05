// Fill out your copyright notice in the Description page of Project Settings.


#include "VoiceRecordingAsyncAction.h"
#include "AudioCaptureComponent.h"
#include "AudioMixerBlueprintLibrary.h"

DEFINE_LOG_CATEGORY(LogVoiceRecordingAsyncAction)

UVoiceRecordingAsyncAction::UVoiceRecordingAsyncAction(const FObjectInitializer& ObjectInitializer)
: bIsRecording(false)
, SilenceDuration(0.0f)
, SilenceThreshold(0.009999f)
, MaxSilenceDuration(3.0f)
, OutputType(EAudioRecordingExportType::WavFile)
, AudioCaptureComponent(nullptr)
, SoundSubmix(nullptr)
, ExistingSoundWaveToOverwrite(nullptr)
{
}

UVoiceRecordingAsyncAction* UVoiceRecordingAsyncAction::Start(
	const UObject* WorldContextObject,
	const EAudioRecordingExportType InOutputType,
	const FString& InOutputDir,
	const FString& InOutputName,
	const float TimeVAD,
	UAudioCaptureComponent* InAudioCaptureComponent,
	USoundSubmix* InSoundSubmix,
	USoundWave* InExistingSoundWaveToOverwrite)
{
	if(!InAudioCaptureComponent || InOutputDir.IsEmpty() || InOutputName.IsEmpty())
	{
		UE_LOG(LogVoiceRecordingAsyncAction, Error, TEXT("ThreadID:%d, %s: Voice recording params invalid!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}
	
	ThisClass* const AsyncAction = NewObject<ThisClass>();
	AsyncAction->RegisterWithGameInstance(WorldContextObject);

	AsyncAction->OutputType = InOutputType;
	AsyncAction->OutputDir = InOutputDir;
	AsyncAction->OutputName = InOutputName;
	AsyncAction->AudioCaptureComponent = InAudioCaptureComponent;
	AsyncAction->SoundSubmix = InSoundSubmix;
	AsyncAction->ExistingSoundWaveToOverwrite = InExistingSoundWaveToOverwrite;

	AsyncAction->AudioEnvelopeDelegateHandle = AsyncAction->AudioCaptureComponent->OnAudioEnvelopeValueNative.AddLambda([AsyncAction](const UAudioComponent* AudioComponent, const float EnvelopeValue)
	{
		AsyncAction->RecordingDelegate.Broadcast(EnvelopeValue, nullptr, FString());

		if(EnvelopeValue > AsyncAction->SilenceThreshold)
		{
			AsyncAction->SilenceDuration = 0.0f;
		}
	});


	if(TimeVAD > 0)
	{
		AsyncAction->MaxSilenceDuration = TimeVAD;
	}
	
	AsyncAction->SilenceDuration = 0.0f;
	InAudioCaptureComponent->Start();
	AsyncAction->bIsRecording = true;
	AsyncAction->StartDelegate.Broadcast(0.0, nullptr, FString());
	
	UAudioMixerBlueprintLibrary::StartRecordingOutput(WorldContextObject, 0.0f, AsyncAction->SoundSubmix);
	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(AsyncAction->SilenceCheckTimerHandle, [WorldContextObject, AsyncAction]()
	{
		AsyncAction->SilenceDuration += 0.1f;
		if(AsyncAction->SilenceDuration > AsyncAction->MaxSilenceDuration)
		{
			AsyncAction->StopVoiceRecording(WorldContextObject);
		}
	}, 0.1, true);

	return AsyncAction;
}

void UVoiceRecordingAsyncAction::StopAudioRecordingOverwriteOutput(const UObject* WorldContextObject,
		const EAudioRecordingExportType InOutputType,
		const FString& InOutputDir,
		const FString& InOutputName,
		USoundSubmix* InSoundSubmix,
		USoundWave* InExistingSoundWaveToOverwrite)
{
	if(SilenceCheckTimerHandle.IsValid())
		WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(SilenceCheckTimerHandle);

	if(!bIsRecording)
	{
		return;
	}
	
	USoundWave* SoundWave = UAudioMixerBlueprintLibrary::StopRecordingOutput(WorldContextObject, InOutputType, InOutputName, InOutputDir, InSoundSubmix, InExistingSoundWaveToOverwrite);

	AudioCaptureComponent->Stop();
	AudioCaptureComponent->OnAudioEnvelopeValueNative.Remove(AudioEnvelopeDelegateHandle);
	
	bIsRecording = false;
	FString AbsoluteFilePath = FPaths::ConvertRelativePathToFull(InOutputDir + InOutputName + FString(TEXT(".wav")));
	StopDelegate.Broadcast(0.0, SoundWave, AbsoluteFilePath);

	SetReadyToDestroy();
}

void UVoiceRecordingAsyncAction::StopVoiceRecording(const UObject* WorldContextObject)
{
	StopAudioRecordingOverwriteOutput(WorldContextObject, OutputType, OutputDir, OutputName, SoundSubmix, ExistingSoundWaveToOverwrite);
}
