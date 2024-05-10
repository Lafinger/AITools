// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IVXunFeiSpeechTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UObject/NoExportTypes.h"
#include "IVXunFeiNodes.generated.h"

class IWebSocket;






UCLASS(Abstract,NotPlaceable)
class IVXUNFEISPEECH_API UIVFunFeiSpeechAsyncProxyBase  : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	friend class UFunFeiSpeechTaskStatus;
public:
	
	UFUNCTION(BlueprintCallable, Category=IVXunFeiSpeech)
	void Cancel();
	
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

	void SendMessage(const FString& Message);
	bool IsConnected() const;
	
protected:

	virtual void OnConnectedInternal();
	virtual void OnConnectionErrorInternal(const FString& Error);
	virtual void OnCloseInternal(const int32 Status, const FString& Reason, const bool bWasClean);
	virtual void OnMessageInternal(const FString& Message);

	virtual bool StartTaskWork();
	virtual void Connect();

	void CreateURL(FString APISecret,FString APIKey);
	
#if WITH_EDITOR
	bool bIsEditorTask = false;
	bool bEndingPIE = false;

	virtual void PrePIEEnded(bool bIsSimulating);
#endif
	
protected:
	TSharedPtr<IWebSocket> Socket;
	
	FName TaskName = NAME_None;

	FString Host;
	FString Path;
	FString Url;
	
private:
	bool bIsTaskActive = false;
	bool bIsReadyToDestroy = false;
	

};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSparkDeskEvent, const FString&, Message);


UCLASS(meta = (NotPlaceable,ExposedAsyncProxy = Task))
class IVXUNFEISPEECH_API UIVSparkDeskWebSocketAsyncProxy  final : public UIVFunFeiSpeechAsyncProxyBase
{
	GENERATED_BODY()
public:
	UIVSparkDeskWebSocketAsyncProxy(const FObjectInitializer& ObjectInitializer);

	/* WebSocket 连接到服务器后调用。 */
	UPROPERTY(BlueprintAssignable)
	FSparkDeskEvent OnConnected;
    
	/* 当WebSocket无法连接到服务器时调用。 */
	UPROPERTY(BlueprintAssignable)
	FSparkDeskEvent OnConnectionError;
    
	/* 当与服务器的连接关闭时调用。 */
	UPROPERTY(BlueprintAssignable)
	FSparkDeskEvent OnClose;

