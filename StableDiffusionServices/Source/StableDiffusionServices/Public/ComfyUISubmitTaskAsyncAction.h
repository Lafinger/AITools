// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StableDiffusionAsyncActionBase.h"
#include "ComfyUISubmitTaskAsyncAction.generated.h"

UCLASS(BlueprintType)
class UStableDiffusionSubmitTaskOutput final : public UStableDiffusionOutputsBase
{
	GENERATED_BODY()
	
public:
	UStableDiffusionSubmitTaskOutput(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
	: Super(ObjectInitializer), Number(-1)
	{
		
	};
	
	UPROPERTY(BlueprintReadOnly)
	FString PromptID;

	UPROPERTY(BlueprintReadOnly)
	int32 Number;

	// UPROPERTY(BlueprintReadOnly)
	// FString NodeError;
};

/**
 * UComfyUISubmitTaskAsyncAction
 */
UCLASS(BlueprintType, meta = (NotPlaceable, ExposedAsyncProxy = RequestTask))
class STABLEDIFFUSIONSERVICES_API UComfyUISubmitTaskAsyncAction final: public UStableDiffusionHTTPAsyncActionBase
{
	GENERATED_BODY()

public:
	UComfyUISubmitTaskAsyncAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
		* 帮助节点轻松连接到服务器并处理连接事件。
		* @param WorldContextObject 代表了函数调用所处的世界。
		* @param InRequestBody 请求的内容。
		* @param InTimeout 连接的时长。
	*/
	UFUNCTION(BlueprintCallable, Category = "ComfyUI", meta = (DisplayName = "Submit task to server ComfyUI", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UComfyUISubmitTaskAsyncAction* Connect(const UObject* WorldContextObject, const FString& InPrompt, int32 InTimeout = 120);

	/** UStableDiffusionAsyncActionBase interface */
	UFUNCTION(BlueprintCallable, Category = ComfyUIListenAsyncAction)
	virtual void Cancel() override;

private:
	/** UStableDiffusionHTTPAsyncActionBase interface */
	virtual void InitHTTP(const FString& InUrl, EHTTPRequestType InRequestType, const TMap<FString, FString>& InHeaders, const FString& InRequestBody, int32 InTimeout = 120) override;
	virtual void OnRequestProgressInternal(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived) override;
	virtual void OnProcessRequestCompleteInternal(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully) override;
};
