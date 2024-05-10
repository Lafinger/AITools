// Copyright Epic Games, Inc. All Rights Reserved.

#include "IVCommon.h"

DEFINE_LOG_CATEGORY(LogIVJson);
DEFINE_LOG_CATEGORY(LogIVFileHepler);
DEFINE_LOG_CATEGORY(LogIVTween);
DEFINE_LOG_CATEGORY(IVJsonConfig); 
DEFINE_LOG_CATEGORY(LogIVAudioCapture);

#define LOCTEXT_NAMESPACE "FIVCommonModule"

void FIVCommonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FIVCommonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FIVCommonModule, IVCommon)