// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/IVDeveloperSettings.h"

#include "VoiceRecognitionSettings.generated.h"

/**
 * 
 */
UCLASS(config = VoiceRecognition, defaultconfig, meta = (DisplayName = "Voice Recognition Settings"))
class VOICERECOGNITION_API UVoiceRecognitionSettings : public UIVDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Gets the settings container name for the settings, either Project or Editor */
	virtual FName GetContainerName() const override
	{
		return FName("Project");
	}
	/** Gets the category for the settings, some high level grouping like, Editor, Engine, Game...etc. */
	virtual FName GetCategoryName() const override
	{

		return FName("VoiceRecognition");
	}
	/** The unique name for your section of settings, uses the class's FName. */
	virtual FName GetSectionName() const override
	{
		return FName("Settings");
	}

	/** 语音识别服务器链接 */
	UPROPERTY(EditAnywhere, config, Category = Settings)
	FString ServerUrl;
	
	static UVoiceRecognitionSettings* Get();
};