	/* 服务器发送消息时调用。 */
	UPROPERTY(BlueprintAssignable)
	FSparkDeskEvent OnMessage;

public:
	virtual void Connect() override;
	
public:
	/**
	* 建立星火大模型连接，并发送信息。
	* @param Message 发送的信息。
	*/
	UFUNCTION(BlueprintCallable, Category="AIGC|XunFei", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Spark Desk Chat"))
	static UIVSparkDeskWebSocketAsyncProxy* SparkDeskChat( FString Message,FSparkDeskOptions SparkDeskOptions,bool bSplice);

	UFUNCTION(BlueprintCallable, Category="AIGC|XunFei", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Spark Desk Chat Texture Understanding"))
	static UIVSparkDeskWebSocketAsyncProxy* SparkDeskChaTextureUnderstanding(FString Message,UTexture2D* Texture,FSparkDeskOptions SparkDeskOptions);
	
	UFUNCTION(BlueprintCallable, Category="AIGC|XunFei", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Spark Desk Chat Explorer Image Understanding"))
	static UIVSparkDeskWebSocketAsyncProxy* SparkDeskChaExplorerImageUnderstanding(FString Message, const FString& DefaultPath,FSparkDeskOptions SparkDeskOptions);
protected:

	virtual void OnConnectedInternal() override;
	virtual void OnConnectionErrorInternal(const FString& Error) override;
	virtual void OnCloseInternal(const int32 Status, const FString& Reason, const bool bWasClean) override;
	virtual void OnMessageInternal(const FString& Message) override ;

private:
	int32 CaculateTokenCount(FString Message);
	
	FString GenerateSocketJsonString(FString NewMessage);
	
private:

	FString RequestMessage;
	FString ResponseMessage;

	bool bImageUnderstanding;
	bool bSplice;

	FSparkDeskOptions SparkDeskOptions;
};


UCLASS(Abstract,NotPlaceable)
class IVXUNFEISPEECH_API UIVXunFeiTTSWebSocketAsyncProxyBase  : public UIVFunFeiSpeechAsyncProxyBase
{
	GENERATED_BODY()
public:
	UIVXunFeiTTSWebSocketAsyncProxyBase(const FObjectInitializer& ObjectInitializer);

protected:
	FString GenerateSocketJsonString(FString NewMessage);
	void ConvertPCMToWave(const TArray<uint8>& InPCMData, TArray<uint8>& OutWaveData);
	
	FXunFeiTTSOptions TTSOptions;;

	FString RequestMessage;

	TArray<uint8> XunFeiAudioData;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FXunFeiTTSEvent, USoundWave*, SoundWave, const float& ,SoundDuration);
DECLARE_DELEGATE_TwoParams(FXunFeiTTSEventC,USoundWave*, const float&)
UCLASS(meta = (NotPlaceable,ExposedAsyncProxy = Task))
class IVXUNFEISPEECH_API UIVXunFeiTTSWebSocketAsyncProxy  final : public UIVXunFeiTTSWebSocketAsyncProxyBase
{
	GENERATED_BODY()
public:
	UIVXunFeiTTSWebSocketAsyncProxy(const FObjectInitializer& ObjectInitializer);

	/* WebSocket 连接到服务器后调用。 */
	UPROPERTY(BlueprintAssignable)
	FXunFeiTTSEvent OnConnected;
    
	/* 当WebSocket无法连接到服务器时调用。 */
	UPROPERTY(BlueprintAssignable)
	FXunFeiTTSEvent OnConnectionError;
    
	/* 当与服务器的连接关闭时调用。 */
	UPROPERTY(BlueprintAssignable)
	FXunFeiTTSEvent OnClose;
	
	/**文字转SoundWave*/
	UFUNCTION(BlueprintCallable, Category="AIGC|XunFei", meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Xun Fei Text To Sound Wave"))
	static UIVXunFeiTTSWebSocketAsyncProxy*  XunFeiTextToSoundWave(const UObject* WorldContextObject, FString Message, FXunFeiTTSOptions TTSOptions);

public:
	virtual void Connect() override;
	virtual void SetReadyToDestroy() override;

public:
	FXunFeiTTSEventC OnConnectedC;
	FXunFeiTTSEventC OnConnectionErrorC;
	FXunFeiTTSEventC OnCloseC;
protected:
	virtual void OnConnectedInternal() override;
	virtual void OnConnectionErrorInternal(const FString& Error) override;
	virtual void OnCloseInternal(const int32 Status, const FString& Reason, const bool bWasClean) override;
	virtual void OnMessageInternal(const FString& Message) override ;
	
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FXunFeiTTSWavFileEvent,const float& ,SoundDuration);
UCLASS(meta = (NotPlaceable,ExposedAsyncProxy = Task))
class IVXUNFEISPEECH_API UIVXunFeiTTSWavFileWebSocketAsyncProxy  final : public UIVXunFeiTTSWebSocketAsyncProxyBase
{
	GENERATED_BODY()
public:
	UIVXunFeiTTSWavFileWebSocketAsyncProxy(const FObjectInitializer& ObjectInitializer);

	/* WebSocket 连接到服务器后调用。 */
	UPROPERTY(BlueprintAssignable)
	FXunFeiTTSWavFileEvent OnConnected;
    
	/* 当WebSocket无法连接到服务器时调用。 */
	UPROPERTY(BlueprintAssignable)
	FXunFeiTTSWavFileEvent OnConnectionError;
    
	/* 当与服务器的连接关闭时调用。 */
	UPROPERTY(BlueprintAssignable)
	FXunFeiTTSWavFileEvent OnClose;
	
	/**语音合成并保存*/
	UFUNCTION(BlueprintCallable, Category="AIGC|XunFei", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Xun Fei Text To .wav File"))
	static UIVXunFeiTTSWavFileWebSocketAsyncProxy*  XunFeiTextToWavFile( FString Message,FXunFeiTTSOptions TTSOptions,const FString& FilePath);

public:
	virtual void Connect() override;

protected:
	virtual void OnConnectedInternal() override;
	virtual void OnConnectionErrorInternal(const FString& Error) override;
	virtual void OnCloseInternal(const int32 Status, const FString& Reason, const bool bWasClean) override;
	virtual void OnMessageInternal(const FString& Message) override ;

private:
	FString FilePath;
};

DECLARE_DELEGATE_OneParam(FLoopTaskEvent,const float&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FXunFeiMultittsFrameEvent,const TArray<int32>&, Index,const TArray<USoundWave*>&, Sounds);
UCLASS(meta = (NotPlaceable,ExposedAsyncProxy = Task))
class IVXUNFEISPEECH_API UIVXunFeiMultiFrameTTSAsyncProxy  : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UIVXunFeiMultiFrameTTSAsyncProxy(const FObjectInitializer& ObjectInitializer);
	
	
	UPROPERTY(BlueprintAssignable)
	FXunFeiMultittsFrameEvent Completed;
	
	UPROPERTY(BlueprintAssignable)
	FXunFeiMultittsFrameEvent OnCanceled;

	/**
	 * 用于多文本转换
	 * @param MessageArray 
	 * @param TTSOptions 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category="AIGC|XunFei", meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Xun Fei Multi Frame Text To Sound Wave"))
	static UIVXunFeiMultiFrameTTSAsyncProxy*  XunFeiNultiFrameTextToSoundWave(const UObject* WorldContextObject, const TArray<FString>& MessageArray,FXunFeiTTSOptions TTSOptions);

	UFUNCTION(BlueprintCallable, Category="AIGC|XunFei")
	void Cancel();

private:
	mutable TMap<uint32,TWeakObjectPtr<UIVXunFeiTTSWebSocketAsyncProxy>> TTSWebSocketAsyncProxyMap;
	mutable TMap<int32,TWeakObjectPtr<USoundWave>> SoundMap;
	mutable TMap<uint32,int32> QueueMap;

	int32 NumQueue;
	int32 TTSNum;
};

UCLASS(Category = "IVXunFeiSpeech")
class IVXUNFEISPEECH_API UFunFeiSpeechTaskStatus final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "IVXunFeiSpeech")
	static bool IsTaskActive(const UIVFunFeiSpeechAsyncProxyBase* const Test);

	UFUNCTION(BlueprintPure, Category = "IVXunFeiSpeech")
	static bool IsTaskReadyToDestroy(const UIVFunFeiSpeechAsyncProxyBase* const Test);

	UFUNCTION(BlueprintPure, Category = "IVXunFeiSpeech")
	static bool IsTaskStillValid(const UIVFunFeiSpeechAsyncProxyBase* const Test);
};

