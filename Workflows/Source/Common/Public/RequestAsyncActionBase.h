// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "RequestAsyncActionBase.generated.h"


UCLASS(BlueprintType)
class COMMON_API UReceiveMessageBase : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, meta = (DeterminesOutputType = "ClassType", DynamicOutputParam = "Output"))
	static void GetSpecified(TSubclassOf<UReceiveMessageBase> ClassType, UReceiveMessageBase*& Output){};

	/* Raw message from HTTP or WebSocket */
	UPROPERTY(BlueprintReadOnly)
	FString Message; 
};

/*-------------------------------------- AsyncActionBase ------------------------------------*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRequestBaseDelegate, UReceiveMessageBase*, ReceiveMessageObj);

UCLASS(Abstract)
class COMMON_API URequestAsyncActionBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	URequestAsyncActionBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	/** UBlueprintAsyncActionBase interface */
	virtual void SetReadyToDestroy() override;
	
	/** URequestAsyncActionBase interface */
	virtual void Cancel();

	// 连接成功
	UPROPERTY(BlueprintAssignable, Category = "Common|Base", meta=(DisplayName = "Connected"))
	FRequestBaseDelegate ConnectedDelegate;
	
	// 每次的响应
	UPROPERTY(BlueprintAssignable, Category = "Common|Base", meta=(DisplayName = "Process"))
	FRequestBaseDelegate ProcessDelegate;

	// 完成
	UPROPERTY(BlueprintAssignable, Category = "Common|Base", meta=(DisplayName = "Completed"))
	FRequestBaseDelegate CompletedDelegate;

	// 取消
	UPROPERTY(BlueprintAssignable, Category = "Common|Base", meta=(DisplayName = "Cancel"))
	FRequestBaseDelegate CancelDelegate;

	// 失败
	UPROPERTY(BlueprintAssignable, Category = "Common|Base", meta=(DisplayName = "Error"))
	FRequestBaseDelegate ErrorDelegate;

private:
	// 是否取消
	bool bIsCancel;
};

/*-------------------------------------- AsyncActionBase ------------------------------------*/


/*-------------------------------------- WebSocket ------------------------------------*/
DECLARE_LOG_CATEGORY_EXTERN(LogWebSocketRequestAsyncAction, Log, All);

class IWebSocket;
UCLASS(Abstract)
class COMMON_API UWebSocketRequestAsyncActionBase : public URequestAsyncActionBase
{
	GENERATED_BODY()

public:
	UWebSocketRequestAsyncActionBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** UBlueprintAsyncActionBase interface */
	virtual void Activate() override;

	/** URequestAsyncActionBase interface */
	virtual void Cancel() override;

protected:
	/** Must execute before Active() */
	virtual void InitWebSocket(const FString& InUrl, const TArray<FString>& InProtocols = TArray<FString>(), const TMap<FString, FString>& InHeaders = TMap<FString, FString>());

	virtual bool SendString(const FString& InData);
	virtual bool SendBinary(const void* InData, SIZE_T InSize, bool bIsBinary = false);
	
	/** Callback function */
	virtual void OnConnectedInternal();
	virtual void OnConnectionErrorInternal(const FString& Error);
	virtual void OnClosedInternal(int32 StatusCode, const FString& Reason, bool bWasClean);
	virtual void OnMessageInternal(const FString& MessageString);

private:
	TSharedPtr<IWebSocket, ESPMode::ThreadSafe> WebSocket;
	FString Url;
	TArray<FString> Protocols;
	TMap<FString, FString> Headers;
};

/*-------------------------------------- UStableDiffusionWebSocketAsyncActionBase ------------------------------------*/


/*-------------------------------------- HTTP ------------------------------------*/
DECLARE_LOG_CATEGORY_EXTERN(LogHttpRequestAsyncAction, Log, All);

UCLASS(Abstract)
class COMMON_API UHttpRequestAsyncActionBase : public URequestAsyncActionBase
{
	GENERATED_BODY()

public:
	UHttpRequestAsyncActionBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** UBlueprintAsyncActionBase interface */
	virtual void Activate() override;

	/** URequestAsyncActionBase interface */
	virtual void Cancel() override;

protected:
	/** Must execute before Active() */
	virtual void InitHttp(const FString& InUrl, const FString& InRequestType, const TMap<FString, FString>& InHeaders, const FString& InRequestBody, int32 InTimeout = 120);
	
	/** Callback function */
	virtual void OnRequestProgressInternal(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived);
	virtual void OnProcessRequestCompletedInternal(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest;
	FString Url;
	FString RequestType;
	TMap<FString, FString> Headers;
	FString RequestBody;
	int Timeout;
};

/*-------------------------------------- HTTP ------------------------------------*/
