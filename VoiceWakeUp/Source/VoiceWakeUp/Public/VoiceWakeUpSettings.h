// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/IVDeveloperSettings.h"
#include "VoiceWakeUpSettings.generated.h"


UCLASS(config = VoiceWakeUp, defaultconfig, meta = (DisplayName = "Voice Wake Up Settings"))
class VOICEWAKEUP_API UVoiceWakeUpSettings : public UIVDeveloperSettings
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

		return FName("VoiceWakeUp");
	}
	/** The unique name for your section of settings, uses the class's FName. */
	virtual FName GetSectionName() const override
	{
		return FName("Settings");
	}

	/** 更换APPID需要同时更换SDK */
	UPROPERTY(EditAnywhere, config, Category = Settings)
	FString APPID = TEXT("ea2a4eaa");
	
	static UVoiceWakeUpSettings* Get();
};
