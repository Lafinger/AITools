// Fill out your copyright notice in the Description page of Project Settings.


#include "IVXunFeiSpeechSettings.h"

UIVXunFeiSpeechSettings::UIVXunFeiSpeechSettings(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

FName UIVXunFeiSpeechSettings::GetCategoryName() const
{
	return TEXT("IV AIGC");
}

UIVXunFeiSpeechSettings* UIVXunFeiSpeechSettings::Get()
{
	UIVXunFeiSpeechSettings* Settings = GetMutableDefault<UIVXunFeiSpeechSettings>();
	
	return Settings;
}
