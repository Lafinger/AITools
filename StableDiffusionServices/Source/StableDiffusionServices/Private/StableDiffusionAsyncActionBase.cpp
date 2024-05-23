// Fill out your copyright notice in the Description page of Project Settings.


#include "StableDiffusionAsyncActionBase.h"

#include "HttpModule.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"
#include "Interfaces/IHttpResponse.h"

/*-------------------------------------- AsyncActionBase ------------------------------------*/
UStableDiffusionAsyncActionBase::UStableDiffusionAsyncActionBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), bIsCancelAsyncAction(false)
{
	AddToRoot();
}

void UStableDiffusionAsyncActionBase::SetReadyToDestroy()
{
	RemoveFromRoot();
	Super::SetReadyToDestroy();
}

void UStableDiffusionAsyncActionBase::Cancel()
{
	bIsCancelAsyncAction = true;
	UStableDiffusionStringOnlyOutput* CancelOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
	CancelOutput->Message = TEXT("Cancel");
	CancelDelegate.Broadcast(CancelOutput);

	SetReadyToDestroy();
}

/*-------------------------------------- AsyncActionBase ------------------------------------*/


/*-------------------------------------- WebSocket ------------------------------------*/
DEFINE_LOG_CATEGORY(LogSDWebSocketAsyncAction);

