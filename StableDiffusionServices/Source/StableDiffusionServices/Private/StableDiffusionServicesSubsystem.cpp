// Copyright 2023 AIRT All Rights Reserved.


#include "StableDiffusionServicesSubsystem.h"

DEFINE_LOG_CATEGORY(LogStableDiffusionServicesSubsystem);

void UStableDiffusionServicesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitComfyUIClientID();
	UE_LOG(LogStableDiffusionServicesSubsystem, Display, TEXT("ThreadID:%d, %s: Stable diffusion services subsystem initialized."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__))
}

void UStableDiffusionServicesSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
	ComfyUIClientID.Invalidate();
	ComfyUIClientIDString.Empty();
	UE_LOG(LogStableDiffusionServicesSubsystem, Display, TEXT("ThreadID:%d, %s: Stable diffusion services subsystem deinitialized."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
}

void UStableDiffusionServicesSubsystem::InitComfyUIClientID()
{
	if(ComfyUIClientID.IsValid() && !ComfyUIClientIDString.IsEmpty())
	{
		UE_LOG(LogStableDiffusionServicesSubsystem, Display, TEXT("ThreadID:%d, %s: ComfyUI client id has been Initialized."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return;
	}
	ComfyUIClientID = FGuid::NewGuid();
	ComfyUIClientIDString = ComfyUIClientID.ToString(EGuidFormats::DigitsWithHyphens).ToLower();
}

FString UStableDiffusionServicesSubsystem::GetComfyUIClientID()
{
	return ComfyUIClientIDString;
}
