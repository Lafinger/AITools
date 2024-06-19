// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StableDiffusionAsyncActionBase.h"
#include "ComfyUIListenAsyncAction.generated.h"

UENUM(BlueprintType)
enum class ListenType : uint8
{
	None UMETA(DisplayName = "无"),
	Status UMETA(DisplayName = "状态"),
	ExecutionStart UMETA(DisplayName = "开始"),
	Executing UMETA(DisplayName = "执行"),
	Progress UMETA(DisplayName = "进度条"),
	Completed UMETA(DisplayName = "完成"),
};

UCLASS(BlueprintType)
class UStableDiffusionListenOutput final : public UStableDiffusionOutputsBase
{
	GENERATED_BODY()
	
public:
	UStableDiffusionListenOutput(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
	: Super(ObjectInitializer), Type(ListenType::None)
	{
		
	};
	
	UPROPERTY(BlueprintReadOnly)
	ListenType Type = ListenType::None;

	// ListenType : Status
	UPROPERTY(BlueprintReadOnly)
	int32 QueueRemaining;

	// ListenType : execution_start || executing || Completed
	UPROPERTY(BlueprintReadOnly)
	FString PromptID;

	// ListenType : executing
	UPROPERTY(BlueprintReadOnly)
	FString Node;

	// ListenType : progress
	UPROPERTY(BlueprintReadOnly)
	int32 Value = -1;
	UPROPERTY(BlueprintReadOnly)
	int32 Max = -1;
};

/**
 * UComfyUIListenAsyncAction
 */
UCLASS(BlueprintType, meta = (NotPlaceable, ExposedAsyncProxy = RequestTask))
class STABLEDIFFUSIONSERVICES_API UComfyUIListenAsyncAction final: public UStableDiffusionWebSocketAsyncActionBase
{
	GENERATED_BODY()
public:
	UComfyUIListenAsyncAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
		* 帮助节点轻松连接到服务器并处理连接事件。
		* @param WorldContextObject 代表了函数调用所处的世界。
	*/
	UFUNCTION(BlueprintCallable, Category = "ComfyUI", meta = (DisplayName = "Listen to server ComfyUI", AutoCreateRefTerm = "Headers", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UComfyUIListenAsyncAction* Connect(const UObject* WorldContextObject);

	/** UStableDiffusionAsyncActionBase interface */
	UFUNCTION(BlueprintCallable, Category = "ComfyUI")
	virtual void Cancel() override;

	UPROPERTY()
	UStableDiffusionListenOutput* StableDiffusionListenOutput;

private:
	/** UStableDiffusionWebSocketAsyncActionBase interface */
	virtual void InitWebSocket(const FString& InUrl, const TArray<FString>& InProtocols = TArray<FString>(), const TMap<FString, FString>& InHeaders = TMap<FString, FString>()) override;
	virtual void OnConnectedInternal() override;
	virtual void OnConnectionErrorInternal(const FString& Error) override;
	virtual void OnClosedInternal(int32 StatusCode, const FString& Reason, bool bWasClean) override;
	virtual void OnMessageInternal(const FString& MessageString) override;
};