UStableDiffusionWebSocketAsyncActionBase::UStableDiffusionWebSocketAsyncActionBase(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UStableDiffusionWebSocketAsyncActionBase::Activate()
{
	FString ProtocolTemp = Url.Left(3);
	FString Protocol = ProtocolTemp.Replace(*FString(TEXT(":")), *FString(TEXT("")));
	if(!Protocol.Equals(TEXT("wss")) && !Protocol.Equals(TEXT("ws")) && !Protocol.IsEmpty())
	{
		UE_LOG(LogSDWebSocketAsyncAction, Error, TEXT("ThreadID:%d, %s: Web socket protocol error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		UStableDiffusionStringOnlyOutput* ErrorOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
		ErrorOutput->Message = TEXT("Web socket protocol error.");
		ErrorDelegate.Broadcast(ErrorOutput);
		return;
	}
	
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url, Protocol, Headers);
	check(WebSocket && "Create web socket error!");

	WebSocket->OnConnected().AddUObject(this, &UStableDiffusionWebSocketAsyncActionBase::OnConnectedInternal);
	WebSocket->OnConnectionError().AddUObject(this, &UStableDiffusionWebSocketAsyncActionBase::OnConnectionErrorInternal);
	WebSocket->OnClosed().AddUObject(this, &UStableDiffusionWebSocketAsyncActionBase::OnClosedInternal);
	WebSocket->OnMessage().AddUObject(this, &UStableDiffusionWebSocketAsyncActionBase::OnMessageInternal);
	WebSocket->Connect();
}

void UStableDiffusionWebSocketAsyncActionBase::Cancel()
{
	if(WebSocket && WebSocket.IsValid())
	{
		WebSocket->Close();
	}
	Super::Cancel();
	UE_LOG(LogSDWebSocketAsyncAction, Warning, TEXT("[TID]:%d, [%s]: %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), TEXT("Cancel web socket!"));
}

void UStableDiffusionWebSocketAsyncActionBase::InitWebSocket(const FString& InUrl, const TMap<FString, FString>& InHeaders)
{
	if (InUrl.IsEmpty())
    {
		UE_LOG(LogSDWebSocketAsyncAction, Error, TEXT("[TID]:%d, [%s]: %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), TEXT("Init web socket url is empty!"));
		UStableDiffusionStringOnlyOutput* ErrorOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
		ErrorOutput->Message = TEXT("Init web socket url is empty!");
    	ErrorDelegate.Broadcast(ErrorOutput);
    	SetReadyToDestroy();
    	return;
    }
	
	Url = InUrl;
	Headers = InHeaders;
}


void UStableDiffusionWebSocketAsyncActionBase::OnConnectedInternal()
{
	UE_LOG(LogSDWebSocketAsyncAction, Display, TEXT("[TID]:%d, [%s]: %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), TEXT("Web socket connected."));
}

void UStableDiffusionWebSocketAsyncActionBase::OnConnectionErrorInternal(const FString& Error)
{
	UE_LOG(LogSDWebSocketAsyncAction, Error, TEXT("[TID]:%d, [%s]: %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), TEXT("Web socket connection error!"));
	UStableDiffusionStringOnlyOutput* ErrorOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
	ErrorOutput->Message = TEXT("Web socket connection error!");
	ErrorDelegate.Broadcast(ErrorOutput);
	SetReadyToDestroy();
}

void UStableDiffusionWebSocketAsyncActionBase::OnClosedInternal(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogSDWebSocketAsyncAction, Display, TEXT("[TID]:%d, [%s]: %s, StatusCode:%d, Reason:%s, bWasClean:%s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), TEXT("Close Web socket."), StatusCode, *Reason,  bWasClean ? TEXT("true") : TEXT("false"));
	UStableDiffusionStringOnlyOutput* CompletedOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
	CompletedOutput->Message = TEXT("Close Web socket.");
	CompletedDelegate.Broadcast(CompletedOutput);
	SetReadyToDestroy();
}

void UStableDiffusionWebSocketAsyncActionBase::OnMessageInternal(const FString& MessageString)
{
}

/*-------------------------------------- WebSocket ------------------------------------*/


/*-------------------------------------- HTTP ------------------------------------*/
DEFINE_LOG_CATEGORY(LogSDHTTPAsyncAction);

UStableDiffusionHTTPAsyncActionBase::UStableDiffusionHTTPAsyncActionBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), RequestType(GET), Timeout(120)
{
}

void UStableDiffusionHTTPAsyncActionBase::Activate()
{
	Super::Activate();
	
	FHttpModule* HttpModule = static_cast<FHttpModule*>(FModuleManager::Get().GetModule("HTTP"));
	HttpRequest = HttpModule->CreateRequest();
	check(HttpRequest && "Create http request error!");
	
	// Set URL
	HttpRequest->SetURL(Url);

	// Set Verb
	const UEnum* EBeTransformEnum = StaticEnum<EHTTPRequestType>();
	const FString TypeStr = EBeTransformEnum->GetNameStringByValue((uint8)RequestType);
	HttpRequest->SetVerb(TypeStr);

	// Set Headers
	for (TPair<FString, FString>& HeaderPair : Headers)
	{
		FString HeaderKey = HeaderPair.Key;
		FString HeaderValue = HeaderPair.Value;
		HttpRequest->SetHeader(HeaderKey, HeaderValue);
	}

	// Set Request Body
	if(!RequestBody.IsEmpty())
	{
		HttpRequest->SetContentAsString(RequestBody);
	}
	
	HttpRequest->SetTimeout(Timeout);
	// HttpRequest->OnHeaderReceived().BindUObject(this, &UStableDiffusionHTTPAsyncActionBase::OnHeaderReceivedInternal);
	HttpRequest->OnRequestProgress().BindUObject(this, &UStableDiffusionHTTPAsyncActionBase::OnRequestProgressInternal);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UStableDiffusionHTTPAsyncActionBase::OnProcessRequestCompleteInternal);
	HttpRequest->ProcessRequest();
}

void UStableDiffusionHTTPAsyncActionBase::Cancel()
{
	if(HttpRequest && HttpRequest.IsValid())
	{
		HttpRequest->CancelRequest();
	}
	
	Super::Cancel();
	UE_LOG(LogSDHTTPAsyncAction, Warning, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
	       FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__), TEXT("Cancel http request!"));
}

void UStableDiffusionHTTPAsyncActionBase::InitHTTP(const FString& InUrl, EHTTPRequestType InRequestType, const TMap<FString, FString>& InHeaders, const FString& InRequestBody, int32 InTimeout)
{
	if (InUrl.IsEmpty())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__), TEXT("Init Http url is empty!"));
		UStableDiffusionStringOnlyOutput* ErrorOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
		ErrorOutput->Message = TEXT("Init Http url is empty!");
		ErrorDelegate.Broadcast(ErrorOutput);
		SetReadyToDestroy();
		return;
	}
	
	Url = InUrl;
	RequestType = InRequestType;
	Headers = InHeaders;
	RequestBody = InRequestBody;
	Timeout = InTimeout;
}

// void UStableDiffusionHTTPAsyncActionBase::OnHeaderReceivedInternal(FHttpRequestPtr Request, const FString& HeaderName,
//                                                                    const FString& NewHeaderValue)
// {
// 	if (!Request || !Request.IsValid())
// 	{
// 		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
// 		       FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__), TEXT("Http request error!"));
// 		FStableDiffusionOutputs StableDiffusionOutputs(TEXT("Http request error!"), nullptr, nullptr);
// 		ErrorDelegate.Broadcast(StableDiffusionOutputs);
// 		SetReadyToDestroy();
// 		return;
// 	}
//
// 	UE_LOG(LogSDHTTPAsyncAction, Display, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
// 	       FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
// 	       TEXT("Http request success."));
// }

void UStableDiffusionHTTPAsyncActionBase::OnRequestProgressInternal(FHttpRequestPtr Request, int32 BytesSent,
                                                                    int32 BytesReceived)
{
	if (!Request || !Request.IsValid())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
		       FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
		       TEXT("Http request error!"));
		UStableDiffusionStringOnlyOutput* ErrorOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
		ErrorOutput->Message = TEXT("Http request progress error!");
		ErrorDelegate.Broadcast(ErrorOutput);
		SetReadyToDestroy();
		return;
	}
	
	// if (BytesSent != BytesReceived)
	// {
	// 	UE_LOG(LogSDHTTPAsyncAction, Warning, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
	// 	       FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
	// 	       TEXT("Http request receive [BytesSent!=BytesReceived]!"));
	// }
}

void UStableDiffusionHTTPAsyncActionBase::OnProcessRequestCompleteInternal(FHttpRequestPtr Request,
                                                                           FHttpResponsePtr Response,
                                                                           bool bConnectedSuccessfully)
{
	if (!Request || !Request.IsValid() || !Response || !Response.IsValid() || !bConnectedSuccessfully || Response->GetResponseCode() != 200)
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
		       FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
		       TEXT("Http request error!"));
		UStableDiffusionStringOnlyOutput* ErrorOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
		ErrorOutput->Message = TEXT("Http request complete error!");
		ErrorDelegate.Broadcast(ErrorOutput);
		SetReadyToDestroy();
		return;
	}
}

/*-------------------------------------- HTTP ------------------------------------*/
