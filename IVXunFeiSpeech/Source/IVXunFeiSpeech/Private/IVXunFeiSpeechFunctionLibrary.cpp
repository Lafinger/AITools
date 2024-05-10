// Fill out your copyright notice in the Description page of Project Settings.


#include "IVXunFeiSpeechFunctionLibrary.h"

#include "IVXunFeiSpeechTypes.h"
#include "IVXunFeiTTSSubsystem.h"

TArray<FSparkDeskMessage> UIVXunFeiSpeechFunctionLibrary::History;

void UIVXunFeiSpeechFunctionLibrary::XunFeiBeginRealTimeSpeechToText(FXunFeiRealTimeSTTOptions XunFeiRealTimeSTTOptions,
                                                                     FInitRealTimeSTTDelegate InitRealTimeSTTDelegate,
                                                                     FRealTimeSTTNoTranslateDelegate RealTimeSTTNoTranslateDelegate,
                                                                     FRealTimeSTTTranslateDelegate RealTimeSTTTranslateDelegate)
{
	if(UIVXunFeiTTSSubsystem* Subsystem = GEngine->GetEngineSubsystem<UIVXunFeiTTSSubsystem>())
	{
		Subsystem->BeginRealTimeSpeechToText(XunFeiRealTimeSTTOptions,InitRealTimeSTTDelegate,RealTimeSTTNoTranslateDelegate,RealTimeSTTTranslateDelegate);
	}
	else
	{
		InitRealTimeSTTDelegate.ExecuteIfBound(false, TEXT("WorldContextObject is null"));
	}
}

void UIVXunFeiSpeechFunctionLibrary::XunFeiStopRealTimeSpeechToText()
{
	if(UIVXunFeiTTSSubsystem* Subsystem = GEngine->GetEngineSubsystem<UIVXunFeiTTSSubsystem>())
	{
		Subsystem->StopRealTimeSpeechToText();
	}
}

void UIVXunFeiSpeechFunctionLibrary::XunFeiBeginStreamingSpeechToText(FXunFeiStreamingSTTOptions XunFeiStreamingSTTOptions)
{
	UIVXunFeiTTSSubsystem* Subsystem = GEngine->GetEngineSubsystem<UIVXunFeiTTSSubsystem>();
	check(Subsystem && "UIVXunFeiTTSSubsystem is nullptr!");
	Subsystem->BeginStreamingSpeechToText(XunFeiStreamingSTTOptions);
}

void UIVXunFeiSpeechFunctionLibrary::XunFeiStopStreamingSpeechToText()
{
	UIVXunFeiTTSSubsystem* Subsystem = GEngine->GetEngineSubsystem<UIVXunFeiTTSSubsystem>();
	check(Subsystem && "UIVXunFeiTTSSubsystem is nullptr!");
	Subsystem->StopStreamingSpeechToText();
}

void UIVXunFeiSpeechFunctionLibrary::XunFeiBeginTranslate(const FString& SourceText, FXunFeiTranslateOptions XunFeiTranslateOptions, FTranslateCompletedDelegate TranslateCompletedDelegate)
{
	UIVXunFeiTTSSubsystem* Subsystem = GEngine->GetEngineSubsystem<UIVXunFeiTTSSubsystem>();
	check(Subsystem && "UIVXunFeiTTSSubsystem is nullptr!");
	Subsystem->BeginTranslate(SourceText,XunFeiTranslateOptions, TranslateCompletedDelegate);
}

void UIVXunFeiSpeechFunctionLibrary::XunFeiStopTranslate()
{
	UIVXunFeiTTSSubsystem* Subsystem = GEngine->GetEngineSubsystem<UIVXunFeiTTSSubsystem>();
	check(Subsystem && "UIVXunFeiTTSSubsystem is nullptr!");
	Subsystem->StopTranslate();
}

void UIVXunFeiSpeechFunctionLibrary::ClearHistory()
{
	History.Empty();
}

void UIVXunFeiSpeechFunctionLibrary::AddHistory(FSparkDeskMessage InHistory)
{
	History.Add(InHistory);
}
