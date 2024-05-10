// Copyright 2023 AIRT,  All Rights Reserved.


#include "IVWebSocketWrapper.h"

#include "Json/IVJsonObject.h"
#include "Json/IVJsonValue.h"
#include "IVWebSocket.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"

UIVWebSocketWrapper* UIVWebSocketWrapper::CreateWebSocket()
{
	return NewObject<UIVWebSocketWrapper>();
}

UIVWebSocketWrapper::UIVWebSocketWrapper()
	: Super()
	, NativeSocket(nullptr)
{}

void UIVWebSocketWrapper::Connect(const FString& Url, const FString& Protocol)
{
	if (IsConnected())
	{
		UE_LOG(IVWebSocket, Warning, TEXT("Ignoring Connect(): the socket is already connected to a server."));
		return;
	}

	NativeSocket = FWebSocketsModule::Get().CreateWebSocket(Url, Protocol, Headers);

	NativeSocket->OnConnected()      .AddUObject(this, &UIVWebSocketWrapper::OnConnected);
	NativeSocket->OnConnectionError().AddUObject(this, &UIVWebSocketWrapper::OnConnectionError);
	NativeSocket->OnClosed()         .AddUObject(this, &UIVWebSocketWrapper::OnClosed);
	NativeSocket->OnMessage()        .AddUObject(this, &UIVWebSocketWrapper::OnMessage);
	NativeSocket->OnRawMessage()     .AddUObject(this, &UIVWebSocketWrapper::OnRawMessage);
	
	NativeSocket->Connect();
}

void UIVWebSocketWrapper::Close(const int32 Code, const FString& Reason)
{
	if (IsConnected())
	{
		NativeSocket->Close(Code, Reason);
	}
	else
	{
		UE_LOG(IVWebSocket, Warning, TEXT("Close() called but the WebSocket is not connected."));
	}
}

void UIVWebSocketWrapper::SetHeaders(const TMap<FString, FString>& InHeaders)
{
	SetHeaders(CopyTemp(InHeaders));
}

void UIVWebSocketWrapper::SetHeaders(TMap<FString, FString>&& InHeaders)
{
	Headers.Append(MoveTemp(InHeaders));
}

void UIVWebSocketWrapper::AddHeader(const FString& Header, const FString& Value)
{
	AddHeader(CopyTemp(Header), CopyTemp(Value));
}

void UIVWebSocketWrapper::AddHeader(FString&& Header, FString&& Value)
{
	Headers.Add(MoveTemp(Header), MoveTemp(Value));
}

void UIVWebSocketWrapper::RemoveHeader(const FString& HeaderToRemove)
{
	Headers.Remove(HeaderToRemove);
}

bool UIVWebSocketWrapper::IsConnected() const
{
	return NativeSocket && NativeSocket->IsConnected();
}

void UIVWebSocketWrapper::BranchIsConnected(EIVWebSocketIsConnectedExecution& Branches)
{
	Branches = IsConnected() ? EIVWebSocketIsConnectedExecution::Connected : EIVWebSocketIsConnectedExecution::NotConnected;
}

void UIVWebSocketWrapper::SendMessage(const FString& Message)
{
	if (IsConnected())
	{
		NativeSocket->Send(Message);
		OnMessageSent(Message);
	}
	else
	{
		UE_LOG(IVWebSocket, Warning, TEXT("SendMessage() called but the WebSocket is not connected."));
	}
}

void UIVWebSocketWrapper::SendJsonMessage(const UIVJsonObject* Json)
{
	if(IsValid(Json))
	{
		return;
	}
	const FString Message=Json->EncodeJson(true);
	
	if (IsConnected())
	{
		NativeSocket->Send(Message);
		OnMessageSent(Message);
	}
	else
	{
		UE_LOG(IVWebSocket, Warning, TEXT("SendJsonMessage() called but the WebSocket is not connected."));
	}
}

void UIVWebSocketWrapper::SendRawMessage(const TArray<uint8> & Message, const bool bIsBinary)
{
	if (IsConnected())
	{
		NativeSocket->Send(Message.GetData(), sizeof(uint8) * Message.Num(), bIsBinary);
	}
	else
	{
		UE_LOG(IVWebSocket, Warning, TEXT("SendRawMessage() called but the WebSocket is not connected."));
	}
}

void UIVWebSocketWrapper::OnConnected()
{
	OnConnectedEvent.Broadcast();
}

void UIVWebSocketWrapper::OnConnectionError(const FString& Error)
{
	OnConnectionErrorEvent.Broadcast(Error);
}

void UIVWebSocketWrapper::OnClosed(int32 Status, const FString& Reason, bool bWasClean)
{
	OnCloseEvent.Broadcast(Status, Reason, bWasClean);
}

void UIVWebSocketWrapper::OnMessage(const FString& Message)
{
	 UIVJsonObject* Json=NewObject<UIVJsonObject>();
	if(!Json->DecodeJson(Message))
	{
		UIVJsonValue* JsonValue=UIVJsonValue::ValueFromJsonString(Message);
		if(!IsValid(JsonValue))
		{
			return;
		}
		Json=JsonValue->AsObject();
	}
	
	OnMessageEvent.Broadcast(Message,Json);
}

void UIVWebSocketWrapper::OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining)
{
	const TArray<uint8> ArrayData(reinterpret_cast<const uint8*>(Data), Size / sizeof(uint8));
	OnRawMessageEvent.Broadcast(ArrayData, static_cast<int64>(BytesRemaining));
}

void UIVWebSocketWrapper::OnMessageSent(const FString& Message)
{
	OnMessageSentEvent.Broadcast(Message);
}
