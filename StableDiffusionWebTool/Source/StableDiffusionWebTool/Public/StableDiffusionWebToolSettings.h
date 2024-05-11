// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/IVDeveloperSettings.h"
#include "StableDiffusionWebToolSettings.generated.h"

USTRUCT(BlueprintType)
struct FStableDiffusionWebToolSetting
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, config, Category = "Stable Diffusion Web Tool Settings")
	FString APPID;

	UPROPERTY(EditAnywhere, config, Category = "Stable Diffusion Web Tool Settings")
	FString APISecret;

	UPROPERTY(EditAnywhere, config, Category = "Stable Diffusion Web Tool Settings")
	FString APIKey;
	
};

/**
 * 继承了UIVDeveloperSettings，拥有将配置信息序列化成Json文件的能力，Json文件路径为：{项目根目录}/Config/ProjectJsonConfig/*。
 */
UCLASS(config = StableDiffusionWebTool, defaultconfig, meta = (DisplayName = "Stable Diffusion Web Tool Settings"))
class STABLEDIFFUSIONWEBTOOL_API UStableDiffusionWebToolSettings : public UIVDeveloperSettings
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

		return FName("StableDiffusionWebTool");
	}
	/** The unique name for your section of settings, uses the class's FName. */
	virtual FName GetSectionName() const override
	{
		return FName("Settings");
	}
	
	/**
	* 目前这个项目配置没用，因为与IVXunFeiSpeech里的大模型发生冲突
	*/
	UPROPERTY(EditAnywhere, config, Category = Settings)
	FStableDiffusionWebToolSetting StableDiffusionWebToolSetting;
	
	static UStableDiffusionWebToolSettings* Get();
};
