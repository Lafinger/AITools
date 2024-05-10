// Fill out your copyright notice in the Description page of Project Settings.


#include "IVPlatformLibrary.h"


#include "IVCommon.h"
#include "Interfaces/IPluginManager.h"
#include "Kismet/GameplayStatics.h"


#if PLATFORM_ANDROID
#include <AndroidPermissionFunctionLibrary.h>
#endif

const bool UIVPlatformLibrary::CheckAndroidPermission(const FString& InPermission)
{
#if PLATFORM_ANDROID
	UE_LOG(LogCBPlatform, Display, TEXT("%s: Checking android permission: %s"), *FString(__func__), *InPermission);
	if (!UAndroidPermissionFunctionLibrary::CheckPermission(InPermission))
	{
		UAndroidPermissionFunctionLibrary::AcquirePermissions({ InPermission });
		return false;
	}

#else
	UE_LOG(LogIVFileHepler, Error, TEXT("%s: Platform %s is not supported"), *FString(__func__), *UGameplayStatics::GetPlatformName());
#endif

	return true;
}

const bool UIVPlatformLibrary::IsContentModuleAvailable(const FString& ModuleName)
{
	const FString QualifiedParam = QualifyModulePath(ModuleName);

	bool bOutput = false;
	for (const FString& Module : GetAvailableContentModules())
	{
		if (QualifyModulePath(Module).Contains(QualifiedParam, ESearchCase::IgnoreCase))
		{
			bOutput = true;
			break;
		}
	}

	return bOutput;
}

const FString UIVPlatformLibrary::QualifyModulePath(const FString& ModuleName)
{
	FString Output = ModuleName;

	if (!Output.StartsWith("/"))
	{
		Output = "/" + Output;
	}
	if (!Output.EndsWith("/"))
	{
		Output += '/';
	}

	return Output;
}

const TArray<FString> UIVPlatformLibrary::GetAvailableContentModules()
{
	TArray<FString> Output{ "Game" };

	IPluginManager& PluginManager = IPluginManager::Get();
	const TArray<TSharedRef<IPlugin>> PluginsArray = PluginManager.GetEnabledPluginsWithContent();

	for (const TSharedRef<IPlugin>& Plugin : PluginsArray)
	{
		if (Plugin->GetLoadedFrom() != EPluginLoadedFrom::Project)
		{
			continue;
		}

		Output.Add(Plugin->GetName());
	}

	return Output;
}
