// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "StableDiffusionHTTPAsyncActionBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(StableDiffusionHTTPAsyncAction, Log, All);

class UTexture2DDynamic;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStableDiffusionBaseDelegate, FString, Message, UTexture2DDynamic*, Texture);

UCLASS(Abstract)
class STABLEDIFFUSIONWEBTOOL_API UStableDiffusionHTTPAsyncActionBase  : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	// 每次的响应
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Process"))
	FStableDiffusionBaseDelegate ProcessDelegate;
	
	// 完成
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Completed"))
	FStableDiffusionBaseDelegate CompletedDelegate;
	
	// 取消
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Cancel"))
	FStableDiffusionBaseDelegate CancelDelegate;

	// 失败
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Error"))
	FStableDiffusionBaseDelegate ErrorDelegate;
	
	UStableDiffusionHTTPAsyncActionBase(const FObjectInitializer& ObjectInitializer);

	/** UBlueprintAsyncActionBase interface */
	// virtual void Activate() override;

	virtual void SetReadyToDestroy() override;
	
	/** UStableDiffusionAsyncActionBase interface */
	virtual void Cancel();
	
// protected:
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest;
	// 是否取消
	bool bIsCancel;
};

