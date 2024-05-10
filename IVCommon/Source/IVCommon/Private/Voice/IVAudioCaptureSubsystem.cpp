// Fill out your copyright notice in the Description page of Project Settings.


#include "Voice/IVAudioCaptureSubsystem.h"
#include "Generators/AudioGenerator.h"
#include "AudioCapture.h"
#include "IVCommon.h"

void UIVAudioCaptureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogIVAudioCapture, Display, TEXT("%s: IVAudioCapture Engine Subsystem initialized."), *FString(__func__));
}

void UIVAudioCaptureSubsystem::Deinitialize()
{
	UE_LOG(LogIVAudioCapture, Display, TEXT("%s: IVAudioCapture Engine Subsystem deinitialized."), *FString(__func__));
	StopCapturingAudio();
	
	Super::Deinitialize();
}

void UIVAudioCaptureSubsystem::StartCapturingAudio()
{
	if (!AudioCapture)
	{
		StopCapturingAudio();
	}

	AudioCapture = UAudioCaptureFunctionLibrary::CreateAudioCapture();

	if(const UIVAudioCaptureSubsystem* const Subsystem = GEngine->GetEngineSubsystem<UIVAudioCaptureSubsystem>())
	{
		AudioGeneratorHandle = AudioCapture->AddGeneratorDelegate(&UIVAudioCaptureSubsystem::OnAudioGenerate);
	}
	

	if (AudioCapture)
	{
		ClearVoiceData();

		AudioCapture->StartCapturingAudio();
	}
}

void UIVAudioCaptureSubsystem::StopCapturingAudio()
{
	if (AudioCapture)
	{
		AudioCapture->StopCapturingAudio();
		AudioCapture->RemoveGeneratorDelegate(AudioGeneratorHandle);
		AudioCapture = nullptr;
	}

	ClearVoiceData();
}

bool UIVAudioCaptureSubsystem::IsCapturingAudio()
{
	if (AudioCapture)
	{
		return AudioCapture->IsCapturingAudio();
	}

	return false;
}

bool UIVAudioCaptureSubsystem::GetAudioCaptureDeviceInfo(FAudioCaptureDeviceInfo& OutInfo)
{
	if (AudioCapture)
	{
		return AudioCapture->GetAudioCaptureDeviceInfo(OutInfo);
	}

	return false;
}

void UIVAudioCaptureSubsystem::AppendVoiceData(const TArray<float>& InVoiceData)
{
	{
		FScopeLock VoiceDataScopeLock(&VoiceDataCriticalSection);
		VoiceData.Append(InVoiceData);
	}
}

bool UIVAudioCaptureSubsystem::GetVoiceData(TArray<float>& OutVoiceData)
{
	OutVoiceData.Empty();

	if (VoiceData.Num() > 1024)
	{
		OutVoiceData.Append(VoiceData.GetData(), 1024);
		
		{
			FScopeLock VoiceDataScopeLock(&VoiceDataCriticalSection);
			VoiceData.RemoveAt(0, 1024);
		}
		
		return true;
	}
	
	return false;
}

void UIVAudioCaptureSubsystem::ClearVoiceData()
{
	{
		FScopeLock VoiceDataScopeLock(&VoiceDataCriticalSection);
		VoiceData.Empty();
	}
}

void UIVAudioCaptureSubsystem::OnAudioGenerate(const float* InAudio, int32 NumSamples)
{
	static int32 IndexSend = 0;
	TArray<float> VoiceDataToApeend;
	int32 VoiceIndex = 0;

	if (NumSamples == 2048)
	{

		if (IndexSend == 0)
		{
			IndexSend++;
			for (int32 VoiceNum = 0; VoiceNum < 341; VoiceNum++)
			{
				VoiceDataToApeend.Add(InAudio[VoiceIndex]);
				VoiceIndex += 6;
			}

		}
		else if (IndexSend == 1)
		{
			IndexSend++;

			for (int32 VoiceNum = 341; VoiceNum < 682; VoiceNum++)
			{
				VoiceDataToApeend.Add(InAudio[VoiceIndex]);

				VoiceIndex += 6;
			}
		}
		else if (IndexSend == 2)
		{

			IndexSend = 0;

			for (int32 VoiceNum = 682; VoiceNum < 1024; VoiceNum++)
			{
				VoiceDataToApeend.Add(InAudio[VoiceIndex]);
				VoiceIndex += 6;
			}
		}
		

	}
	else if (NumSamples == 960)
	{
		for (int32 VoiceNum = 0; VoiceNum < 160; VoiceNum++)
		{
			VoiceDataToApeend.Add(InAudio[VoiceIndex]);
			VoiceIndex += 6;
		}
	}
	else if (NumSamples == 480)
	{
		for (int32 VoiceNum = 0; VoiceNum < 160; VoiceNum++)
		{
			VoiceDataToApeend.Add(InAudio[VoiceIndex]);
			VoiceIndex += 3;
		}
	}
	else if (NumSamples == 320)
	{
		for (int32 VoiceNum = 0; VoiceNum < 160; VoiceNum++)
		{
			VoiceDataToApeend.Add(InAudio[VoiceIndex]);
			VoiceIndex += 2;
		}

	}
	else if (NumSamples == 160)
	{
		for (int32 VoiceNum = 0; VoiceNum < 160; VoiceNum++)
		{
			VoiceDataToApeend.Add(InAudio[VoiceIndex]);
			VoiceIndex += 1;
		}

	}

	if( UIVAudioCaptureSubsystem*  Subsystem = GEngine->GetEngineSubsystem<UIVAudioCaptureSubsystem>())
	{
		Subsystem->AppendVoiceData(VoiceDataToApeend);
	}
	
}
