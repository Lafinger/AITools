// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "VoiceRecognitionSubsystem.generated.h"

class UAudioCaptureComponent;
class UVoiceRecordingAsyncAction;
class UVoiceRecognitionAsyncAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVoiceRecognitionDelegate, const FString&, Text);

DECLARE_LOG_CATEGORY_EXTERN(LogVoiceRecognitionSubsystem, Log, All);

/**
 * 
 */
UCLASS()
class VOICERECOGNITION_API UVoiceRecognitionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UVoiceRecognitionSubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/** 语音识别开始 */
	UPROPERTY(BlueprintAssignable, Category = "Workflow|Voice Recognition")
	FVoiceRecognitionDelegate StartDelegate;

	/** 语音识别中 */
	UPROPERTY(BlueprintAssignable, Category = "Workflow|Voice Recognition")
	FVoiceRecognitionDelegate RecordingDelegate;
	
	/** 语音识别完成 */
	UPROPERTY(BlueprintAssignable, Category = "Workflow|Voice Recognition")
	FVoiceRecognitionDelegate CompletedDelegate;

	/** 语音识别出错 */
	UPROPERTY(BlueprintAssignable, Category = "Workflow|Voice Recognition")
	FVoiceRecognitionDelegate ErrorDelegate;
	
	/** 开始语音识别 */
	UFUNCTION(BlueprintCallable, Category = "Workflow|Voice Recognition", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	bool Start(const UObject* WorldContextObject, const float TimeVAD, const FString& InLanguage, UAudioCaptureComponent* InAudioCaptureComponent, USoundSubmix* InSoundSubmix = nullptr, USoundWave* InExistingSoundWaveToOverwrite = nullptr);

	/** 停止语音识别 */
	UFUNCTION(BlueprintCallable, Category = "Workflow|Voice Recognition", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	void Stop(const UObject* WorldContextObject);


	UFUNCTION()
	void OnRecordStart(const float EnvelopeValue, USoundWave* SoundWave, const FString FilePath);
	
	UFUNCTION()
	void OnRecording(const float EnvelopeValue, USoundWave* SoundWave, const FString FilePath);
	
	UFUNCTION()
	void OnRecordStop(const float EnvelopeValue, USoundWave* SoundWave, const FString FilePath);

	UFUNCTION()
	void OnServerConnected(UReceiveMessageBase* ReceiveMessageObj);

	UFUNCTION()
	void OnServerResponse(UReceiveMessageBase* ReceiveMessageObj);
	
	UFUNCTION()
	void OnServerConnectError(UReceiveMessageBase* ReceiveMessageObj);

	UPROPERTY()
	UVoiceRecordingAsyncAction* VoiceRecordingAsyncAction;

	UPROPERTY()
	UVoiceRecognitionAsyncAction* VoiceRecognitionAsyncAction;

	FString Language;
	FString AbsoluteFilePath;
};
