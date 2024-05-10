// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatModelsSettings.h"

FString FSparkModelSetting::GetStringDomain()
{
	switch (Domain)
	{
	case ESparkDomainType::general:
		return TEXT("general");
	case ESparkDomainType::generalv2:
		return TEXT("generalv2");
	case ESparkDomainType::generalv3:
		return TEXT("generalv3");
	}

	return FString();
}

FString FSparkModelSetting::GetVersion()
{
	switch (Domain)
	{
	case ESparkDomainType::general:
		return TEXT("v1.1");
	case ESparkDomainType::generalv2:
		return TEXT("v2.1");
	case ESparkDomainType::generalv3:
		return TEXT("v3.1");
	}

	return FString();
}

void FSparkModelSetting::AddQueryField(TSharedPtr<FJsonObject> JsonObject,bool bImageChat)
{
	if(bImageChat)
	{
		JsonObject->SetStringField("domain","general");
	}
	else
	{
		JsonObject->SetStringField("domain",GetStringDomain());
	}
	
	JsonObject->SetNumberField("temperature", Temperature);
	JsonObject->SetNumberField("max_tokens", MaxTokens);
	JsonObject->SetNumberField("top_k", TopK);
}

UChatModelsSettings* UChatModelsSettings::Get()
{
	static UChatModelsSettings* Instance = GetMutableDefault<UChatModelsSettings>();
	return Instance;
}
