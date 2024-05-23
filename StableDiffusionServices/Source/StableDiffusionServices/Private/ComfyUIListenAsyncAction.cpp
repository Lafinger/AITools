// Fill out your copyright notice in the Description page of Project Settings.


#include "ComfyUIListenAsyncAction.h"

#include "StableDiffusionServicesSettings.h"
#include "StableDiffusionServicesSubsystem.h"

UComfyUIListenAsyncAction::UComfyUIListenAsyncAction(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

UComfyUIListenAsyncAction* UComfyUIListenAsyncAction::Connect(const UObject* WorldContextObject)
{
	const UStableDiffusionServicesSettings* Settings = GetDefault<UStableDiffusionServicesSettings>();
	if(!Settings)
	{
		UE_LOG(LogSDWebSocketAsyncAction, Error, TEXT("ThreadID:%d, %s: Get stable diffusion services settings error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));

		return nullptr;
	}

	FString ListenUrl = Settings->ComfyUISettings.Listen();
	if(ListenUrl.IsEmpty())
	{
		UE_LOG(LogSDWebSocketAsyncAction, Error, TEXT("ThreadID:%d, %s: ComfyUI listen url is empty!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}

	UStableDiffusionServicesSubsystem* StableDiffusionServicesSubsystem = GEngine->GetEngineSubsystem<UStableDiffusionServicesSubsystem>();
	if(!StableDiffusionServicesSubsystem)
	{
		UE_LOG(LogSDWebSocketAsyncAction, Error, TEXT("ThreadID:%d, %s: ComfyUI get StableDiffusionServicesSubsystem error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}
	
	ThisClass* const AsyncAction = NewObject<ThisClass>();
	AsyncAction->RegisterWithGameInstance(WorldContextObject);
	
	FString ClientID = StableDiffusionServicesSubsystem->GetClientID();
	TMap<FString, FString> HeadersMap{{FString(TEXT("clientId")), ClientID}};
	AsyncAction->InitWebSocket(ListenUrl, HeadersMap);
	
	return AsyncAction;
}

void UComfyUIListenAsyncAction::Cancel()
{

	
	Super::Cancel(); // place end
}

void UComfyUIListenAsyncAction::InitWebSocket(const FString& InUrl, const TMap<FString, FString>& InHeaders)
{
	Super::InitWebSocket(InUrl, InHeaders);
}

void UComfyUIListenAsyncAction::OnConnectedInternal()
{
	Super::OnConnectedInternal();

	
}

void UComfyUIListenAsyncAction::OnConnectionErrorInternal(const FString& Error)
{

	
	Super::OnConnectionErrorInternal(Error); // place end!!!
}

void UComfyUIListenAsyncAction::OnClosedInternal(int32 StatusCode, const FString& Reason, bool bWasClean)
{

	
	Super::OnClosedInternal(StatusCode, Reason, bWasClean); // place end!!!
}

void UComfyUIListenAsyncAction::OnMessageInternal(const FString& MessageString)
{
	Super::OnMessageInternal(MessageString);
	
	UStableDiffusionStringOnlyOutput* ProcessOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
	ProcessOutput->Message = MessageString;
	ProcessDelegate.Broadcast(ProcessOutput);
}
