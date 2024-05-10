// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChatModelsAsyncActionBase.h"
#include "Interfaces/IHttpRequest.h"
#include "ChatModelsSettings.h"
#include "Json/IVJsonObject.h"
#include "DifyModelAsyncTaskAction.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDifyModelAsyncAction, Log, All);

UENUM(BlueprintType)
enum ERequestType : uint8
{
	None,
	HEAD,
	GET,
	POST,
	PUT
};

struct FResponseInfo
{
	int32 ResponseCode;
	int32 ResponseCount;
	int32 TotalBytes;
	int32 ReceiveBytes;
	float ElapsedTime;
	FString ResponseHeader;
	FString ResponseHeaderValue;
	
	FString Message;
	
	FResponseInfo()
	:ResponseCode(0),
	ResponseCount(0),
	TotalBytes(0),
	ReceiveBytes(0),
	ElapsedTime(0),
	ResponseHeader("None"),
	ResponseHeaderValue("None"),
	Message("None")
	{}
};

/**
 * 连接Dify的异步蓝图节点
 */
UCLASS(BlueprintType, meta = (NotPlaceable, ExposedAsyncProxy = RequestTask))
class UDifyModelAsyncTaskAction : public UChatModelsAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	* 与Dify建立链接，目前不支持阻塞式回答。
	* @param ConversationID 对话，用于保存上下文历史。
	* @param Question 向大模型问的问题。
	* @param RequestType Http请求的类型。
	* @param ChatModelType 请求大模型的种类。
	* @param bIsBlocking 大模型的输出是流式的还是阻塞式的。
	*/
	UFUNCTION(BlueprintCallable, Category = "Dify Model", meta=(DisplayName = "Dify Model Connect", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UDifyModelAsyncTaskAction* AsyncHttpRequest(
		const UObject* WorldContextObject,
		const FString& ConversationID,
		const FString& Question,
		ERequestType RequestType = ERequestType::POST,
		EDifyModelType ChatModelType = EDifyModelType::ChatGPT,
		bool bIsBlocking = false);

	/**
	* 每次请求的反馈
	* Delegate called per tick to update an Http request upload or download size progress
	*
	* @param RequestPtr original Http request that started things
	* @param ServeHadSendBytes the number of bytes sent / uploaded in the request so far.
	* @param ClientReceiveBytes the number of bytes received / downloaded in the response so far.
	*/
	void OnRequestProgress(FHttpRequestPtr RequestPtr,int32 ServeHadSendBytes,int32 ClientReceiveBytes);

	/**
	* 请求完成的反馈
	* Delegate called when an Http request completes
	*
	* @param RequestPtr original Http request that started things
	* @param ResponsePtr response received from the server if a successful connection was established
	* @param bSuccess - indicates whether or not the request was able to connect successfully
	*/
	void OnProcessRequestComplete(FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSuccess);

	/**
	* Delegate called when an Http request will be retried in the future
	*
	* @param RequestPtr - original Http request that started things
	* @param ResponsePtr - response received from the server if a successful connection was established
	* @param SecondsToRetry - seconds in the future when the response will be retried
	*/
	void OnRequestWillRetry(FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, float SecondsToRetry);

	/**
	* 用于取消异步Http请求任务。注意:Http请求可能已经执行完了。
	*/
	UFUNCTION(BlueprintCallable, Category = "Dify Model")
	void CancelDifyModelAsyncTaskAction();
	
	// 用于执行具体HTTP请求的对象.
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest;
	// HTTP响应信息
	FResponseInfo ResponseInfo;
	FString CurrentAPIKey;
	FString CurrentTaskID;
};