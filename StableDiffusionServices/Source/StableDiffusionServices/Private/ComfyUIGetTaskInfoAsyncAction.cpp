// Fill out your copyright notice in the Description page of Project Settings.


#include "ComfyUIGetTaskInfoAsyncAction.h"

#include "StableDiffusionServicesSettings.h"
#include "Interfaces/IHttpResponse.h"

UComfyUIGetTaskInfoAsyncAction::UComfyUIGetTaskInfoAsyncAction(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

UComfyUIGetTaskInfoAsyncAction* UComfyUIGetTaskInfoAsyncAction::Connect(const UObject* WorldContextObject, const FString& PromptID, int32 InTimeout)
{
	if(PromptID.IsEmpty())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: Input invalid!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}
	
	const UStableDiffusionServicesSettings* Settings = GetDefault<UStableDiffusionServicesSettings>();
	if(!Settings)
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: Get stable diffusion services settings error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}

	FString GetTaskInfoUrl = Settings->ComfyUISettings.GetTaskInfo();
	if(GetTaskInfoUrl.IsEmpty())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: ComfyUI get task info url is empty!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}
	
	ThisClass* const AsyncAction = NewObject<ThisClass>();
	AsyncAction->RegisterWithGameInstance(WorldContextObject);
	AsyncAction->PromptID = PromptID;
	
	GetTaskInfoUrl += "/" + PromptID; // Example : http://127.0.0.1:8188/history/4cf4d08f-8d0a-4e16-a3c1-c30391a8e7b9
	TMap<FString, FString> HeadersMap;
	AsyncAction->InitHTTP(GetTaskInfoUrl, EHTTPRequestType::GET, HeadersMap, FString(), InTimeout);
	
	return AsyncAction;
}


void UComfyUIGetTaskInfoAsyncAction::Cancel()
{

	
	Super::Cancel(); // place end
}

void UComfyUIGetTaskInfoAsyncAction::InitHTTP(const FString& InUrl, EHTTPRequestType InRequestType, const TMap<FString, FString>& InHeaders, const FString& InRequestBody,
	int32 InTimeout)
{
	Super::InitHTTP(InUrl, InRequestType, InHeaders, InRequestBody, InTimeout);
}

void UComfyUIGetTaskInfoAsyncAction::OnRequestProgressInternal(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
{
	Super::OnRequestProgressInternal(Request, BytesSent, BytesReceived);

	FHttpResponsePtr HttpResponsePtr = Request->GetResponse();
	if(!HttpResponsePtr || !HttpResponsePtr.IsValid())
	{
		UE_LOG(LogSDHTTPAsyncAction, Warning, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
			   TEXT("Http request invalid!"));
		return;
	}

	UStableDiffusionStringOnlyOutput* ProcessOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
	ProcessOutput->Message = HttpResponsePtr->GetContentAsString();
	ProcessDelegate.Broadcast(ProcessOutput);
}

void UComfyUIGetTaskInfoAsyncAction::OnProcessRequestCompleteInternal(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	Super::OnProcessRequestCompleteInternal(Request, Response, bConnectedSuccessfully);
	
	// Convert FString to FJsonObject
	FString ResultString = Response->GetContentAsString();
	if(ResultString.IsEmpty())
	{
		UE_LOG(LogSDHTTPAsyncAction, Warning, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
			   TEXT("Task result is empty!"));

		SetReadyToDestroy();
		return;
	}
	
	TSharedPtr<FJsonObject> ResultJsonObj;
	TSharedRef<TJsonReader<>> ResultJsonObjReader = TJsonReaderFactory<>::Create(*ResultString);
	if(!FJsonSerializer::Deserialize(ResultJsonObjReader, ResultJsonObj) || !ResultJsonObj.IsValid())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
			   TEXT("Convert task info to FJsonObject error!"));
		
		UStableDiffusionStringOnlyOutput* ErrorOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
		ErrorOutput->Message = TEXT("Convert task info to FJsonObject error!");
		ErrorDelegate.Broadcast(ErrorOutput);
		
		SetReadyToDestroy();
		return;
	}
	
	TSharedPtr<FJsonObject> ResultTaskObj = ResultJsonObj->GetObjectField(PromptID);
	if(!ResultTaskObj || !ResultTaskObj.IsValid())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s %s"),
			   FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
			   TEXT("Cannot find task with"), *PromptID);
		UStableDiffusionStringOnlyOutput* ErrorOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
		ErrorOutput->Message = FString(TEXT("Cannot find task with ")) + PromptID;
		ErrorDelegate.Broadcast(ErrorOutput);
	}

	// Get result and parsing
	TArray<TSharedPtr<FJsonValue>> ResultPrompts = ResultTaskObj->GetArrayField(TEXT("prompt")); // todo
	
	TSharedPtr<FJsonObject> ResultOutput = ResultTaskObj->GetObjectField(TEXT("outputs"));
	TSharedPtr<FJsonObject> ResultStatus = ResultTaskObj->GetObjectField(TEXT("status")); // todo

	TSharedPtr<FJsonObject> Result9 = ResultOutput->GetObjectField(TEXT("9"));
	TArray<TSharedPtr<FJsonValue>> ResultImagesInfos = Result9->GetArrayField(TEXT("images"));

	// 目前只支持一张图片， todo：支持多张图片
	TSharedPtr<FJsonObject> ResultImageInfoObj = ResultImagesInfos[0]->AsObject();
	FString ResultImageFileName = ResultImageInfoObj->GetStringField(TEXT("filename"));
	FString ResultSubfolder = ResultImageInfoObj->GetStringField(TEXT("subfolder"));
	FString ResultType = ResultImageInfoObj->GetStringField(TEXT("type"));

	
	UStableDiffusionGetTaskInfoOutput* CompletedOutput = NewObject<UStableDiffusionGetTaskInfoOutput>(this);
	CompletedOutput->PromptID = PromptID;
	CompletedOutput->ImageFileName = ResultImageFileName;
	CompletedOutput->Type = ResultType;
	CompletedDelegate.Broadcast(CompletedOutput);
	
	SetReadyToDestroy();
}