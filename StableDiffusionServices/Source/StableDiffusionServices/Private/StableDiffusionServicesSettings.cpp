// Fill out your copyright notice in the Description page of Project Settings.


#include "StableDiffusionServicesSettings.h"

UStableDiffusionServicesSettings* UStableDiffusionServicesSettings::Get()
{
	static UStableDiffusionServicesSettings* Instance = GetMutableDefault<UStableDiffusionServicesSettings>();
	return Instance;
}
