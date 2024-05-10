// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IVPlatformLibrary.generated.h"

/**
 * 
 */
UCLASS()
class IVCOMMON_API UIVPlatformLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/** 检查android平台是否已有权限，没有则添加 */
	UFUNCTION(BlueprintCallable, Category = "IVPlatform | Utils", meta = (DisplayName = "Check and Add Android Permission"))
	static const bool CheckAndroidPermission(const FString& InPermission);

	/** 检查内容模块是否可用 */
	UFUNCTION(BlueprintPure, Category = "IVPlatform | Utils")
	static const bool IsContentModuleAvailable(const FString& ModuleName);

	/** 将模块名称字符串限定为单个字符串（如 /ModulePath/）*/
	UFUNCTION(BlueprintPure, Category = "IVPlatform | Utils", meta = (DisplayName = "Qualify Module Path"))
	static const FString QualifyModulePath(const FString& ModuleName);

	/** 获取启用content的可用模块 */
	UFUNCTION(BlueprintPure, Category = "AzSpeech | Utils")
	static const TArray<FString> GetAvailableContentModules();
};
