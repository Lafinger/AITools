// Copyright 2023 AIRT,  All Rights Reserved.

#include "IVProcess/IVExecutableExtensionSettings.h"

#include "IVCommon.h"
#include "iVisualFileHeplerBPLibrary.h"
#include "IVProcess/IVProcessTypes.h"


UIVExecutableExtensionSettings::UIVExecutableExtensionSettings(const FObjectInitializer& ObjectInitializer)
{
	FIVProcInfo ExecutableExtensionSetting;
	ExecutableExtensionSetting.bActive=true;
	ExecutableExtensionSetting.Path=TEXT("LocalExe://SocketIOService");
	ExecutableExtensionSetting.ExeName=TEXT("socket-node-service.exe");
	ExecutableExtensionSetting.CommandLineArguments=FString();
	ExecutableExtensionSetting.bWindowHidden=true;
	ExecutableExtensionSetting.bLogCatch=false;

	ProcessList.Add(ExecutableExtensionSetting);

}

FName UIVExecutableExtensionSettings::GetCategoryName() const
{
	return TEXT("IVPlugins");
}

UIVExecutableExtensionSettings* UIVExecutableExtensionSettings::Get()
{
	UIVExecutableExtensionSettings* Settings = GetMutableDefault<UIVExecutableExtensionSettings>();
	return Settings;
}

void UIVExecutableExtensionSettings::PostInitProperties()
{
	Super::PostInitProperties();

	
	for(FIVProcInfo ExecutableExtensionSetting:ProcessList)
	{
		if(ExecutableExtensionSetting.bActive)
		{
			FString OutProcessID;
#if PLATFORM_WINDOWS			
			FString FileExe=ExecutableExtensionSetting.Path/ExecutableExtensionSetting.ExeName+TEXT(".exe");
#elif PLATFORM_LINUX
			FString FileExe=ExecutableExtensionSetting.Path/ExecutableExtensionSetting.ExeName;
#endif			
			bool bSuccessed=UiVisualFileHeplerBPLibrary::LoadProc(FileExe
				,ExecutableExtensionSetting.CommandLineArguments
				,ExecutableExtensionSetting.bWindowHidden
				,ExecutableExtensionSetting.bLogCatch
				,OutProcessID);

			if(bSuccessed)
			{
				UE_LOG(LogIVFileHepler,Log,TEXT("Application is running at %s"),*FileExe);
			}
			else
			{
				UE_LOG(LogIVFileHepler,Warning,TEXT("No valid resoure  file found at %s"),*FileExe);
			}
		}
	}
	

}

void UIVExecutableExtensionSettings::BeginDestroy()
{
	Super::BeginDestroy();
	
	UiVisualFileHeplerBPLibrary::UnLoadProcList();
}


