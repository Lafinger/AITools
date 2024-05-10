// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/IVDeveloperSettings.h"
#include "IVXunFeiSpeechSettings.generated.h"

USTRUCT(BlueprintType)
struct IVXUNFEISPEECH_API FXunFeiStreamingSTTParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, config, Category = "SpaskDeskParams")
	FString APPID;

	UPROPERTY(EditAnywhere, config, Category = "SpaskDeskParams")
	FString APISecret;

	UPROPERTY(EditAnywhere, config, Category = "SpaskDeskParams")
	FString APIKey;
};

/**
 * 语音转文字用户参数
 */

USTRUCT(BlueprintType)
struct IVXUNFEISPEECH_API FXunFeiRealTimeSTTParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config,Category = "STTParams")
	FString	APPID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config,Category = "STTParams")
	FString	APIKey;
};

/**
 * 讯飞星火用户参数
 */
USTRUCT(BlueprintType)
struct IVXUNFEISPEECH_API FXunFeiSparkDeskParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, config, Category = "SpaskDeskParams")
	FString APPID;

	UPROPERTY(EditAnywhere, config, Category = "SpaskDeskParams")
	FString APISecret;

	UPROPERTY(EditAnywhere, config, Category = "SpaskDeskParams")
	FString APIKey;
};

/**
 * 语音合成用户参数
 */
USTRUCT(BlueprintType)
struct IVXUNFEISPEECH_API FXunFeiTTSParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, config, Category = "TTSParams")
	FString APPID;

	UPROPERTY(EditAnywhere, config, Category = "TTSParams")
	FString APISecret;

	UPROPERTY(EditAnywhere, config, Category = "TTSParams")
	FString APIKey;
};


/**
 * 科大讯飞机器翻译
 */
USTRUCT(BlueprintType)
struct IVXUNFEISPEECH_API FXunFeiTranslateParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, config, Category = "TTSParams")
	FString APPID;

	UPROPERTY(EditAnywhere, config, Category = "TTSParams")
	FString APISecret;

	UPROPERTY(EditAnywhere, config, Category = "TTSParams")
	FString APIKey;
};


UCLASS(Config = "IV" , meta = (DisplayName = "IV Xun Fei Speech Settings"))
class IVXUNFEISPEECH_API UIVXunFeiSpeechSettings : public UIVDeveloperSettings
{
	GENERATED_BODY()

public:
	UIVXunFeiSpeechSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;


public:

	/**语音转文字用户参数*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "StreamingSTTParams")
	FXunFeiStreamingSTTParams StreamingSTTParams;
	
	/**语音转文字用户参数*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "RealTimeSTTParams")
	FXunFeiRealTimeSTTParams RealTimeSTTParams;
	
	/**语音合成用户参数*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config,Category = "TTSParams")
	FXunFeiTTSParams TTSParams;
	
	/**讯飞星火用户参数*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config,Category ="SpaskDeskParams")
	FXunFeiSparkDeskParams SparkDeskParams;

	/**翻译参数*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "TranslateParams")
	FXunFeiTranslateParams TranslateParams;

public:
	UFUNCTION(BlueprintPure, Category = "IV AIGC|XunFeiSpeech|",DisplayName = "GeHuaWeiChatSettings")
	static UIVXunFeiSpeechSettings* Get();
};
