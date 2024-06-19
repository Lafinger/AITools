// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/IVDeveloperSettings.h"
#include "StableDiffusionServicesSettings.generated.h"

USTRUCT(BlueprintType)
struct FComfyUISettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, config, Category = "ComfyUI Settings")
	FString Host;

	UPROPERTY(EditAnywhere, config, Category = "ComfyUI Settings")
	int32 Port;

	UPROPERTY(EditAnywhere, config, Category = "ComfyUI Settings")
	FString WorkflowFileName;

	UPROPERTY(EditAnywhere, config, Category = "ComfyUI Settings")
	FString WorkflowAPIFileName;

	FORCEINLINE FString GetListenUrl() const { return "ws://" + Host + ":" + FString::FromInt(Port) + "/ws"; }
	FORCEINLINE FString GetSubmitTaskUrl() const { return "http://" + Host + ":" + FString::FromInt(Port) + "/prompt"; }
	FORCEINLINE FString GetTaskInfoUrl() const { return "http://" + Host + ":" + FString::FromInt(Port) + "/history"; }
	FORCEINLINE FString GetTaskViewUrl() const { return "http://" + Host + ":" + FString::FromInt(Port) + "/view"; }
};

/**
 * 继承了UIVDeveloperSettings，拥有将配置信息序列化成Json文件的能力，Json文件路径为：{项目根目录}/Config/ProjectJsonConfig/*。
 */
UCLASS(config = StableDiffusionServices, defaultconfig, meta = (DisplayName = "Stable Diffusion Services Settings"))
class STABLEDIFFUSIONSERVICES_API UStableDiffusionServicesSettings : public UIVDeveloperSettings
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

		return FName("StableDiffusionServices");
	}
	/** The unique name for your section of settings, uses the class's FName. */
	virtual FName GetSectionName() const override
	{
		return FName("Settings");
	}
	
	static UStableDiffusionServicesSettings* Get();

	UPROPERTY(EditAnywhere, config, Category = "StableDiffusionServices")
	FComfyUISettings ComfyUISettings;
};
