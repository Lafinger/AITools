// Fill out your copyright notice in the Description page of Project Settings.


#include "VoiceWakeUpSettings.h"

UVoiceWakeUpSettings* UVoiceWakeUpSettings::Get()
{
	static UVoiceWakeUpSettings* Instance = GetMutableDefault<UVoiceWakeUpSettings>();
	return Instance;
}
