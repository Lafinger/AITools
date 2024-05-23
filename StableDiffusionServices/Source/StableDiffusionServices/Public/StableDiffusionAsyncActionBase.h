// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "StableDiffusionAsyncActionBase.generated.h"

// DECLARE_LOG_CATEGORY_EXTERN(LogSDWebSocketAsyncAction, Log, All);
// DECLARE_LOG_CATEGORY_EXTERN(LogSDHTTPAsyncAction, Log, All);

// #ifndef UE_LOG_SD_WEBSOCKET
// #define UE_LOG_SD_WEBSOCKET(Verbosity, Format, ...) \
// 	UE_LOG(LogStableDiffusionWebSocketAsyncActionBase, Verbosity, TEXT("[TID]:%d, [%s]: %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), Format, ##__VA_ARGS__);
// #endif
//
// #ifndef UE_LOG_SD_HTTP
// #define UE_LOG_SD_HTTP(Verbosity, ElapsedTime, Format, ...) \
// 	UE_LOG(LogStableDiffusionWebSocketAsyncActionBase, Verbosity, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), ElapsedTime, Format, ##__VA_ARGS__);
// #endif

/*-------------------------------------- AsyncActionBase ------------------------------------*/
class UStableDiffusionOnlyStringOutput;

UCLASS(Abstract)
class UStableDiffusionOutputsBase : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, meta = (DeterminesOutputType = "ClassType", DynamicOutputParam = "Output"))
	void GetSpecifiedOutput(TSubclassOf<UStableDiffusionOutputsBase> ClassType, UStableDiffusionOutputsBase*& Output){};
};

UCLASS(BlueprintType)
class UStableDiffusionStringOnlyOutput final : public UStableDiffusionOutputsBase
{
	GENERATED_BODY()
	
public:
	UStableDiffusionStringOnlyOutput(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
	: Super(ObjectInitializer)
	{
		
	};
	
	UPROPERTY(BlueprintReadOnly)
	FString Message;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStableDiffusionBaseDelegate, UStableDiffusionOutputsBase*, Output);

UCLASS(Abstract)
class STABLEDIFFUSIONSERVICES_API UStableDiffusionAsyncActionBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UStableDiffusionAsyncActionBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** UStableDiffusionAsyncActionBase interface */
	virtual void Cancel();

protected:
	// 每次的响应
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Process"))
	FStableDiffusionBaseDelegate ProcessDelegate;

	// 完成
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Completed"))
	FStableDiffusionBaseDelegate CompletedDelegate;

	// 取消
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Cancel"))
	FStableDiffusionBaseDelegate CancelDelegate;

	// 失败
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Error"))
	FStableDiffusionBaseDelegate ErrorDelegate;
	
	// 是否取消
	bool bIsCancelAsyncAction;

	/** UBlueprintAsyncActionBase interface */
	virtual void SetReadyToDestroy() override;
};

/*-------------------------------------- AsyncActionBase ------------------------------------*/


/*-------------------------------------- WebSocket ------------------------------------*/
DECLARE_LOG_CATEGORY_EXTERN(LogSDWebSocketAsyncAction, Log, All);

class IWebSocket;

UCLASS(Abstract)
class STABLEDIFFUSIONSERVICES_API UStableDiffusionWebSocketAsyncActionBase : public UStableDiffusionAsyncActionBase
{
	GENERATED_BODY()

public:
	UStableDiffusionWebSocketAsyncActionBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** UBlueprintAsyncActionBase interface */
	virtual void Activate() override;

	/** UStableDiffusionAsyncActionBase */
	virtual void Cancel() override;

protected:
	// Must execute before Active()
	virtual void InitWebSocket(const FString& InUrl, const TMap<FString, FString>& InHeaders);
	virtual void OnConnectedInternal();
	virtual void OnConnectionErrorInternal(const FString& Error);
	virtual void OnClosedInternal(int32 StatusCode, const FString& Reason, bool bWasClean);
	virtual void OnMessageInternal(const FString& MessageString);

	TSharedPtr<IWebSocket> WebSocket;

	FString Url;
	TMap<FString, FString> Headers;
};

/*-------------------------------------- UStableDiffusionWebSocketAsyncActionBase ------------------------------------*/


/*-------------------------------------- HTTP ------------------------------------*/
DECLARE_LOG_CATEGORY_EXTERN(LogSDHTTPAsyncAction, Log, All);

UENUM(BlueprintType)
enum EHTTPRequestType : uint8
{
	GET,
	POST,
	HEAD,
	PUT
};

UCLASS(Abstract)
class STABLEDIFFUSIONSERVICES_API UStableDiffusionHTTPAsyncActionBase : public UStableDiffusionAsyncActionBase
{
	GENERATED_BODY()

public:
	UStableDiffusionHTTPAsyncActionBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** UBlueprintAsyncActionBase interface */
	virtual void Activate() override;

	/** UStableDiffusionAsyncActionBase */
	virtual void Cancel() override;

protected:
	// Must execute before Active()
	virtual void InitHTTP(const FString& InUrl, EHTTPRequestType InRequestType, const TMap<FString, FString>& InHeaders, const FString& InRequestBody, int32 InTimeout = 120);
	// virtual void OnHeaderReceivedInternal(FHttpRequestPtr Request, const FString& HeaderName,
	//                                       const FString& NewHeaderValue);
	virtual void OnRequestProgressInternal(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived);
	virtual void OnProcessRequestCompleteInternal(FHttpRequestPtr Request, FHttpResponsePtr Response,
	                                              bool bConnectedSuccessfully);

	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest;

	FString Url;
	EHTTPRequestType RequestType;
	TMap<FString, FString> Headers;
	FString RequestBody;
	int Timeout;
};

/*-------------------------------------- HTTP ------------------------------------*/
