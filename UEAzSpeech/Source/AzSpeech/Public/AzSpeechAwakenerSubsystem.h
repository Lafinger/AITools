// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

THIRD_PARTY_INCLUDES_START
#include <speechapi_cxx_keyword_recognition_model.h>
#include <speechapi_cxx_speech_recognizer.h>
THIRD_PARTY_INCLUDES_END

#include "AzSpeechAwakenerSubsystem.generated.h"

namespace MicrosoftSpeech = Microsoft::CognitiveServices::Speech;

using MicrosoftKeywordModel = std::shared_ptr<MicrosoftSpeech::KeywordRecognitionModel>;
using MicrosoftSpeechRecognizer = std::shared_ptr<MicrosoftSpeech::SpeechRecognizer>;


DECLARE_LOG_CATEGORY_EXTERN(LogAzSpeechAwakenerSubsystem, Log, All);


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAwakenerDelegate);

/**
 * 
 */
UCLASS()
class AZSPEECH_API UAzSpeechAwakenerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	// /** 语音唤醒开始委托 */
	// UPROPERTY(BlueprintAssignable, Category = "AzSpeech|AzSpeechAwakenerSubsystem")
	// FAwakenerDelegate AwakenerStartDelegate;
	
	/** 语音唤醒触发委托 */
	UPROPERTY(BlueprintAssignable, Category = "AzSpeech|AzSpeechAwakenerSubsystem")
	FAwakenerDelegate AwakenerTriggerDelegate;
	
	// /** 语音唤醒完成委托 */
	// UPROPERTY(BlueprintAssignable, Category = "AzSpeech|AzSpeechAwakenerSubsystem")
	// FAwakenerDelegate AwakenerFinishedDelegate;
	
	// /** 语音唤醒出错委托 */
	// UPROPERTY(BlueprintAssignable, Category = "AzSpeech|AzSpeechAwakenerSubsystem")
	// FAwakenerDelegate AwakenerFailedDelegate;

	UAzSpeechAwakenerSubsystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** 开启异步语音唤醒任务 */
	UFUNCTION(BlueprintCallable, Category = "AzSpeech|AzSpeechAwakenerSubsystem")
	bool StartKeywordRecognitionAsync();

	/** 停止异步语音唤醒任务 */
	UFUNCTION(BlueprintCallable, Category = "AzSpeech|AzSpeechAwakenerSubsystem")
	void StopKeywordRecognitionAsync();

private:
	MicrosoftKeywordModel Model;
	MicrosoftSpeechRecognizer SpeechRecognizer;
	bool bIsRunning;
};
