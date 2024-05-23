// Copyright 2023 AIRT All Rights Reserved.


#include "StableDiffusionServicesSubsystem.h"

DEFINE_LOG_CATEGORY(LogStableDiffusionServicesSubsystem);

void UStableDiffusionServicesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogStableDiffusionServicesSubsystem, Display, TEXT("ThreadID:%d, %s: Stable diffusion services subsystem initialized."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));

	ClientID = FGuid::NewGuid();
}

void UStableDiffusionServicesSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
	ClientID.Invalidate();
	UE_LOG(LogStableDiffusionServicesSubsystem, Display, TEXT("ThreadID:%d, %s: Stable diffusion services subsystem deinitialized."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
}

FString UStableDiffusionServicesSubsystem::GetClientID()
{
	return ClientID.ToString();
}
