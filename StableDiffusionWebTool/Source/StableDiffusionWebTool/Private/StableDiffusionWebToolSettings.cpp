// Fill out your copyright notice in the Description page of Project Settings.


#include "StableDiffusionWebToolSettings.h"

UStableDiffusionWebToolSettings* UStableDiffusionWebToolSettings::Get()
{
	static UStableDiffusionWebToolSettings* Instance = GetMutableDefault<UStableDiffusionWebToolSettings>();
	return Instance;
}
