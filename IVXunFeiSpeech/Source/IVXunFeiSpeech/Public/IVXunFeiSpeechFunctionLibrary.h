// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IVXunFeiSpeechTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IVXunFeiSpeechFunctionLibrary.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct IVXUNFEISPEECH_API FSparkDeskMessage
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "XunFeiSpeech")
	FString Role;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "XunFeiSpeech")
	FString Content;
};

UCLASS()
class IVXUNFEISPEECH_API UIVXunFeiSpeechFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**开始实时语音转文字*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Beign XunFei Real Time Speech To Text"), Category = "IVXunFeiSpeech|STT")
	static void XunFeiBeginRealTimeSpeechToText(
		FXunFeiRealTimeSTTOptions XunFeiRealTimeSTTOptions,
		FInitRealTimeSTTDelegate InitRealTimeSTTDelegate,
		FRealTimeSTTNoTranslateDelegate RealTimeSTTNoTranslateDelegate,
		FRealTimeSTTTranslateDelegate RealTimeSTTTranslateDelegate);

	/**停止实时语音转文字*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Stop XunFei Real Time Speech To Text"), Category = "IVXunFeiSpeech|STT")
	static void XunFeiStopRealTimeSpeechToText();

	/**开始流式语音转文字*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Beign XunFei Streaming Speech To Text"), Category = "IVXunFeiSpeech|STT")
	static void XunFeiBeginStreamingSpeechToText(
		FXunFeiStreamingSTTOptions XunFeiStreamingSTTOptions);

	/**停止流式语音转文字*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Stop XunFei Streaming Speech To Text"), Category = "IVXunFeiSpeech|STT")
	static void XunFeiStopStreamingSpeechToText();

	/**翻译*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Beign XunFei Translate"), Category = "IVXunFeiSpeech|Translate")
	static void XunFeiBeginTranslate(const FString& SourceText,  FXunFeiTranslateOptions XunFeiTranslateOptions, FTranslateCompletedDelegate TranslateCompletedDelegate);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Stop XunFei Translate"), Category = "IVXunFeiSpeech|Translate")
	static void XunFeiStopTranslate();

public:
	/**清除聊天记录*/
	UFUNCTION(BlueprintCallable, Category = "IVXunFeiSpeech|Chat")
	static void ClearHistory();

	static void AddHistory(FSparkDeskMessage InHistory);
public:
	static TArray<FSparkDeskMessage> History;
};
