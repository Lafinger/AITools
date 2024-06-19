// Copyright 2023 AIRT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "StableDiffusionServicesSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStableDiffusionServicesSubsystem, Log, All);

/**
 * 大模型管理子系统
 * 
 */
UCLASS(BlueprintType)
class STABLEDIFFUSIONSERVICES_API UStableDiffusionServicesSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Stable Diffusion Services")
	void InitComfyUIClientID();
	
	UFUNCTION(BlueprintPure, Category = "Stable Diffusion Services")
	FString GetComfyUIClientID();

private:
	FGuid ComfyUIClientID;
	FString ComfyUIClientIDString;
};
