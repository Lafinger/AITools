// Fill out your copyright notice in the Description page of Project Settings.


#include "ComfyUIListenAsyncAction.h"

#include "StableDiffusionServicesSettings.h"
#include "StableDiffusionServicesSubsystem.h"
#include "Kismet/GameplayStatics.h"

UComfyUIListenAsyncAction::UComfyUIListenAsyncAction(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer), StableDiffusionListenOutput(nullptr)
{
}

UComfyUIListenAsyncAction* UComfyUIListenAsyncAction::Connect(const UObject* WorldContextObject, const TArray<FString>& InProtocols, const TMap<FString, FString>& InHeaders)
{
	const UStableDiffusionServicesSettings* Settings = GetDefault<UStableDiffusionServicesSettings>();
	if(!Settings)
	{
		UE_LOG(LogSDWebSocketAsyncAction, Error, TEXT("ThreadID:%d, %s: Get stable diffusion services settings error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));

		return nullptr;
	}

	FString ListenUrl = Settings->ComfyUISettings.GetListenUrl();
	if(ListenUrl.IsEmpty())
	{
		UE_LOG(LogSDWebSocketAsyncAction, Error, TEXT("ThreadID:%d, %s: ComfyUI listen url is empty!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	check(GameInstance && "Canont find game instance!");
	UStableDiffusionServicesSubsystem* StableDiffusionServicesSubsystem = GameInstance->GetSubsystem<UStableDiffusionServicesSubsystem>();
	
	if(!StableDiffusionServicesSubsystem)
	{
		UE_LOG(LogSDWebSocketAsyncAction, Error, TEXT("ThreadID:%d, %s: ComfyUI get StableDiffusionServicesSubsystem error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}

	FString ClientID = StableDiffusionServicesSubsystem->GetComfyUIClientID();
	FString ListenFullUrl = ListenUrl + FString("?clientId=") + ClientID;
	if(!StableDiffusionServicesSubsystem)
	{
		UE_LOG(LogSDWebSocketAsyncAction, Error, TEXT("ThreadID:%d, %s: Connect to comfy ui url is empty!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}
	
	
	ThisClass* const AsyncAction = NewObject<ThisClass>();
	AsyncAction->RegisterWithGameInstance(WorldContextObject);
	AsyncAction->InitWebSocket(ListenFullUrl, InProtocols, InHeaders);
	
	AsyncAction->StableDiffusionListenOutput = NewObject<UStableDiffusionListenOutput>(AsyncAction);
	
	return AsyncAction;
}

void UComfyUIListenAsyncAction::Cancel()
{
	Super::Cancel(); // place end
}

void UComfyUIListenAsyncAction::InitWebSocket(const FString& InUrl, const TArray<FString>& InProtocols, const TMap<FString, FString>& InHeaders)
{
	Super::InitWebSocket(InUrl, InProtocols, InHeaders);
}
void UComfyUIListenAsyncAction::OnConnectedInternal()
{
	Super::OnConnectedInternal();
}

void UComfyUIListenAsyncAction::OnConnectionErrorInternal(const FString& Error)
{

	
	Super::OnConnectionErrorInternal(Error); // place to the end!!!
}

void UComfyUIListenAsyncAction::OnClosedInternal(int32 StatusCode, const FString& Reason, bool bWasClean)
{

	
	Super::OnClosedInternal(StatusCode, Reason, bWasClean); // place to the end!!!
}

void UComfyUIListenAsyncAction::OnMessageInternal(const FString& MessageString)
{
	Super::OnMessageInternal(MessageString);

	if(MessageString.IsEmpty())
	{
		UE_LOG(LogSDWebSocketAsyncAction, Warning, TEXT("ThreadID:%d, %s: Receive comfyui message is emtpty!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return;
	}

	// parse sting to json obj
	TSharedPtr<FJsonObject> RootJsonObj;
	TSharedRef<TJsonReader<>> RootJsonObjReader = TJsonReaderFactory<>::Create(*MessageString);
	if(!FJsonSerializer::Deserialize(RootJsonObjReader, RootJsonObj) || !RootJsonObj.IsValid())
	{
		UE_LOG(LogSDWebSocketAsyncAction, Error, TEXT("ThreadID:%d, %s: Comfyui message convert to json obj error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		StableDiffusionListenOutput->Message = TEXT("Comfyui message convert to json obj error!");
		ErrorDelegate.Broadcast(StableDiffusionListenOutput);
		
		return;
	}
	FString Type = RootJsonObj->GetStringField(TEXT("type"));
	TSharedPtr<FJsonObject> DataJsonObj = RootJsonObj->GetObjectField(TEXT("data"));

	ListenType ListenType = ListenType::None;
	int32 QueueRemaining = -1;
	FString PromptID;
	FString Node;
	int32 Value = -1;
	int32 Max = -1;

	if(Type == TEXT("status"))
	{
		ListenType = ListenType::Status;
		TSharedPtr<FJsonObject> StatusJsonObj = DataJsonObj->GetObjectField(TEXT("status"));
		const TSharedPtr<FJsonObject>* ExecInfoJsonObj = nullptr;
		if(DataJsonObj->TryGetObjectField(TEXT("exec_info"), ExecInfoJsonObj))
		{
			(*ExecInfoJsonObj)->TryGetNumberField(TEXT("queue_remaining"), QueueRemaining);
		}
	}
	else if(Type == TEXT("execution_start"))
	{
		ListenType = ListenType::ExecutionStart;
		PromptID = DataJsonObj->GetStringField(TEXT("prompt_id"));
	}
	else if (Type == TEXT("executing"))
	{
		if(DataJsonObj->TryGetStringField(TEXT("node"), Node))
		{
			ListenType = ListenType::Executing;
		}
		else
		{
			ListenType = ListenType::Completed;
		}
		PromptID = DataJsonObj->GetStringField(TEXT("prompt_id"));
	}
	else if (Type == TEXT("progress"))
	{
		ListenType = ListenType::Progress;
		Value = DataJsonObj->GetIntegerField(TEXT("value"));
		Max = DataJsonObj->GetIntegerField(TEXT("max"));
	}
	
	StableDiffusionListenOutput->Type = ListenType;
	StableDiffusionListenOutput->Message = MessageString;
	StableDiffusionListenOutput->QueueRemaining = QueueRemaining;
	StableDiffusionListenOutput->PromptID = PromptID;
	StableDiffusionListenOutput->Node = Node;
	StableDiffusionListenOutput->Value = Value;
	StableDiffusionListenOutput->Max = Max;
	ProcessDelegate.Broadcast(StableDiffusionListenOutput);
}
