// Fill out your copyright notice in the Description page of Project Settings.


#include "ComfyUISubmitTaskAsyncAction.h"

#include "StableDiffusionServicesSettings.h"
#include "StableDiffusionServicesSubsystem.h"
#include "StableDiffusionServicesUtilities.h"
#include "Interfaces/IHttpResponse.h"

UComfyUISubmitTaskAsyncAction::UComfyUISubmitTaskAsyncAction(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

UComfyUISubmitTaskAsyncAction* UComfyUISubmitTaskAsyncAction::Connect(const UObject* WorldContextObject, const FString& InPrompt, int32 InTimeout)
{
	const UStableDiffusionServicesSettings* Settings = GetDefault<UStableDiffusionServicesSettings>();
	if(!Settings)
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__),
			   TEXT("Get stable diffusion services settings error!"));
		return nullptr;
	}

	FString SubmitTaskUrl = Settings->ComfyUISettings.SubmitTask();
	if(SubmitTaskUrl.IsEmpty())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__),
			   TEXT("ComfyUI submit task url is empty!"));
		return nullptr;
	}

	UStableDiffusionServicesSubsystem* StableDiffusionServicesSubsystem = GEngine->GetEngineSubsystem<UStableDiffusionServicesSubsystem>();
	if(!StableDiffusionServicesSubsystem)
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__),
			   TEXT("ComfyUI get StableDiffusionServicesSubsystem error!"));
		return nullptr;
	}
	
	// FString AbsoluteFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir()) + "AITools/VoiceWakeUp/Source/ThirdParty/VoiceWakeUpSdk/res/wakeupresource.jet";
	// AbsoluteFilePath = AbsoluteFilePath.Replace(TEXT("/"), TEXT("\\\\"));
	
	// Load json file to FJsonObject
	FString WorkflowAPIJsonString = FComfyUIUtilities::GetWorkflowAPIStringWithPrompt(InPrompt);
	if(WorkflowAPIJsonString.IsEmpty())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__),
			   TEXT("Get workflow API string error!"));
		return nullptr;
	}
	
	TSharedPtr<FJsonObject> APIJsonObj;
	TSharedRef<TJsonReader<>> APIJsonObjReader = TJsonReaderFactory<>::Create(*WorkflowAPIJsonString);
	if(!FJsonSerializer::Deserialize(APIJsonObjReader, APIJsonObj) || !APIJsonObj.IsValid())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__),
			   TEXT("workflow_api.json parsing to obj error !"));
		return nullptr;
	}
	
	// // prompt.json
	// FString PromptFileAbsolutePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir()) + "AITools/StableDiffusionServices/Resources/ComfyUI/prompt.json";
	// PromptFileAbsolutePath = PromptFileAbsolutePath.Replace(TEXT("/"), TEXT("\\\\"));
	//
	// FString PromptFileJsonString;
	// bool bIsPromptFileLoadSuccess = FFileHelper::LoadFileToString(PromptFileJsonString, *PromptFileAbsolutePath);
	// if(!bIsPromptFileLoadSuccess || PromptFileJsonString.IsEmpty())
	// {
	// 	UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
	// 		   FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__),
	// 		   TEXT("ComfyUI load prompt.json error or prompt.json is empty!"));
	// 	return nullptr;
	// }

	TSharedPtr<FJsonObject> PromptJsonObj = MakeShared<FJsonObject>();
	// TSharedRef<TJsonReader<>> PromptJsonObjReader = TJsonReaderFactory<>::Create(*PromptFileJsonString);
	// if(!FJsonSerializer::Deserialize(PromptJsonObjReader, PromptJsonObj) || !PromptJsonObj.IsValid())
	// {
	// 	UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
	// 		   FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__),
	// 		   TEXT("prompt.json parsing to obj error!"));
	// 	return nullptr;
	// }


	// Construct request body
	FString ClientIDName = TEXT("client_id");
	FString ClientIDValue = StableDiffusionServicesSubsystem->GetClientID();
	PromptJsonObj->SetStringField(ClientIDName, ClientIDValue);

	FString PromptName = TEXT("prompt");
	PromptJsonObj->SetObjectField(PromptName, APIJsonObj);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	bool bIsConvertToStringSuccess = FJsonSerializer::Serialize(PromptJsonObj.ToSharedRef(),Writer);
	if(!bIsConvertToStringSuccess || RequestBody.IsEmpty())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), 0.0, *FString(__FUNCTION__),
			   TEXT("Prompt json obj convert to string error!"));
		return nullptr;
	}
	
	RequestBody = RequestBody.Replace(TEXT("\n"),TEXT(""));
	RequestBody = RequestBody.Replace(TEXT("\r"),TEXT(""));
	RequestBody = RequestBody.Replace(TEXT("\t"),TEXT(""));
	

	ThisClass* const AsyncAction = NewObject<ThisClass>();
	AsyncAction->RegisterWithGameInstance(WorldContextObject);
	
	TMap<FString, FString> HeadersMap{{FString(TEXT("Content-Type")), FString(TEXT("application/json"))}};
	AsyncAction->InitHTTP(SubmitTaskUrl, EHTTPRequestType::POST, HeadersMap, RequestBody, InTimeout);
	
	return AsyncAction;
}


