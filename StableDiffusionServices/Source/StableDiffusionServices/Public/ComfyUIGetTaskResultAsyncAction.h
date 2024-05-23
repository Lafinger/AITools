// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StableDiffusionAsyncActionBase.h"
#include "ComfyUIGetTaskResultAsyncAction.generated.h"

UCLASS(BlueprintType)
class UStableDiffusionGetTaskResultOutput final : public UStableDiffusionOutputsBase
{
	GENERATED_BODY()
	
public:
	UStableDiffusionGetTaskResultOutput(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
	: Super(ObjectInitializer), Texture2DDynamic(nullptr)
	{
		
	};
	
	UPROPERTY(BlueprintReadOnly)
	UTexture2DDynamic* Texture2DDynamic;
};

/**
 * UComfyUIGetTaskResultAsyncAction
 */
UCLASS(BlueprintType, meta = (NotPlaceable, ExposedAsyncProxy = RequestTask))
class STABLEDIFFUSIONSERVICES_API UComfyUIGetTaskResultAsyncAction final : public UStableDiffusionHTTPAsyncActionBase
{
	GENERATED_BODY()

public:
	UComfyUIGetTaskResultAsyncAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
		* 帮助节点轻松连接到服务器并处理连接事件。
		* @param WorldContextObject 代表了函数调用所处的世界。
		* @param FileName 需要获取的文件名。
		* @param Type 获取类型。
		* @param InTimeout 连接的时长。
	*/
	UFUNCTION(BlueprintCallable, Category = "ComfyUI", meta = (DisplayName = "Get task result from server ComfyUI", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UComfyUIGetTaskResultAsyncAction* Connect(const UObject* WorldContextObject, const FString& FileName, const FString& Type, int InTimeout = 120);

	/** UStableDiffusionAsyncActionBase interface */
	UFUNCTION(BlueprintCallable, Category = ComfyUIListenAsyncAction)
	virtual void Cancel() override;

private:
	/** UStableDiffusionHTTPAsyncActionBase interface */
	virtual void InitHTTP(const FString& InUrl, EHTTPRequestType InRequestType, const TMap<FString, FString>& InHeaders, const FString& InRequestBody, int32 InTimeout = 120) override;
	virtual void OnRequestProgressInternal(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived) override;
	virtual void OnProcessRequestCompleteInternal(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully) override;

	
};
