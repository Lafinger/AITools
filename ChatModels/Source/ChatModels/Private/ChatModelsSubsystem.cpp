// Copyright 2023 AIRT All Rights Reserved.


#include "ChatModelsSubsystem.h"

DEFINE_LOG_CATEGORY(LogChatModelsSubsystem);

void UChatModelsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogChatModelsSubsystem, Display, TEXT("ThreadID:%d, %s: Chat models subsystem initialized."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
}

void UChatModelsSubsystem::Deinitialize()
{
	UE_LOG(LogChatModelsSubsystem, Display, TEXT("ThreadID:%d, %s: Chat models subsystem deinitialized."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	Super::Deinitialize();	
}

void UChatModelsSubsystem::AddHistory(const FContextHistory& InHistory)
{
	ContextHistories.Add(InHistory);
}

void UChatModelsSubsystem::ClearHistories()
{
	ContextHistories.Empty();
}

bool UChatModelsSubsystem::CheckDifyModelType(EDifyModelType ModelType)
{
	const UChatModelsSettings* Settings = GetDefault<UChatModelsSettings>();
	if(!Settings)
	{
		UE_LOG(LogChatModelsSubsystem, Error, TEXT("ThreadID:%d, %s: Check dify model type error."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return false;
	}
	FString APIKey = *Settings->DifySetting.APIKeys.Find(ModelType);
	if(APIKey.IsEmpty())
	{
		UE_LOG(LogChatModelsSubsystem, Warning, TEXT("ThreadID:%d, %s: Dify model APIKey is empty."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return false;
	}
	return true;
}

FString UChatModelsSubsystem::RemoveStringLineBreak(const FString& String)
{
	FString Result = String;
	int Index = 0;
	TArray<int> PointIndexs;
	
	while(-1 != Index)
	{
		Index = -1;
		Result.FindChar('\n', Index);
		if(-1 != Index)
		{
			// if(Index - 1 >= 0 && Index - 1 < Result.Len())
			// {
			// 	TCHAR Char = Result[Index - 1];
			// 	int CharInt = Char;
			// 	if(12290 == CharInt) // 12290 == '。' 
			// 	{
			// 		PointIndexs.AddUnique(Index);
			// 	}
			// 	// UE_LOG(LogChatModelsSubsystem, Log, TEXT("去除换行符, 前面的Char为：%c : %d"), Char, CharInt);
			// }
			Result.RemoveAt(Index);
			// UE_LOG(LogChatModelsSubsystem, Display, TEXT("ThreadID:%d, %s: Remove line break."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		}
	}

	// Result = Result.Replace(*FString(L"。"), *FString(L"。\n"));
	return Result;
}

TArray<FString> UChatModelsSubsystem::GetSpecialSplitStrings(const FString& LongString, const FString& Delimiter, int32 NoLessThan)
{
	TArray<FString> SplitStrings;
	if(LongString.Len() < NoLessThan)
	{
		UE_LOG(LogChatModelsSubsystem, Warning, TEXT("ThreadID:%d, %s: Input long string is less than %d!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), NoLessThan);
		return SplitStrings;
	}
	FString TempLongString = RemoveStringLineBreak(LongString);

	TArray<FString> OutArray;
	TempLongString.ParseIntoArray(OutArray, *Delimiter, true);
	FString Temp;
	for (FString String : OutArray)
	{
		FString TempString = String.Append(TEXT("。"));
		Temp.Append(TempString);
		if(Temp.Len() >= NoLessThan)
		{
			SplitStrings.Add(Temp);
			Temp.Empty();
		}
	}
	if(!Temp.IsEmpty())
	{
		SplitStrings.Add(Temp);
	}
	return SplitStrings;
}
