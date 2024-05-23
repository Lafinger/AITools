// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StableDiffusionAsyncActionBase.h"
#include "ComfyUIGetTaskInfoAsyncAction.generated.h"

UCLASS(BlueprintType)
class UStableDiffusionGetTaskInfoOutput final : public UStableDiffusionOutputsBase
{
	GENERATED_BODY()
	
public:
	UStableDiffusionGetTaskInfoOutput(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
	: Super(ObjectInitializer)
	{
		
	};
	
	UPROPERTY(BlueprintReadOnly)
	FString PromptID;

	UPROPERTY(BlueprintReadOnly)
	FString ImageFileName;
	
	UPROPERTY(BlueprintReadOnly)
	FString Type;
};

/**
 * UComfyUIGetTaskInfoAsyncAction
 */
UCLASS(BlueprintType, meta = (NotPlaceable, ExposedAsyncProxy = RequestTask))
class STABLEDIFFUSIONSERVICES_API UComfyUIGetTaskInfoAsyncAction final : public UStableDiffusionHTTPAsyncActionBase
{
	GENERATED_BODY()

public:
	UComfyUIGetTaskInfoAsyncAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
		* 帮助节点轻松连接到服务器并处理连接事件。
		* @param WorldContextObject 代表了函数调用所处的世界。
		* @param PromptID 代表一次Prompt任务的Id
		* @param InTimeout 连接的时长。
	*/
	UFUNCTION(BlueprintCallable, Category = "ComfyUI", meta = (DisplayName = "Get task info from server ComfyUI", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UComfyUIGetTaskInfoAsyncAction* Connect(const UObject* WorldContextObject, const FString& PromptID, int32 InTimeout = 120);

	/** UStableDiffusionAsyncActionBase interface */
	UFUNCTION(BlueprintCallable, Category = ComfyUIListenAsyncAction)
	virtual void Cancel() override;

private:
	/** UStableDiffusionHTTPAsyncActionBase interface */
	virtual void InitHTTP(const FString& InUrl, EHTTPRequestType InRequestType, const TMap<FString, FString>& InHeaders, const FString& InRequestBody, int32 InTimeout = 120) override;
	virtual void OnRequestProgressInternal(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived) override;
	virtual void OnProcessRequestCompleteInternal(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully) override;

	FString PromptID;
};
