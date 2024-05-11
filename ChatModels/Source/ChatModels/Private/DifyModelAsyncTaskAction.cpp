
#include "DifyModelAsyncTaskAction.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json/IVJsonLibrary.h"

DEFINE_LOG_CATEGORY(LogDifyModelAsyncAction);

UDifyModelAsyncTaskAction* UDifyModelAsyncTaskAction::AsyncHttpRequest(
	const UObject* WorldContextObject,
	const FString& ConversationID,
	const FString& Question,
	ERequestType RequestType,
	EDifyModelType ChatModelType,
	bool bIsBlocking)
{
	//目前不支持阻塞式回答
	check(!bIsBlocking && "Blocking answers are not currently supported");
	
	if(Question.IsEmpty())
	{
		UE_LOG(LogDifyModelAsyncAction, Error, TEXT("ThreadID:%d, %s: Question is empty."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}
	
	const UChatModelsSettings* Settings = GetDefault<UChatModelsSettings>();
	if(!Settings)
	{
		UE_LOG(LogDifyModelAsyncAction, Error, TEXT("ThreadID:%d, %s: UChatModelsSettings::Get() Error."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}
	
	FString APIKey = *Settings->DifySetting.APIKeys.Find(ChatModelType);
	UE_LOG(LogDifyModelAsyncAction, Warning, TEXT("ThreadID:%d, %s: APIKey : %s."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *APIKey);
	if(APIKey.IsEmpty())
	{
		UE_LOG(LogDifyModelAsyncAction, Error, TEXT("ThreadID:%d, %s: APIKey is empty."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return nullptr;
	}

	UE_LOG(LogDifyModelAsyncAction, Display, TEXT("ThreadID:%d, %s: Dify model Connect start."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));

	UDifyModelAsyncTaskAction* NewAsync = NewObject<UDifyModelAsyncTaskAction>();
	NewAsync->RegisterWithGameInstance(WorldContextObject);
	
	// 创建一个请求对象
	FHttpModule* HttpModule = static_cast<FHttpModule*>(FModuleManager::Get().GetModule("HTTP"));
	NewAsync->HttpRequest = HttpModule->CreateRequest();
	
	// Http请求的默认超时为300秒, 目前150秒超时
	NewAsync->HttpRequest->SetTimeout(150);
 
	// 绑定流式委托
	NewAsync->HttpRequest->OnRequestProgress().BindUObject(NewAsync, &UDifyModelAsyncTaskAction::OnRequestProgress);
	NewAsync->HttpRequest->OnProcessRequestComplete().BindUObject(NewAsync, &UDifyModelAsyncTaskAction::OnProcessRequestComplete);
	NewAsync->HttpRequest->OnRequestWillRetry().BindUObject(NewAsync, &UDifyModelAsyncTaskAction::OnRequestWillRetry);
	
	// 设置请求URL
	NewAsync->HttpRequest->SetURL(Settings->DifySetting.URL);

	// 枚举转换成FString
	const UEnum* EBeTransformEnum = StaticEnum<ERequestType>();
	const FString TypeStr = EBeTransformEnum->GetNameStringByValue((uint8)RequestType);
	
	// 设置请求方法
	NewAsync->HttpRequest->SetVerb(TypeStr);

	// 设置请求头
	NewAsync->CurrentAPIKey = APIKey;
	FString AuthorizationValue = "Bearer " + APIKey;
	NewAsync->HttpRequest->SetHeader("Authorization", AuthorizationValue);
	FString ContentTypeValue = "application/json";
	NewAsync->HttpRequest->SetHeader("Content-Type", ContentTypeValue);
	
	// 设置请求内容
	// FString ResponseMode = bIsBlocking ? "blocking" : "streaming";
	FString ResponseMode = "streaming";
	FString RequestBody = FString::Format(TEXT("{ \"inputs\":\"{0}\", \"query\":\"{1}\", \"response_mode\":\"{2}\", \"conversation_id\":\"{3}\", \"user\":\"{4}\" }"), TArray<FStringFormatArg>({ FString(), Question, ResponseMode,  ConversationID, Settings->DifySetting.User}));
	NewAsync->HttpRequest->SetContentAsString(RequestBody);
	UE_LOG(LogDifyModelAsyncAction, Display, TEXT("ThreadID:%d, %s: request body : %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *RequestBody);
	
	//发送请求
	NewAsync->HttpRequest->ProcessRequest();
	return NewAsync;
}

void UDifyModelAsyncTaskAction::OnRequestProgress(FHttpRequestPtr RequestPtr, int32 ServeHadSendBytes, int32 ClientReceiveBytes)
{
	++ResponseInfo.ResponseCount;
	ResponseInfo.TotalBytes = ServeHadSendBytes;
	ResponseInfo.ReceiveBytes = ClientReceiveBytes;
	ResponseInfo.ElapsedTime = RequestPtr->GetElapsedTime();
	ResponseInfo.Message.Empty();

	UE_LOG(LogDifyModelAsyncAction, Display, TEXT("ThreadID:%d, %s: Response elapsed time : %f seconds"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), ResponseInfo.ElapsedTime);
	
	if (RequestPtr.IsValid() && RequestPtr)
	{
		if (RequestPtr->GetResponse().IsValid() && RequestPtr->GetResponse())
		{
			ResponseInfo.Message = RequestPtr->GetResponse().Get()->GetContentAsString();
		}
	}
	
	if(ResponseInfo.Message.IsEmpty())
	{
		UE_LOG(LogDifyModelAsyncAction, Warning, TEXT("ThreadID:%d, %s: Message ie empty!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	}
	FString Message = ResponseInfo.Message.Replace(*FString("data: ping"),*FString());
	TArray<FString> OutArray;
	Message.ParseIntoArray(OutArray, *FString("data:"));
	for (FString String : OutArray)
	{
		if(String.Contains(TEXT(" ping")))
		{
			continue;
		}
		UIVJsonObject* JsonObject = UIVJsonLibrary::ConvString_ToJsonObject(String);
		CurrentTaskID = JsonObject->GetStringField(FString("task_id"));
	}
	ResponseDelegate.Broadcast(ResponseInfo.Message);
}

void UDifyModelAsyncTaskAction::OnProcessRequestComplete(FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSuccess)
{
	if(RequestPtr && RequestPtr.IsValid() && ResponsePtr && ResponsePtr.IsValid())
	{
		ResponseInfo.ResponseCode = ResponsePtr.Get()->GetResponseCode();
		ResponseInfo.TotalBytes = ResponsePtr->GetContentLength();
		ResponseInfo.ReceiveBytes = ResponsePtr->GetContentLength();
		ResponseInfo.ElapsedTime = RequestPtr->GetElapsedTime();
		ResponseInfo.Message.Empty();
		ResponseInfo.Message = RequestPtr->GetResponse()->GetContentAsString();
		
		UE_LOG(LogDifyModelAsyncAction, Display, TEXT("ThreadID:%d, %s: Response elapsed time : %f seconds"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), ResponseInfo.ElapsedTime);

		if(!bIsCancel)
		{
			if(bSuccess)
			{
				CompletedDelegate.Broadcast(ResponseInfo.Message);
			}
			else
			{
				UE_LOG(LogDifyModelAsyncAction, Error, TEXT("ThreadID:%d, %s: Process http request error, error code : %d."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), ResponseInfo.ResponseCode);
				ErrorDelegate.Broadcast(ResponseInfo.Message);
			}
		}
	}
	else if(!bIsCancel)
	{
		UE_LOG(LogDifyModelAsyncAction, Error, TEXT("ThreadID:%d, %s: No network!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		ErrorDelegate.Broadcast(TEXT("No network!"));
	}
	else
	{
		UE_LOG(LogDifyModelAsyncAction, Error, TEXT("ThreadID:%d, %s: Unknown error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		ErrorDelegate.Broadcast(TEXT("Unknown error!"));
	}
	
	this->SetReadyToDestroy();
}

void UDifyModelAsyncTaskAction::OnRequestWillRetry(FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, float SecondsToRetry)
{
	UE_LOG(LogDifyModelAsyncAction, Display, TEXT("ThreadID:%d, %s: To retry time : %f seconds"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), SecondsToRetry);
}

void UDifyModelAsyncTaskAction::CancelDifyModelAsyncTaskAction()
{
	UE_LOG(LogDifyModelAsyncAction, Warning, TEXT("ThreadID:%d, %s: Cancel dify task action"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	bIsCancel = true;
	ResponseInfo.Message.Empty();
	HttpRequest->CancelRequest();
	
	// // 向服务端发送取消请求
	// const UChatModelsSettings* Settings = GetDefault<UChatModelsSettings>();
	// if(!Settings)
	// {
	// 	UE_LOG(LogDifyModelAsyncAction, Error, TEXT("ThreadID:%d, %s: UChatModelsSettings::Get() Error."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	// 	return;
	// }
	//
	// if(CurrentTaskID.IsEmpty())
	// {
	// 	UE_LOG(LogDifyModelAsyncAction, Error, TEXT("ThreadID:%d, %s: Current task id is empty!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	// 	this->CancelDelegate.Broadcast(FString());
	// 	this->SetReadyToDestroy();
	// 	return;
	// }
	//
	// // 先取消上一次
	// FHttpModule* HttpModule = static_cast<FHttpModule*>(FModuleManager::Get().GetModule("HTTP"));
	// TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpStopRequest = HttpModule->CreateRequest();
	//
	//
	// // 设置请求方法
	// const UEnum* EBeTransformEnum = StaticEnum<ERequestType>();
	// // const uint8 ConstraintType = static_cast<uint8>(InItem->Type);
	// const FString TypeStr = EBeTransformEnum->GetNameStringByValue((uint8)ERequestType::POST);
	// HttpStopRequest->SetVerb(TypeStr);
	//
	// // 设置请求头
	// FString AuthorizationValue = "Bearer " + CurrentAPIKey;
	// HttpStopRequest->SetHeader("Authorization", AuthorizationValue);
	// FString ContentTypeValue = "application/json";
	// HttpStopRequest->SetHeader("Content-Type", ContentTypeValue);
	//
	// FString StopURL = FString::Format(TEXT("https://api.dify.ai/v1/chat-messages/:{0}}/stop"), TArray<FStringFormatArg>({ CurrentTaskID}));
	// HttpStopRequest->SetURL(StopURL);
	//
	// FString RequestBody = FString::Format(TEXT("{ \"user\":\"{0}\" }"), TArray<FStringFormatArg>({ Settings->DifySetting.User}));
	// HttpStopRequest->SetContentAsString(RequestBody);
	// UE_LOG(LogDifyModelAsyncAction, Display, TEXT("ThreadID:%d, %s: Stop request body : %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *RequestBody);
	// 		
	// // 重绑回调
	// HttpStopRequest->OnProcessRequestComplete().BindLambda([this, HttpStopRequest](FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bConnectedSuccessfully)
	// {
	// 	CurrentTaskID.Empty();
	// 	if(!bConnectedSuccessfully)
	// 	{
	// 		UE_LOG(LogDifyModelAsyncAction, Warning, TEXT("ThreadID:%d, %s: Dify model stop error"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	// 		return;
	// 	}
	// 	if(RequestPtr && RequestPtr.IsValid() && ResponsePtr && ResponsePtr.IsValid())
	// 	{
	// 		FString StopMessage = RequestPtr->GetResponse()->GetContentAsString();
	// 		UE_LOG(LogDifyModelAsyncAction, Warning, TEXT("ThreadID:%d, %s: Dify model stop message : %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *StopMessage);
	// 	}
	// 	AsyncTask(ENamedThreads::GameThread, [this, HttpStopRequest]()
	// 	{
	// 		this->CancelDelegate.Broadcast(FString());
	// 		HttpStopRequest->CancelRequest();
	// 		this->SetReadyToDestroy();
	// 	});
	// 	
	// });
	// //发送停止请求
	// HttpStopRequest->ProcessRequest();
	// CurrentTaskID.Empty();
	
	this->SetReadyToDestroy();
}
