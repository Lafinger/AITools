// Fill out your copyright notice in the Description page of Project Settings.


#include "RequestAsyncActionBase.h"

#include "HttpModule.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"

/*-------------------------------------- AsyncActionBase ------------------------------------*/
URequestAsyncActionBase::URequestAsyncActionBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), bIsCancel(false)
{
	AddToRoot();
}

void URequestAsyncActionBase::SetReadyToDestroy()
{
	RemoveFromRoot();
	Super::SetReadyToDestroy();
}

void URequestAsyncActionBase::Cancel()
{
	bIsCancel = true;
	UReceiveMessageBase* CancelOutput = NewObject<UReceiveMessageBase>(this);
	CancelOutput->Message = TEXT("Cancel");
	CancelDelegate.Broadcast(CancelOutput);

	SetReadyToDestroy();
}

/*-------------------------------------- AsyncActionBase ------------------------------------*/


/*-------------------------------------- WebSocket ------------------------------------*/
DEFINE_LOG_CATEGORY(LogWebSocketRequestAsyncAction);

UWebSocketRequestAsyncActionBase::UWebSocketRequestAsyncActionBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UWebSocketRequestAsyncActionBase::Activate()
{
	Super::Activate();
	
	if(Url.IsEmpty())
	{
		ensureAlwaysMsgf(false, TEXT("must call InitWebSocket() before Active()"));
		SetReadyToDestroy();
		
		return;
	}
	
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url, Protocols, Headers);
	
	WebSocket->OnConnected().AddUObject(this, &UWebSocketRequestAsyncActionBase::OnConnectedInternal);
	WebSocket->OnConnectionError().AddUObject(this, &UWebSocketRequestAsyncActionBase::OnConnectionErrorInternal);
	WebSocket->OnClosed().AddUObject(this, &UWebSocketRequestAsyncActionBase::OnClosedInternal);
	WebSocket->OnMessage().AddUObject(this, &UWebSocketRequestAsyncActionBase::OnMessageInternal);
	WebSocket->Connect();

	UE_LOG(LogWebSocketRequestAsyncAction, Display, TEXT("[TID]:%d, [%s]: %s"),
		FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__),
		TEXT("web socket initialized."));
}

void UWebSocketRequestAsyncActionBase::Cancel()
{
	if(WebSocket && WebSocket.IsValid())
	{
		WebSocket->Close();
	}
	
	Super::Cancel();
	UE_LOG(LogWebSocketRequestAsyncAction, Warning, TEXT("[TID]:%d, [%s]: %s"),
		FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__),
		TEXT("Websocket cancel!"));
}

bool UWebSocketRequestAsyncActionBase::SendString(const FString& InData)
{
	if(!WebSocket || !WebSocket.IsValid() || !WebSocket->IsConnected())
	{
		UE_LOG(LogWebSocketRequestAsyncAction, Error, TEXT("[TID]:%d, [%s]: %s"),
		FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__),
		TEXT("Websocket invalid or Websocket disconnect!"));
		return false;
	}

	WebSocket->Send(InData);
	return true;
}

bool UWebSocketRequestAsyncActionBase::SendBinary(const void* InData, SIZE_T InSize, bool bIsBinary)
{
	if(!WebSocket || !WebSocket.IsValid() || !WebSocket->IsConnected())
	{
		UE_LOG(LogWebSocketRequestAsyncAction, Error, TEXT("[TID]:%d, [%s]: %s"),
		FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__),
		TEXT("Websocket invalid or Websocket disconnect!"));
		return false;
	}

	WebSocket->Send(InData, InSize, bIsBinary);
	return true;
}

void UWebSocketRequestAsyncActionBase::InitWebSocket(const FString& InUrl, const TArray<FString>& InProtocols, const TMap<FString, FString>& InHeaders)
{
	if (InUrl.IsEmpty())
    {
		UE_LOG(LogWebSocketRequestAsyncAction, Error, TEXT("[TID]:%d, [%s]: %s"),
			FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__),
			TEXT("Init web socket url is empty!"));
		UReceiveMessageBase* ErrorOutput = NewObject<UReceiveMessageBase>(this);
		ErrorOutput->Message = TEXT("Init web socket url is empty!");
    	ErrorDelegate.Broadcast(ErrorOutput);
    	SetReadyToDestroy();
    	return;
    }
	
	Url = InUrl;
	Headers = InHeaders;
	Protocols = InProtocols;
}


void UWebSocketRequestAsyncActionBase::OnConnectedInternal()
{
	UE_LOG(LogWebSocketRequestAsyncAction, Display, TEXT("[TID]:%d, [%s]: %s"),
		FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__),
		TEXT("Web socket connected."));

	UReceiveMessageBase* ConnectedOutput = NewObject<UReceiveMessageBase>(this);
	ConnectedOutput->Message = TEXT("Close Web socket.");
	ConnectedDelegate.Broadcast(ConnectedOutput);
}

