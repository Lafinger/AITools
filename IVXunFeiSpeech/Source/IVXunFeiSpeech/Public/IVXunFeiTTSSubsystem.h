// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpFwd.h"
#include "IVXunFeiSpeechTypes.h"
#include "Subsystems/EngineSubsystem.h"
#include "IVXunFeiTTSSubsystem.generated.h"


class FIVVoiceRunnable;
class IWebSocket;

UENUM()
enum class EXunFeiRealTimeSTTStatus : uint8
{
	Ready,
	Init,
	Processing,
	Finish,
};


USTRUCT(BlueprintType)
struct FXunFeiRealTimeSTTTranslateData
{
	GENERATED_BODY()

	/**
	*业务标识字段，开启翻译功能后值为 trans 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	FString biz;

	/**
	 * 目标语种翻译文本结果
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	FString dst;
	/**
	 * 目标语种翻译文本结果
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	bool isEnd=false;

	/**
	 * 转写结果序号,从0开始
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	int32 segId=INDEX_NONE;

	/**
	 * 送翻译的原始文本,音频对应的识别文本
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	FString src;

	/**
	 * 结果类型标识,0-最终结果；1-中间结果
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	int32 type=INDEX_NONE;


	/**
	 *句子在整段语音中的开始时间，单位毫秒(ms)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	int32 bg=INDEX_NONE;


	/**
	 * 句子在整段语音中的结束时间，单位毫秒(ms)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	int32 ed=INDEX_NONE;

};

USTRUCT(BlueprintType)
struct FXunFeiRealTimeSTTNoTranslateCWData
{
	GENERATED_BODY()

	/**
	 * 词识别结果
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	FString w;

	/**
	 * 词标识
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	FString wp;
};


USTRUCT(BlueprintType)
struct FXunFeiRealTimeSTTNoTranslateWSData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	TArray<FXunFeiRealTimeSTTNoTranslateCWData> cw;
	/**
	* T词在本句中的开始时间，单位是帧，1帧=10ms,即词在整段语音中的开始时间为(bg+wb*10)ms,中间结果的 wb 为 0
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	int32 wb=INDEX_NONE;

	/**
	* 词在本句中的结束时间，单位是帧，1帧=10ms,即词在整段语音中的结束时间为(bg+we*10)ms,中间结果的 we 为 0
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	int32 we=INDEX_NONE;
};


USTRUCT(BlueprintType)
struct FXunFeiRealTimeSTTNoTranslateRTData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	TArray<FXunFeiRealTimeSTTNoTranslateWSData> ws;
};

USTRUCT(BlueprintType)
struct FXGXunFeiRealTimeSTTNoTranslateSTData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	FString bg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	FString ed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	TArray<FXunFeiRealTimeSTTNoTranslateRTData> rt;


	/**
	 * 结果类型标识,0-最终结果；1-中间结果
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	FString type;
};


USTRUCT(BlueprintType)
struct FXunFeiRealTimeSTTNoTranslateCNData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	FXGXunFeiRealTimeSTTNoTranslateSTData st;
};

USTRUCT(BlueprintType)
struct FXunFeiRealTimeSTTNoTranslateData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	FXunFeiRealTimeSTTNoTranslateCNData cn;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG XunFei RealTime STT")
	int32 seg_id=INDEX_NONE;
};


/**
 * 
 */
UCLASS()
class IVXUNFEISPEECH_API UIVXunFeiTTSSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FStreamingSTTInitdDelegate StreamingSTTInitdDelegate;
	UPROPERTY(BlueprintAssignable)
	FStreamingSTTResponseDelegate StreamingSTTResponseDelegate;
	UPROPERTY(BlueprintAssignable)
	FStreamingSTTCompletedDelegate StreamingSTTCompletedDelegate;
	UPROPERTY(BlueprintAssignable)
	FStreamingSTTErrorDelegate StreamingSTTErrorDelegate;
	
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

public:
	void BeginRealTimeSpeechToText(
	FXunFeiRealTimeSTTOptions& RealTimeSTTOptions,
	FInitRealTimeSTTDelegate InInitRealTimeSTTDelegate,
	FRealTimeSTTNoTranslateDelegate InRealTimeSTTNoTranslateDelegate,
	FRealTimeSTTTranslateDelegate InRealTimeSTTTranslateDelegate);

	void StopRealTimeSpeechToText();
	
	void BeginStreamingSpeechToText(FXunFeiStreamingSTTOptions& InStreamingSTTOptions);
	void StopStreamingSpeechToText();

	void BeginTranslate(const FString& SourceText, FXunFeiTranslateOptions& XunFeiTranslateOptions, FTranslateCompletedDelegate InTranslateCompletedDelegate);
	void StopTranslate();

private:
	void OnRealTimeSTTConnected();;
	void OnRealTimeSTTConnectionError(const FString& Error);
	void OnRealTimeSTTClosed(const int32 Status, const FString& Reason, const bool bWasClean);
	void OnRealTimeSTTMessage(const FString& Message);
	void OnRealTimeSTTMessageSent(const FString& Message);

	void OnStreamingSTTConnected();;
	void OnStreamingSTTConnectionError(const FString& Error);
	void OnStreamingSTTClosed(const int32 Status, const FString& Reason, const bool bWasClean);
	void OnStreamingSTTMessage(const FString& Message);
	void OnStreamingSTTMessageSent(const FString& Message);

	void OnTranslateCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	

	void CallInitRealTimeSTTDelegate(bool bInitResult,FString InitMessage);

	void CallRealTimeSTTNoTranslateDelegate(FString InSrcText);
	void CallRealTimeSTTTranslateDelegate(FString InSrcText, FString InDstText);

	void SendRealTimeSTTVoiceData( TArray<float>& InVoiceData);
	void SendStreamingSTTVoiceData( TArray<float>& InVoiceData);

	void RealeaseVoiceConsumeRunnable();

	void EndRealTimeSTTSendVoiceData();
	void EndStreamingSTTSendVoiceData();
	

protected:
	
	TSharedPtr<IWebSocket> Socket;
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest;

	EXunFeiRealTimeSTTStatus ReakTimeSTTStatus = EXunFeiRealTimeSTTStatus::Ready;

	TSharedPtr<FIVVoiceRunnable> VoiceRunnable;

	FCriticalSection SocketCriticalSection;

	// RealTimeSTT
	FInitRealTimeSTTDelegate InitRealTimeSTTDelegate;
	FRealTimeSTTTranslateDelegate RealTimeSTTTranslateDelegate;
	FRealTimeSTTNoTranslateDelegate RealTimeSTTNoTranslateDelegate;

	// StreamingSTT
	FString StreamingSTTResult;
	FString LastStreamingSTTResult;
	FXunFeiStreamingSTTOptions StreamingSTTOptions;

	// Translate
	FTranslateCompletedDelegate TranslateCompletedDelegate;
};
