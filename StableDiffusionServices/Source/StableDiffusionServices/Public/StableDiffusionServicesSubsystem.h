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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Stable Diffusion Services")
	FString GetClientID();

private:
	FGuid ClientID;;
};
