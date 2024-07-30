// Fill out your copyright notice in the Description page of Project Settings.


#include "VoiceRecognitionSubsystem.h"

#include "VoiceRecognitionAsyncAction.h"
#include "VoiceRecognitionSettings.h"
#include "VoiceRecordingAsyncAction.h"

DEFINE_LOG_CATEGORY(LogVoiceRecognitionSubsystem)

UVoiceRecognitionSubsystem::UVoiceRecognitionSubsystem(const FObjectInitializer& ObjectInitializer)
: VoiceRecordingAsyncAction(nullptr)
, VoiceRecognitionAsyncAction(nullptr)
, Language(TEXT("cn"))
{
	
}

bool UVoiceRecognitionSubsystem::Start(const UObject* WorldContextObject, const float TimeVAD, const FString& InLanguage, UAudioCaptureComponent* InAudioCaptureComponent, USoundSubmix* InSoundSubmix, USoundWave* InExistingSoundWaveToOverwrite)
{
	if(!InAudioCaptureComponent)
	{
		UE_LOG(LogVoiceRecognitionSubsystem, Error, TEXT("ThreadID:%d, %s: Voice recognition input parameters invaild!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return false;
	}

	if(InLanguage.IsEmpty())
	{
		Language = InLanguage;
	}

	VoiceRecordingAsyncAction =
		UVoiceRecordingAsyncAction::Start(
			WorldContextObject,
			EAudioRecordingExportType::WavFile,
			FPaths::ProjectDir(),
			TEXT("RecordingAudio"),
			TimeVAD,
			InAudioCaptureComponent,
			InSoundSubmix,
			InExistingSoundWaveToOverwrite);

	if(!VoiceRecordingAsyncAction)
	{
		UE_LOG(LogVoiceRecognitionSubsystem, Error, TEXT("ThreadID:%d, %s: Voice recording async action error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return false; 
	}

	VoiceRecordingAsyncAction->StartDelegate.AddDynamic(this, &UVoiceRecognitionSubsystem::OnRecordStart);
	VoiceRecordingAsyncAction->RecordingDelegate.AddDynamic(this, &UVoiceRecognitionSubsystem::OnRecording);
	VoiceRecordingAsyncAction->StopDelegate.AddDynamic(this, &UVoiceRecognitionSubsystem::OnRecordStop);

	return true;
}

void UVoiceRecognitionSubsystem::Stop(const UObject* WorldContextObject)
{
	if(VoiceRecordingAsyncAction)
	{
		VoiceRecordingAsyncAction->StopVoiceRecording(WorldContextObject);
	}

	if(VoiceRecognitionAsyncAction)
	{
		VoiceRecognitionAsyncAction->Cancel();
	}
}

void UVoiceRecognitionSubsystem::OnRecordStart(const float EnvelopeValue, USoundWave* SoundWave, const FString FilePath)
{
	StartDelegate.Broadcast(FString());
}

void UVoiceRecognitionSubsystem::OnRecording(const float EnvelopeValue, USoundWave* SoundWave, const FString FilePath)
{
	RecordingDelegate.Broadcast(FString());
}

void UVoiceRecognitionSubsystem::OnRecordStop(const float EnvelopeValue, USoundWave* SoundWave, const FString FilePath)
{

	UE_LOG(LogVoiceRecognitionSubsystem, Display, TEXT("ThreadID:%d, %s: Voice record completed"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	
	const UVoiceRecognitionSettings* Settings = UVoiceRecognitionSettings::Get();

	AbsoluteFilePath = FilePath;
	VoiceRecognitionAsyncAction = UVoiceRecognitionAsyncAction::Connect(this, Settings->ServerUrl, TArray<FString>(), TMap<FString,FString>());
	if(!VoiceRecognitionAsyncAction)
	{
		UE_LOG(LogVoiceRecognitionSubsystem, Error, TEXT("ThreadID:%d, %s: Voice recognition server connection error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return;
	}
	
	VoiceRecognitionAsyncAction->ConnectedDelegate.AddDynamic(this, &UVoiceRecognitionSubsystem::OnServerConnected);
	VoiceRecognitionAsyncAction->ProcessDelegate.AddDynamic(this, &UVoiceRecognitionSubsystem::OnServerResponse);
	VoiceRecognitionAsyncAction->ErrorDelegate.AddDynamic(this, &UVoiceRecognitionSubsystem::OnServerConnectError);

	VoiceRecognitionAsyncAction->Activate();
}

void UVoiceRecognitionSubsystem::OnServerConnected(UReceiveMessageBase* ReceiveMessageObj)
{
	VoiceRecognitionAsyncAction->Send(Language, AbsoluteFilePath);
}

void UVoiceRecognitionSubsystem::OnServerResponse(UReceiveMessageBase* ReceiveMessageObj)
{
	const FString RecognizedText = ReceiveMessageObj->Message;
	UE_LOG(LogVoiceRecognitionSubsystem, Display, TEXT("ThreadID:%d, %s: Recognized text is : %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *RecognizedText);
	CompletedDelegate.Broadcast(RecognizedText);
	
	VoiceRecognitionAsyncAction->Cancel();
}

void UVoiceRecognitionSubsystem::OnServerConnectError(UReceiveMessageBase* ReceiveMessageObj)
{
	const FString ErrorMessage = ReceiveMessageObj->Message;
	UE_LOG(LogVoiceRecognitionSubsystem, Error, TEXT("ThreadID:%d, %s: Voice recognition server connecting error! Error message : %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *ErrorMessage);
	
	ErrorDelegate.Broadcast(ErrorMessage);
}
