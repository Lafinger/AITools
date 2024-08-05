// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioCaptureComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "VoiceRecordingAsyncAction.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVoiceRecordingAsyncAction, Log, All);

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRecordingDelegate, const float, EnvelopeValue, USoundWave*, SoundWave, const FString, FilePath);


UCLASS(BlueprintType, meta = (NotPlaceable, ExposedAsyncProxy = RequestTask))
class VOICERECOGNITION_API UVoiceRecordingAsyncAction final: public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/** 开始 */
	UPROPERTY(BlueprintAssignable, Category = "Workflow|Voice Recording", meta=(DisplayName = "Start"))
	FRecordingDelegate StartDelegate;

	/** 录制 */
	UPROPERTY(BlueprintAssignable, Category = "Workflow|Voice Recording", meta=(DisplayName = "Recording"))
	FRecordingDelegate RecordingDelegate;
	
	/** 停止 */
	UPROPERTY(BlueprintAssignable, Category = "Workflow|Voice Recording", meta=(DisplayName = "Stop"))
	FRecordingDelegate StopDelegate;
	
	UVoiceRecordingAsyncAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 开始录制本地语音 */
	UFUNCTION(BlueprintCallable, Category = "Workflow|Voice Recording", meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UVoiceRecordingAsyncAction* Start(
		const UObject* WorldContextObject,
		const EAudioRecordingExportType InOutputType,
		const FString& InOutputDir,
		const FString& InOutputName,
		const float TimeVAD,
		UAudioCaptureComponent* InAudioCaptureComponent,
		USoundSubmix* InSoundSubmix = nullptr,
		USoundWave* InExistingSoundWaveToOverwrite = nullptr);

	/** 保存语音录制到本地（指定参数） */
	UFUNCTION(BlueprintCallable, Category = "Workflow|Voice Recording", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	void StopAudioRecordingOverwriteOutput(
		const UObject* WorldContextObject,
		const EAudioRecordingExportType InOutputType,
		const FString& InOutputDir,
		const FString& InOutputName,
		USoundSubmix* InSoundSubmix,
		USoundWave* InExistingSoundWaveToOverwrite);

	/** 保存语音录制到本地（默认参数） */
	UFUNCTION(BlueprintCallable, Category = "Workflow|Voice Recording", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	void StopVoiceRecording(const UObject* WorldContextObject);

private:
	bool bIsRecording;
	float SilenceDuration;
	float SilenceThreshold; // 静音阈值
	float MaxSilenceDuration; // 最大静音时间
	FTimerHandle SilenceCheckTimerHandle;
	FDelegateHandle AudioEnvelopeDelegateHandle;
	
	EAudioRecordingExportType OutputType;
	FString OutputDir;
	FString OutputName;
	
	UPROPERTY()
	UAudioCaptureComponent* AudioCaptureComponent;
	UPROPERTY()
	USoundSubmix* SoundSubmix;
	UPROPERTY()
	USoundWave* ExistingSoundWaveToOverwrite;
	
};
