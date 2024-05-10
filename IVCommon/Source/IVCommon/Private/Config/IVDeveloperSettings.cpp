// Copyright 2023 AIRT,  All Rights Reserved.


#include "Config/IVDeveloperSettings.h"

#include "Config/IVConfig.h"
#include "Misc/FileHelper.h"

UIVDeveloperSettings::UIVDeveloperSettings(const FObjectInitializer& ObjectInitializer)
{
	FilePath=FPaths::ProjectConfigDir() / "ProjectJsonConfig"/ GetSectionName().ToString()+".json";
}

FName UIVDeveloperSettings::GetCategoryName() const
{
	return TEXT("IVPlugins");
}

void UIVDeveloperSettings::PostInitProperties()
{
	Super::PostInitProperties();
	
	FIVConfig Config;
	FString Contents;
	if(!FFileHelper::LoadFileToString(Contents, *FilePath))
	{
		return;
	}
	
	if (!Config.LoadFromString(Contents))
	{
		return;
	}

	Config.TryGetRootUObject(GetClass(),this);
}

#if WITH_EDITOR
void UIVDeveloperSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if(PropertyChangedEvent.Property!=nullptr)
	{
		FIVConfig Config;
		FString Result;
		
		Config.SetRootUObject(GetClass(),this);
		if (!Config.SaveToString(Result))
		{
			return ;
		}

		
		FFileHelper::SaveStringToFile(Result, *FilePath);
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif