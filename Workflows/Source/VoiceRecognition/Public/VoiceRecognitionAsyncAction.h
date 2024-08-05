// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RequestAsyncActionBase.h"
#include "VoiceRecognitionAsyncAction.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, meta = (NotPlaceable, ExposedAsyncProxy = RequestTask))
class VOICERECOGNITION_API UVoiceRecognitionAsyncAction final : public UWebSocketRequestAsyncActionBase
{
	GENERATED_BODY()
public:
	UVoiceRecognitionAsyncAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 链接语音识别服务器 */
	UFUNCTION(BlueprintCallable, Category = "Workflow|Websocket|Voice Recognition Request", meta = (DisplayName = "Start Voice Recognition Request", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject", AutoCreateRefTerm = "InProtocols,InHeaders"))
	static UVoiceRecognitionAsyncAction* Connect(const UObject* WorldContextObject, const FString& InUrl, const TArray<FString>& InProtocols, const TMap<FString, FString>& InHeaders);

	/** 取消链接语音识别服务器 */
	UFUNCTION(BlueprintCallable, Category = "Workflow|Websocket|Voice Recognition Request")
	virtual void Cancel() override;

	/** 发送本地音频文件请求到语音识别服务器 */
	UFUNCTION(BlueprintCallable, Category = "Workflow|Websocket|Voice Recognition Request")
	bool Send(const FString& InLanguage, const FString& InAbsoluteFilePath);
	
	UPROPERTY()
	UReceiveMessageBase* ReceiveMessageBase;

private:
	virtual void InitWebSocket(const FString& InUrl, const TArray<FString>& InProtocols = TArray<FString>(), const TMap<FString, FString>& InHeaders = TMap<FString, FString>()) override;
	virtual void OnConnectedInternal() override;
	virtual void OnConnectionErrorInternal(const FString& Error) override;
	virtual void OnClosedInternal(int32 StatusCode, const FString& Reason, bool bWasClean) override;
	virtual void OnMessageInternal(const FString& MessageString) override;
};
