// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
DECLARE_LOG_CATEGORY_EXTERN(LogIVJson, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogIVFileHepler, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogIVTween,Log,All);
DECLARE_LOG_CATEGORY_EXTERN(IVJsonConfig,Log,All);
DECLARE_LOG_CATEGORY_EXTERN(LogIVAudioCapture, Log, All);
class FIVCommonModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
