// // Fill out your copyright notice in the Description page of Project Settings.
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "StableDiffusionAsyncActionBase.h"
// #include "Interfaces/IHttpRequest.h"
// #include "WebUITxt2ImgAsyncAction.generated.h"
//
//
//
// /**
//  * Stable diffusion text to image async action
//  */
// UCLASS()
// class STABLEDIFFUSIONSERVICES_API UWebUITxt2ImgAsyncAction final : public UStableDiffusionAsyncActionBase
// {
// 	GENERATED_UCLASS_BODY()
// public:
// 	//http://127.0.0.1:7860/sdapi/v1/txt2img
// 	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
// 	static UWebUITxt2ImgAsyncAction* Txt2ImgWithParamsFile(const UObject* WorldContextObject, const FString& InPrompt, const FString& InNegativePrompt, const FString& InParamsFilePath, const FString& InHostOrIP = TEXT("127.0.0.1"), int32 InPort = 7860);
// 	
// 	// UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
// 	// static UTxt2ImgAsyncAction* Txt2ImgWithStruct(const UObject* WorldContextObject, const FTxt2ImgPayload& InJsonStruct, const FString& InHostOrIP = TEXT("127.0.0.1"), int32 InPort = 7860);
// 	
// 	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
// 	static UWebUITxt2ImgAsyncAction* Txt2ImgWithJsonString(const UObject* WorldContextObject, const FString& InJsonString, const FString& InHostOrIP = TEXT("127.0.0.1"), int32 InPort = 7860);
// 	
// 	void Txt2ImgInternal(const UObject* WorldContextObject, const FString& InUrl, const FString& InPayload);
// 	
// 	void OnTxt2ImgCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
//
// protected:
// 	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest;
// 	
// private:
// 	FString Url;
// };