void UWebSocketRequestAsyncActionBase::OnConnectionErrorInternal(const FString& Error)
{
	UE_LOG(LogWebSocketRequestAsyncAction, Error, TEXT("[TID]:%d, [%s]: %s"),
		FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__),
		TEXT("Web socket connection error!"));
	UReceiveMessageBase* ErrorOutput = NewObject<UReceiveMessageBase>(this);
	ErrorOutput->Message = TEXT("Web socket connection error!");
	ErrorDelegate.Broadcast(ErrorOutput);
	SetReadyToDestroy();
}

void UWebSocketRequestAsyncActionBase::OnClosedInternal(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	
	// UWebSocketRequestAsyncActionBase::OnClosedInternal() must be placed final
	UE_LOG(LogWebSocketRequestAsyncAction, Display, TEXT("[TID]:%d, [%s]: %s, StatusCode:%d, Reason:%s, bWasClean:%s"),
		FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__),
		TEXT("Close Web socket."), StatusCode, *Reason,  bWasClean ? TEXT("true") : TEXT("false"));
	UReceiveMessageBase* CompletedOutput = NewObject<UReceiveMessageBase>(this);
	CompletedOutput->Message = TEXT("Close Web socket.");
	CompletedDelegate.Broadcast(CompletedOutput);
	SetReadyToDestroy();
}

void UWebSocketRequestAsyncActionBase::OnMessageInternal(const FString& MessageString)
{
}

/*-------------------------------------- WebSocket ------------------------------------*/


/*-------------------------------------- HTTP ------------------------------------*/
DEFINE_LOG_CATEGORY(LogHttpRequestAsyncAction);

UHttpRequestAsyncActionBase::UHttpRequestAsyncActionBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer), Timeout(120)
{
}

void UHttpRequestAsyncActionBase::Activate()
{
	Super::Activate();

	if(Url.IsEmpty() || RequestType.IsEmpty())
	{
		ensureAlwaysMsgf(false, TEXT("must call InitWebSocket() before Active()"));
		SetReadyToDestroy();
		return;
	}
	
	FHttpModule* HttpModule = static_cast<FHttpModule*>(FModuleManager::Get().GetModule("HTTP"));
	HttpRequest = HttpModule->CreateRequest();
	
	HttpRequest->SetURL(Url);
	// const UEnum* EBeTransformEnum = StaticEnum<EHttpRequestType>();
	// const FString TypeStr = EBeTransformEnum->GetNameStringByValue((uint8)RequestType);
	HttpRequest->SetVerb(RequestType);
	for (TPair<FString, FString>& HeaderPair : Headers)
	{
		FString HeaderKey = HeaderPair.Key;
		FString HeaderValue = HeaderPair.Value;
		HttpRequest->SetHeader(HeaderKey, HeaderValue);
	}
	if(!RequestBody.IsEmpty())
	{
		HttpRequest->SetContentAsString(RequestBody);
	}
	HttpRequest->SetTimeout(Timeout);
	
	HttpRequest->OnRequestProgress().BindUObject(this, &UHttpRequestAsyncActionBase::OnRequestProgressInternal);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHttpRequestAsyncActionBase::OnProcessRequestCompletedInternal);
	HttpRequest->ProcessRequest();

	UE_LOG(LogHttpRequestAsyncAction, Display, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
		   FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__),
		   TEXT("Http request initialized!"));
}

void UHttpRequestAsyncActionBase::Cancel()
{
	if(HttpRequest && HttpRequest.IsValid())
	{
		HttpRequest->CancelRequest();
	}
	
	Super::Cancel();
	UE_LOG(LogHttpRequestAsyncAction, Warning, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
	       FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__),
	       TEXT("Cancel http request!"));
}

void UHttpRequestAsyncActionBase::InitHttp(const FString& InUrl, const FString& InRequestType, const TMap<FString, FString>& InHeaders, const FString& InRequestBody, int32 InTimeout)
{
	if (InUrl.IsEmpty() || InRequestType.IsEmpty())
	{
		UE_LOG(LogHttpRequestAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__),
			   TEXT("http url or request type is empty!"));
		UReceiveMessageBase* ErrorOutput = NewObject<UReceiveMessageBase>(this);
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

void UHttpRequestAsyncActionBase::OnRequestProgressInternal(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
{
	if (!Request || !Request.IsValid())
	{
		UE_LOG(LogHttpRequestAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
		       FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
		       TEXT("Http request error!"));
		UReceiveMessageBase* ErrorOutput = NewObject<UReceiveMessageBase>(this);
		ErrorOutput->Message = TEXT("Http request progress error!");
		ErrorDelegate.Broadcast(ErrorOutput);
		SetReadyToDestroy();
		return;
	}
}

void UHttpRequestAsyncActionBase::OnProcessRequestCompletedInternal(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	
	// Super::OnProcessRequestCompletedInternal() must be placed final
	UE_LOG(LogHttpRequestAsyncAction, Display, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
			   TEXT("Http request completed"));
	SetReadyToDestroy();
}

/*-------------------------------------- HTTP ------------------------------------*/
