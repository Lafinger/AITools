// Fill out your copyright notice in the Description page of Project Settings.


#include "VoiceRecognitionAsyncAction.h"

DEFINE_LOG_CATEGORY(LogWebSocketRequestAsyncAction);

UVoiceRecognitionAsyncAction::UVoiceRecognitionAsyncAction(const FObjectInitializer& ObjectInitializer)
: ReceiveMessageBase(nullptr)
{
}

UVoiceRecognitionAsyncAction* UVoiceRecognitionAsyncAction::Connect(const UObject* WorldContextObject, const FString& InUrl, const TArray<FString>& InProtocols, const TMap<FString, FString>& InHeaders)
{
	if(InUrl.IsEmpty())
	{
		UE_LOG(LogWebSocketRequestAsyncAction, Error, TEXT("ThreadID:%d, %s: Voice recognition url is empty!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}
	
	ThisClass* const AsyncAction = NewObject<ThisClass>();
	AsyncAction->RegisterWithGameInstance(WorldContextObject);
	AsyncAction->InitWebSocket(InUrl, InProtocols, InHeaders);
	
	AsyncAction->ReceiveMessageBase = NewObject<UReceiveMessageBase>(AsyncAction);
	
	return AsyncAction;
}

void UVoiceRecognitionAsyncAction::Cancel()
{
	
	
	Super::Cancel(); // place final
}

bool UVoiceRecognitionAsyncAction::Send(const FString& InLanguage, const FString& InAbsoluteFilePath)
{
	if(InLanguage.IsEmpty() || InAbsoluteFilePath.IsEmpty())
	{
		UE_LOG(LogWebSocketRequestAsyncAction, Error, TEXT("ThreadID:%d, %s: Voice recognition send data invalid!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return false;
	}

	TSharedPtr<FJsonObject> ObjectJson = MakeShareable(new FJsonObject);
	ObjectJson->SetStringField(TEXT("language"),InLanguage);
	ObjectJson->SetStringField(TEXT("absolute_file_path"),InAbsoluteFilePath);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(ObjectJson.ToSharedRef(),Writer);
	
	return SendString(OutputString);
}


void UVoiceRecognitionAsyncAction::InitWebSocket(const FString& InUrl, const TArray<FString>& InProtocols, const TMap<FString, FString>& InHeaders)
{
	Super::InitWebSocket(InUrl, InProtocols, InHeaders);
}

void UVoiceRecognitionAsyncAction::OnConnectedInternal()
{
	Super::OnConnectedInternal();
}

void UVoiceRecognitionAsyncAction::OnConnectionErrorInternal(const FString& Error)
{

	
	Super::OnConnectionErrorInternal(Error); // place final
}

void UVoiceRecognitionAsyncAction::OnClosedInternal(int32 StatusCode, const FString& Reason, bool bWasClean)
{

	
	Super::OnClosedInternal(StatusCode, Reason, bWasClean); // place final
}

void UVoiceRecognitionAsyncAction::OnMessageInternal(const FString& MessageString)
{
	Super::OnMessageInternal(MessageString);

	ReceiveMessageBase->Message = MessageString;
	ProcessDelegate.Broadcast(ReceiveMessageBase);
}
