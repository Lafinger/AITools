// Fill out your copyright notice in the Description page of Project Settings.


#include "ComfyUIGetTaskResultAsyncAction.h"

#include "StableDiffusioUtilities.h"
#include "StableDiffusionServicesSettings.h"
#include "Interfaces/IHttpResponse.h"

UComfyUIGetTaskResultAsyncAction::UComfyUIGetTaskResultAsyncAction(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

UComfyUIGetTaskResultAsyncAction* UComfyUIGetTaskResultAsyncAction::Connect(const UObject* WorldContextObject, const FString& FileName, const FString& Type, int InTimeout)
{
	const UStableDiffusionServicesSettings* Settings = GetDefault<UStableDiffusionServicesSettings>();
	if(!Settings)
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: Get stable diffusion services settings error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}

	FString GetTaskResultUrl = Settings->ComfyUISettings.GetTaskResult();
	if(GetTaskResultUrl.IsEmpty())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: ComfyUI get task result url is empty!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}
	
	ThisClass* const AsyncAction = NewObject<ThisClass>();
	AsyncAction->RegisterWithGameInstance(WorldContextObject);
	
	TMap<FString, FString> HeadersMap;

	GetTaskResultUrl += FString::Format(TEXT("?filename={0}&type={1}&subfolder&preview&channel"),
		TArray<FStringFormatArg>({FileName, Type}));
	
	AsyncAction->InitHTTP(GetTaskResultUrl, EHTTPRequestType::GET, HeadersMap, FString(), InTimeout);
	
	return AsyncAction;
}


void UComfyUIGetTaskResultAsyncAction::Cancel()
{

	
	Super::Cancel(); // place end
}

void UComfyUIGetTaskResultAsyncAction::InitHTTP(const FString& InUrl, EHTTPRequestType InRequestType, const TMap<FString, FString>& InHeaders, const FString& InRequestBody,
	int32 InTimeout)
{
	Super::InitHTTP(InUrl, InRequestType, InHeaders, InRequestBody, InTimeout);
}

void UComfyUIGetTaskResultAsyncAction::OnRequestProgressInternal(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
{
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

void UComfyUIGetTaskResultAsyncAction::OnProcessRequestCompleteInternal(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	Super::OnProcessRequestCompleteInternal(Request, Response, bConnectedSuccessfully);

	// Convert FString to FJsonObject
	TArray<uint8> ResultBytes = Response->GetContent();
	if(ResultBytes.IsEmpty())
	{
		UE_LOG(LogSDHTTPAsyncAction, Warning, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
			   TEXT("Task result is empty!"));

		SetReadyToDestroy();
		return;
	}

	
	bool bIsWebpImage = StableDiffusioUtilities::WebpFormatCheck(ResultBytes);
	if(!bIsWebpImage)
	{
		UE_LOG(LogSDHTTPAsyncAction, Warning, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
			   TEXT("Result is not webp image!"));

		SetReadyToDestroy();
		return;
	}
	
	UStableDiffusionGetTaskResultOutput* CompletedOutput = NewObject<UStableDiffusionGetTaskResultOutput>(this);
	CompletedOutput->Texture2DDynamic = StableDiffusioUtilities::CreateDynamicTextureFromWebpImageData(ResultBytes);
	CompletedDelegate.Broadcast(CompletedOutput);
	
	SetReadyToDestroy();
}