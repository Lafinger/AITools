// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StableDiffusionHTTPAsyncActionBase.h"
#include "Interfaces/IHttpRequest.h"
#include "Txt2ImgAsyncAction.generated.h"

// USTRUCT(BlueprintType)
// struct FTxt2ImgPayload
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString prompt;
// 	
// 	UPROPERTY(BlueprintReadWrite)
// 	FString negative_prompt;
//
// 	// UPROPERTY(BlueprintReadWrite)
// 	// TArray<FString> styles;
// 	
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 seed = -1;
// 	
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 subseed = -1;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 subseed_strength = 0;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 seed_resize_from_h = -1;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 seed_resize_from_w = -1;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString sampler_name;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString scheduler = TEXT("Automatic");
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 batch_size = 1;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 n_iter = 1;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 steps = 20;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 cfg_scale = 7;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 width = 512;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 height = 512;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	bool restore_faces = true;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	bool tiling = true;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	bool do_not_save_samples = false;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	bool do_not_save_grid = false;
// 	
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 eta = 0;
// 	
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 denoising_strength = 0;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 s_min_uncond = 0;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 s_churn = 0;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 s_tmax = 0;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 s_tmin = 0;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 s_noise = 0;
//
// 	// UPROPERTY(BlueprintReadWrite)
// 	// FJsonObject steps = 50;
// 	// "override_settings": {},
//
// 	UPROPERTY(BlueprintReadWrite)
// 	bool override_settings_restore_afterwards = true;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString refiner_checkpoint;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 refiner_switch_at = 0;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	bool disable_extra_networks = false;
// 	
// 	UPROPERTY(BlueprintReadWrite)
// 	FString firstpass_image;
//
// 	// UPROPERTY(BlueprintReadWrite)
// 	// FJsonObject steps = 50;
// 	// "comments": {},
//
// 	UPROPERTY(BlueprintReadWrite)
// 	bool enable_hr = false;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 firstphase_width = 0;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 firstphase_height = 0;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 hr_scale = 2;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString hr_upscaler;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 hr_second_pass_steps = 0;
// 	
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 hr_resize_x = 0;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 hr_resize_y = 0;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString hr_checkpoint_name;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString hr_sampler_name;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString hr_scheduler;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString hr_prompt;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString hr_negative_prompt;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString force_task_id;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString sampler_index = TEXT("DPM++ 2M");
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString script_name;
//
// 	// UPROPERTY(BlueprintReadWrite)
// 	// TArray<FString> script_args;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	bool send_images = true;
// 	
// 	UPROPERTY(BlueprintReadWrite)
// 	bool save_images = false;
//
// 	// UPROPERTY(BlueprintReadWrite)
// 	// FJsonObject steps = 50;
// 	// "alwayson_scripts": {},
//
// 	UPROPERTY(BlueprintReadWrite)
// 	FString infotext;
// };


/**
 * Stable diffusion text to image async action
 */
UCLASS()
class STABLEDIFFUSIONWEBTOOL_API UTxt2ImgAsyncAction final : public UStableDiffusionHTTPAsyncActionBase
{
	GENERATED_UCLASS_BODY()
public:
	//http://127.0.0.1:7860/sdapi/v1/txt2img
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UTxt2ImgAsyncAction* Txt2ImgWithParamsFile(const UObject* WorldContextObject, const FString& InPrompt, const FString& InNegativePrompt, const FString& InParamsFilePath, const FString& InHostOrIP = TEXT("127.0.0.1"), int32 InPort = 7860);
	
	// UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	// static UTxt2ImgAsyncAction* Txt2ImgWithStruct(const UObject* WorldContextObject, const FTxt2ImgPayload& InJsonStruct, const FString& InHostOrIP = TEXT("127.0.0.1"), int32 InPort = 7860);
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UTxt2ImgAsyncAction* Txt2ImgWithJsonString(const UObject* WorldContextObject, const FString& InJsonString, const FString& InHostOrIP = TEXT("127.0.0.1"), int32 InPort = 7860);
	
	void Txt2ImgInternal(const UObject* WorldContextObject, const FString& InUrl, const FString& InPayload);
	
	void OnTxt2ImgCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

private:
	FString Url;
};
