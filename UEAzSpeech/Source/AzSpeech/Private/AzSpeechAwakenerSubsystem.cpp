// Fill out your copyright notice in the Description page of Project Settings.

#include "AzSpeechAwakenerSubsystem.h"
#include "AzSpeech/AzSpeechSettings.h"

#include <future>
#include <Async/Async.h>

DEFINE_LOG_CATEGORY(LogAzSpeechAwakenerSubsystem)

void UAzSpeechAwakenerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UAzSpeechAwakenerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UAzSpeechAwakenerSubsystem::StartKeywordRecognitionAsync()
{
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
	{
		// 加载自定义关键词模型
		const UAzSpeechSettings* AzSpeechSettings = UAzSpeechSettings::Get();
		const FString ModelPath = AzSpeechSettings->DefaultOptions.RecognitionOptions.KeywordRecognitionModelPath;
		ensureAlwaysMsgf(!ModelPath.IsEmpty() && FPaths::FileExists(ModelPath), TEXT("Must fill correct path of model file in project setting!"));
		this->Model = MicrosoftSpeech::KeywordRecognitionModel::FromFile(TCHAR_TO_UTF8(*ModelPath));
		ensureAlwaysMsgf(Model, TEXT("Load model file failed"));

		// 初始化语音识别
		auto Config = MicrosoftSpeech::SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");
		this->SpeechRecognizer = MicrosoftSpeech::SpeechRecognizer::FromConfig(Config);
		ensureAlwaysMsgf(SpeechRecognizer, TEXT("SpeechRecognizer init failed!"));
		if(!this->SpeechRecognizer)
		{
			UE_LOG(LogAzSpeechAwakenerSubsystem, Error, TEXT("SpeechRecognizer init failed!"));
			return;
		}
		
		// Promise for synchronization of recognition end.
		std::promise<void> recognition_end_promise;
		
		// Subscribes to events.
		// this->SpeechRecognizer->Recognizing.Connect([](const MicrosoftSpeech::SpeechRecognitionEventArgs& e)
		// 	{
		// 		if (e.Result->Reason == MicrosoftSpeech::ResultReason::RecognizingSpeech)
		// 		{
		// 			FString RecognizingText = UTF8_TO_TCHAR(e.Result->Text.c_str());
		// 			UE_LOG(LogAzSpeechAwakenerSubsystem, Display, TEXT("Recognizing text : %s"), *RecognizingText);
		// 		}
		// 		else if (e.Result->Reason == MicrosoftSpeech::ResultReason::RecognizingKeyword)
		// 		{
		// 			FString RecognizingText = UTF8_TO_TCHAR(e.Result->Text.c_str());
		// 			UE_LOG(LogAzSpeechAwakenerSubsystem, Display, TEXT("Recognizing keyword : %s"), *RecognizingText);
		// 		}
		// 	});

		this->SpeechRecognizer->Recognized.Connect([&recognition_end_promise, this](const MicrosoftSpeech::SpeechRecognitionEventArgs& e)
			{
				if (e.Result->Reason == MicrosoftSpeech::ResultReason::RecognizedKeyword)
				{
					FString RecognizedText = UTF8_TO_TCHAR(e.Result->Text.c_str());
					UE_LOG(LogAzSpeechAwakenerSubsystem, Display, TEXT("Recognized text : %s"), *RecognizedText);

					AsyncTask(ENamedThreads::GameThread, [this]()
					{
						this->AwakenerTriggerDelegate.Broadcast();
					});
					
					recognition_end_promise.set_value();
				}
				else if (e.Result->Reason == MicrosoftSpeech::ResultReason::RecognizedSpeech)
				{
					FString RecognizedText = UTF8_TO_TCHAR(e.Result->Text.c_str());
					UE_LOG(LogAzSpeechAwakenerSubsystem, Display, TEXT("Recognized keyword : %s"), *RecognizedText);
				}
				else if (e.Result->Reason == MicrosoftSpeech::ResultReason::NoMatch)
				{
					UE_LOG(LogAzSpeechAwakenerSubsystem, Display, TEXT("Speech could not be recognized"));
				}
			});

		// this->SpeechRecognizer->Canceled.Connect([](const MicrosoftSpeech::SpeechRecognitionCanceledEventArgs& e)
		// 	{
		// 		UE_LOG(LogAzSpeechAwakenerSubsystem, Display, TEXT("Cancle speech recognizing, Reason = %i"), (int32)e.Reason);
		// 		if (e.Reason == MicrosoftSpeech::CancellationReason::Error)
		// 		{
		// 			FString ErrorDetails = UTF8_TO_TCHAR(e.ErrorDetails.c_str());
		// 			UE_LOG(LogAzSpeechAwakenerSubsystem, Error, TEXT("Cancle speech recognizing error, ErrorCode = %i, ErrorDetails= %s"), (int32)e.ErrorCode, *ErrorDetails);
		// 		}
		// 	});

		this->SpeechRecognizer->SessionStarted.Connect([this](const MicrosoftSpeech::SessionEventArgs& e)
			{
				FString SessionId = UTF8_TO_TCHAR(e.SessionId.c_str());
				UE_LOG(LogAzSpeechAwakenerSubsystem, Display, TEXT("Session started, SessionId = %s"), *SessionId);
			});
		
		this->SpeechRecognizer->SessionStopped.Connect([this](const MicrosoftSpeech::SessionEventArgs& e)
			{
				FString SessionId = UTF8_TO_TCHAR(e.SessionId.c_str());
				UE_LOG(LogAzSpeechAwakenerSubsystem, Display, TEXT("Session Stopped, SessionId = %s"), *SessionId);
			});
		
		SpeechRecognizer->StartKeywordRecognitionAsync(this->Model).get();
		recognition_end_promise.get_future().get();
		StopKeywordRecognitionAsync();
		UE_LOG(LogAzSpeechAwakenerSubsystem, Display, TEXT("Keyword recognition finished"));
	});
	
	return true;
}

void UAzSpeechAwakenerSubsystem::StopKeywordRecognitionAsync()
{
	if(!SpeechRecognizer)
	{
		return;
	}
	SpeechRecognizer->StopKeywordRecognitionAsync().get();
	SpeechRecognizer.reset();
	UE_LOG(LogAzSpeechAwakenerSubsystem, Display, TEXT("Keyword recognition stop"));
}