void UComfyUISubmitTaskAsyncAction::Cancel()
{

	
	Super::Cancel(); // place end
}

void UComfyUISubmitTaskAsyncAction::InitHTTP(const FString& InUrl, EHTTPRequestType InRequestType, const TMap<FString, FString>& InHeaders, const FString& InRequestBody,
	int32 InTimeout)
{
	Super::InitHTTP(InUrl, InRequestType, InHeaders, InRequestBody, InTimeout);
}

void UComfyUISubmitTaskAsyncAction::OnRequestProgressInternal(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
{
	Super::OnRequestProgressInternal(Request, BytesSent, BytesReceived);

	// FHttpResponsePtr HttpResponsePtr = Request->GetResponse();
	// if(!HttpResponsePtr || !HttpResponsePtr.IsValid())
	// {
	// 	UE_LOG(LogSDHTTPAsyncAction, Warning, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
	// 		   FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
	// 		   TEXT("Http request invalid!"));
	// 	return;
	// }
	//
	// UStableDiffusionStringOnlyOutput* ProcessOutput = NewObject<UStableDiffusionStringOnlyOutput>(this);
	// ProcessOutput->Message = HttpResponsePtr->GetContentAsString();
	// ProcessDelegate.Broadcast(ProcessOutput);
}

void UComfyUISubmitTaskAsyncAction::OnProcessRequestCompletedInternal(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	Super::OnProcessRequestCompletedInternal(Request, Response, bConnectedSuccessfully);

	if(bIsCancelAsyncAction)
	{
		SetReadyToDestroy();
		return;
	}

	if (!Request || !Request.IsValid() || !Response || !Response.IsValid() || !bConnectedSuccessfully || Response->GetResponseCode() != 200)
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
			   TEXT("Http request error!"));
		UStableDiffusionOutputsBase* ErrorOutput = NewObject<UStableDiffusionOutputsBase>(this);
		ErrorOutput->Message = TEXT("Http request complete error!");
		ErrorDelegate.Broadcast(ErrorOutput);
		SetReadyToDestroy();
		return;
	}

	// Convert FString to FJsonObject
	FString ResultString = Response->GetContentAsString();
	if(ResultString.IsEmpty())
	{
		UE_LOG(LogSDHTTPAsyncAction, Warning, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
			   TEXT("Submit task result is empty!"));

		SetReadyToDestroy();
		return;
	}

	TSharedPtr<FJsonObject> ResultJsonObj;
	TSharedRef<TJsonReader<>> ResultJsonObjReader = TJsonReaderFactory<>::Create(*ResultString);
	if(!FJsonSerializer::Deserialize(ResultJsonObjReader, ResultJsonObj) || !ResultJsonObj.IsValid())
	{
		UE_LOG(LogSDHTTPAsyncAction, Error, TEXT("[TID]:%d, [ET]:%f seconds, [%s]:%s"),
			   FPlatformTLS::GetCurrentThreadId(), Request->GetElapsedTime(), *FString(__FUNCTION__),
			   TEXT("Convert submit task result to FJsonObject error!"));
		UStableDiffusionOutputsBase* ErrorOutput = NewObject<UStableDiffusionOutputsBase>(this);
		ErrorOutput->Message = TEXT("Convert submit task result to FJsonObject error!");
		ErrorDelegate.Broadcast(ErrorOutput);
		
		SetReadyToDestroy();
		return;
	}

	// Get result
	FString ResultPromptID = ResultJsonObj->GetStringField(TEXT("prompt_id"));
	int32 ResultNumber = ResultJsonObj->GetIntegerField(TEXT("number"));

	
	UStableDiffusionSubmitTaskOutput* CompletedOutput = NewObject<UStableDiffusionSubmitTaskOutput>(this);
	CompletedOutput->PromptID = ResultPromptID;
	CompletedOutput->Number = ResultNumber;
	CompletedDelegate.Broadcast(CompletedOutput);
	
	SetReadyToDestroy();
}
