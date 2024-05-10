// Copyright 2023 AIRT,  All Rights Reserved.


#include "IVWebSocketNodes.h"

#include "Json/IVJsonObject.h"
#include "IVWebSocket.h"
#include "IVWebSocketWrapper.h"


void UIVWebSocketConnectAsyncProxyBase::Activate()
{
	const UIVJsonObject* Json=NewObject<UIVJsonObject>();
	
	if (!Socket)
	{
		UE_LOG(IVWebSocket, Warning, TEXT("Passed an invalid socket to \"Connect WebSocket\"."));
		OnConnectionError.Broadcast(nullptr, -1, TEXT("Invalid socket"),Json);
		SetReadyToDestroy();
		return;
	}

	if (Socket->IsConnected())
	{
		UE_LOG(IVWebSocket, Warning, TEXT("Connect() failed: The socket is already connected."));
		OnConnectionError.Broadcast(nullptr, -1, TEXT("Socket already connected"),Json);
		SetReadyToDestroy();
		return;
	}

	Socket->OnConnectedEvent		.AddDynamic(this, &UIVWebSocketConnectAsyncProxyBase::OnConnectedInternal);
	Socket->OnConnectionErrorEvent	.AddDynamic(this, &UIVWebSocketConnectAsyncProxyBase::OnConnectionErrorInternal);
	Socket->OnCloseEvent			.AddDynamic(this, &UIVWebSocketConnectAsyncProxyBase::OnCloseInternal);
	Socket->OnMessageEvent			.AddDynamic(this, &UIVWebSocketConnectAsyncProxyBase::OnMessageInternal);

	Socket->Connect(Url, Protocol);
}

void UIVWebSocketConnectAsyncProxyBase::InitSocket(UIVWebSocketWrapper* const InSocket, const FString& InUrl,
	const FString InProtocol)
{
	Socket   = InSocket;
	Url		 = InUrl;
	Protocol = InProtocol;
}

void UIVWebSocketConnectAsyncProxyBase::OnConnectedInternal()
{
	const UIVJsonObject* Json=NewObject<UIVJsonObject>();
	OnConnected.Broadcast(Socket, -1, TEXT("Connected"),Json);
}

void UIVWebSocketConnectAsyncProxyBase::OnConnectionErrorInternal(const FString& Error)
{
	const UIVJsonObject* Json=NewObject<UIVJsonObject>();
	OnConnectionError.Broadcast(Socket, -1, Error,Json);

	SetReadyToDestroy();
}

void UIVWebSocketConnectAsyncProxyBase::OnCloseInternal(const int64 Status, const FString& Reason, const bool bWasClean)
{
	const UIVJsonObject* Json=NewObject<UIVJsonObject>();
	OnClose.Broadcast(Socket, Status, Reason,Json);
	SetReadyToDestroy();
}

void UIVWebSocketConnectAsyncProxyBase::OnMessageInternal(const FString& Message,const UIVJsonObject* Json)
{
	
	OnMessage.Broadcast(Socket, -1, Message,Json);
}

UIVWebSocketConnectAsyncProxy* UIVWebSocketConnectAsyncProxy::Connect(UIVWebSocketWrapper* const WebSocket,
	const FString& Url, const FString& Protocol)
{
	UIVWebSocketConnectAsyncProxy* const Proxy = NewObject<UIVWebSocketConnectAsyncProxy>();

	Proxy->InitSocket(WebSocket, Url, Protocol);

	return Proxy;
}

UWebSocketCreateConnectAsyncProxy* UWebSocketCreateConnectAsyncProxy::Connect(const FString& Url,
	const FString& Protocol, const TMap<FString, FString>& Headers)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	UIVWebSocketWrapper* const Socket = UIVWebSocketWrapper::CreateWebSocket();

	Proxy->InitSocket(Socket, Url, Protocol);

	Socket->SetHeaders(Headers);

	return Proxy;
}

UWebSocketReconnectProxy* UWebSocketReconnectProxy::Reconnect(UIVWebSocketWrapper* const WebSocket, FString Protocol,
                                                              FString URL, int32 MaxAttemptCount)
{
	UWebSocketReconnectProxy* const Proxy = NewObject<UWebSocketReconnectProxy>();

	Proxy->WebSocket = WebSocket;
	Proxy->URL = MoveTemp(URL);
	Proxy->RemainingAttempts = MaxAttemptCount;
	Proxy->Protocol = MoveTemp(Protocol);

	return Proxy;
}

void UWebSocketReconnectProxy::Activate()
{
	if (!WebSocket)
	{
		FFrame::KismetExecutionMessage(TEXT("Passed nullptr to Reconnect()."), ELogVerbosity::Error);
		OnFailed();
		return;
	}

	WebSocket->OnConnectionErrorEvent.AddDynamic(this, &ThisClass::OnError);
	WebSocket->OnConnectedEvent      .AddDynamic(this, &ThisClass::OnConnected);

	AttemptConnection();
}

void UWebSocketReconnectProxy::OnError(const FString& ConnectionError)
{
	// We never fail if attempts < 0 (wait until success).
	if (--RemainingAttempts == 0)
	{
		OnFailed();
		return;
	}

	UE_LOG(IVWebSocket, Warning, TEXT("Reconnection attempt failed."));

	AttemptFailed.Broadcast(WebSocket, RemainingAttempts);

	AttemptConnection();
}

void UWebSocketReconnectProxy::OnConnected()
{
	UE_LOG(IVWebSocket, Log, TEXT("Reconnected to WebSocket server."));

	Connected.Broadcast(WebSocket, FMath::Max(RemainingAttempts, 0));
	SetReadyToDestroy();
}

void UWebSocketReconnectProxy::OnFailed()
{
	UE_LOG(IVWebSocket, Error, TEXT("Failed to reconnect to WebSocket server. All attempts exhausted."));

	Failed.Broadcast(WebSocket, 0);
	SetReadyToDestroy();
}

void UWebSocketReconnectProxy::AttemptConnection()
{
	if (WebSocket->IsConnected())
	{
		OnConnected();
		return;
	}

	WebSocket->Connect(URL, Protocol);
}
