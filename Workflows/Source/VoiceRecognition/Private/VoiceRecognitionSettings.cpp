// Fill out your copyright notice in the Description page of Project Settings.


#include "VoiceRecognitionSettings.h"

UVoiceRecognitionSettings* UVoiceRecognitionSettings::Get()
{
	static UVoiceRecognitionSettings* Instance = GetMutableDefault<UVoiceRecognitionSettings>();
	return Instance;
}
