// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StableDiffusionAsyncActionBase.h"
#include "ComfyUIListenAsyncAction.generated.h"
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
		* @param InUrl 我们要连接的服务器的 URL。
		* @param InProtocol 要使用的协议。 应该是“ws”或“wss”。
		* @param InHeaders 升级期间使用的标头。
	*/
	UFUNCTION(BlueprintCallable, Category = "ComfyUI", meta = (DisplayName = "Listen to server ComfyUI", AutoCreateRefTerm = "Headers", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UComfyUIListenAsyncAction* Connect(const UObject* WorldContextObject);

	/** UStableDiffusionAsyncActionBase interface */
	UFUNCTION(BlueprintCallable, Category = "ComfyUI")
	virtual void Cancel() override;

private:
	/** UStableDiffusionWebSocketAsyncActionBase interface */
	virtual void InitWebSocket(const FString& InUrl, const TMap<FString, FString>& InHeaders) override;
	virtual void OnConnectedInternal() override;
	virtual void OnConnectionErrorInternal(const FString& Error) override;
	virtual void OnClosedInternal(int32 StatusCode, const FString& Reason, bool bWasClean) override;
	virtual void OnMessageInternal(const FString& MessageString) override;
};
