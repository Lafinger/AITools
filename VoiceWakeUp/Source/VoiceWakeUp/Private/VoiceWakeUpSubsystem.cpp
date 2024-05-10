#pragma once
// #pragma comment(lib, "winmm.lib")

#include "VoiceWakeUpSubsystem.h"
#include "VoiceWakeUpSettings.h"
#include "msp_errors.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <errno.h>
//#include <WinDef.h>
#include <mmsystem.h>



extern "C" {
#include "msp_cmn.h"
#include "qivw.h"
#include "msp_errors.h"
}

#define FRAME_LEN 640 //16k采样率的16bit音频，一帧的大小为640B, 时长20ms

DEFINE_LOG_CATEGORY(LogVoiceWakeUpSubsystem);

UVoiceWakeUpSubsystem::UVoiceWakeUpSubsystem()
{
	VoiceWakeUpSubsystem = this;
}

bool UVoiceWakeUpSubsystem::MsgLogin()
{
	const UVoiceWakeUpSettings* Settings = GetDefault<UVoiceWakeUpSettings>();
	FString appid = FString::Format(TEXT("appid = {0}, work_dir = ."), TArray<FStringFormatArg>({ Settings->APPID}));
	const char* params = TCHAR_TO_ANSI(*appid);
	// const char* params = "appid = 5a61a5a4, work_dir = .";
	
	int Ret = MSP_SUCCESS;
	Ret = MSPLogin(NULL, NULL, params);
	if (MSP_SUCCESS != Ret)
	{
		UE_LOG(LogVoiceWakeUpSubsystem, Error, TEXT("ThreadID:%d, %s: Voice wake up login failed!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		check(false && "Voice wake up login failed!");
		return false;
	}
	bIsLogin = true;

	UE_LOG(LogVoiceWakeUpSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice wake up login success!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	return true;
}

bool UVoiceWakeUpSubsystem::MsgLogout()
{
	while (!bIsCompleted)
	{
		UE_LOG(LogVoiceWakeUpSubsystem, Warning, TEXT("ThreadID:%d, %s: Waiting voice wake up logout!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	}
	
	int Ret = MSP_SUCCESS;
	Ret = MSPLogout();
	if (MSP_SUCCESS != Ret)
	{
		UE_LOG(LogVoiceWakeUpSubsystem, Error, TEXT("ThreadID:%d, %s: Voice wake up logout failed!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		check(false && "Voice wake up logout failed!");
		return false;
	}
	bIsLogin = false;

	UE_LOG(LogVoiceWakeUpSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice wake up logout success!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	return true;
}

bool UVoiceWakeUpSubsystem::Start()
{
	if (!bIsLogin)
	{
		UE_LOG(LogVoiceWakeUpSubsystem, Error, TEXT("ThreadID:%d, %s: Voice wake up is not login!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		check(false && "Voice wake up is not login!");
		return false;
	}

	if (!bIsCompleted || bIsWakeUpContinue)
	{
		UE_LOG(LogVoiceWakeUpSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice wake up does not allow concurrency!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return false;
	}

	bIsCompleted = false;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
	{
		this->StartInternal();
	});

	UE_LOG(LogVoiceWakeUpSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice wake up start working!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	return true;
}

void UVoiceWakeUpSubsystem::Stop()
{
	if (!bIsLogin)
	{
		UE_LOG(LogVoiceWakeUpSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice wake up is not login!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__))
	}

	UE_LOG(LogVoiceWakeUpSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice wake up stop working!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	bIsWakeUpContinue = false;
}

void UVoiceWakeUpSubsystem::StartInternal()
{
	UE_LOG(LogVoiceWakeUpSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice wake up start."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	bIsCompleted = false;
	bIsWakeUpContinue = true;

	FString AbsoluteFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir()) + "VoiceWakeUp/Source/ThirdParty/VoiceWakeUpSdk/res/wakeupresource.jet";
	AbsoluteFilePath = AbsoluteFilePath.Replace(TEXT("/"), TEXT("\\\\"));
	FString Params = "ivw_threshold=0:1450,sst=wakeup,ivw_res_path =fo|" + AbsoluteFilePath;
	
	// FString -> char* (中文)
#ifdef PLATFORM_WINDOWS
	wchar_t* c_source_wttext = TCHAR_TO_WCHAR(*Params);
	int iSize = WideCharToMultiByte(CP_ACP, 0, c_source_wttext, -1, NULL, 0, NULL, NULL); //iSize =wcslen(pwsUnicode)+1=6
	char* session_begin_params = (char*)malloc(iSize * sizeof(char)); //不需要 pszMultiByte = (char*)malloc(iSize*sizeof(char)+1);
	WideCharToMultiByte(CP_ACP, 0, c_source_wttext, -1, session_begin_params, iSize, NULL, NULL);
#else
	checkf(false, Text("Voice wake up is not in windows platform!"));
#endif

	// 硬编码wakeupresource.jet的路径
	// const char* session_begin_params = "ivw_threshold=0:1450,sst=wakeup,ivw_res_path =fo|C:\\Content_For_Programs\\wakeupresource.jet";

	const char* grammar_list = NULL;
	const char* session_id = NULL;
	int err_code = MSP_SUCCESS;
	long audio_size = 0;
	unsigned long audio_count = 0;
	int count = 0;
	int audio_stat = MSP_AUDIO_SAMPLE_CONTINUE;
	char sse_hints[128];
	DWORD bufsize;
	long len = 0;

	HWAVEIN hWaveIn; //输入设备
	WAVEFORMATEX waveform; //采集音频的格式，结构体
	WAVEHDR wHdr; //采集音频时包含数据缓存的结构体
	HANDLE wait;

	waveform.wFormatTag = WAVE_FORMAT_PCM; //声音格式为PCM
	waveform.nSamplesPerSec = 16000; //音频采样率
	waveform.wBitsPerSample = 16; //采样比特
	waveform.nChannels = 1; //采样声道数
	waveform.nAvgBytesPerSec = 16000; //每秒的数据率
	waveform.nBlockAlign = 2; //一个块的大小，采样bit的字节数乘以声道数
	waveform.cbSize = 0; //一般为0

	wait = CreateEvent(NULL, 0, 0, NULL);
	//使用waveInOpen函数开启音频采集
	waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);

	bufsize = 1024 * 500; //开辟适当大小的内存存储音频数据，可适当调整内存大小以增加录音时间，或采取其他的内存管理方案

	session_id = QIVWSessionBegin(grammar_list, session_begin_params, &err_code);
	if (err_code != MSP_SUCCESS)
	{
		UE_LOG(LogVoiceWakeUpSubsystem, Error, TEXT("ThreadID:%d, %s: QIVWSessionBegin failed! error code:%d"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), err_code);
		bIsWakeUpContinue = false;
	}

	err_code = QIVWRegisterNotify(session_id, UVoiceWakeUpSubsystem::OnWakeUpInternal, NULL);
	if (err_code != MSP_SUCCESS)
	{
		UE_LOG(LogVoiceWakeUpSubsystem, Error, TEXT("ThreadID:%d, %s: QIVWRegisterNotify failed! error code:%d"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), err_code);
		bIsWakeUpContinue = false;
	}

	pBuffer = (BYTE*)malloc(sizeof(BYTE*) * bufsize);
	wHdr.lpData = (LPSTR)pBuffer;
	wHdr.dwBufferLength = bufsize;
	wHdr.dwBytesRecorded = 0;
	wHdr.dwUser = 0;
	wHdr.dwFlags = 0;
	wHdr.dwLoops = 1;
	waveInPrepareHeader(hWaveIn, &wHdr, sizeof(WAVEHDR)); //准备一个波形数据块头用于录音
	waveInAddBuffer(hWaveIn, &wHdr, sizeof(WAVEHDR)); //指定波形数据块为录音输入缓存
	waveInStart(hWaveIn); //开始录音

	//while (audio_count< bufsize && wakeupFlage!=1)//单次唤醒
	while (bIsWakeUpContinue) //持续唤醒
	{
		Sleep(200); //等待声音录制5s

		len = 10 * FRAME_LEN; //16k音频，10帧 （时长200ms）
		audio_stat = MSP_AUDIO_SAMPLE_CONTINUE;
		if (audio_count >= wHdr.dwBytesRecorded)
		{
			len = audio_size;
			audio_stat = MSP_AUDIO_SAMPLE_LAST; //最后一块
		}
		if (0 == audio_count)
		{
			audio_stat = MSP_AUDIO_SAMPLE_FIRST;
		}

		FString SessionID = UTF8_TO_TCHAR(session_id);
		// UE_LOG(LogVoiceWakeUpSubsystem, Display, TEXT("ThreadID:%d, %s: SessionID=%s, Count=%d, audio_stat=%d\n"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *SessionID, count++, audio_stat);
		err_code = QIVWAudioWrite(session_id, (const void*)&pBuffer[audio_count], len, audio_stat);
		if (MSP_SUCCESS != err_code)
		{
			UE_LOG(LogVoiceWakeUpSubsystem, Error, TEXT("ThreadID:%d, %s: QIVWAudioWrite failed! error code:%d"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), err_code);
			bIsWakeUpContinue = false;
			break;
		}
		if (MSP_AUDIO_SAMPLE_LAST == audio_stat)
		{
			len = 0;
			audio_count = 0;
			// waveInStop(hWaveIn);
			waveInReset(hWaveIn); //停止录音
			memset(pBuffer, 0, sizeof(BYTE*) * bufsize);
			waveInPrepareHeader(hWaveIn, &wHdr, sizeof(WAVEHDR)); //准备一个波形数据块头用于录音
			waveInAddBuffer(hWaveIn, &wHdr, sizeof(WAVEHDR)); //指定波形数据块为录音输入缓存
			waveInStart(hWaveIn); //开始录音
			// break;
		}
		audio_count += len;
	}
	
	waveInReset(hWaveIn); //停止录音
	
	if (NULL != pBuffer)
	{
		free(pBuffer);
	}
	
	if (NULL != session_id)
	{
		QIVWSessionEnd(session_id, sse_hints);
	}

	// FString -> char* (中文)
#ifdef PLATFORM_WINDOWS
	if (NULL != session_begin_params)
	{
		free(session_begin_params);
	}
#else
	checkf(false, Text("Voice wake up is not in windows platform!"));
#endif

	bIsCompleted = true;
	
	AsyncTask(ENamedThreads::GameThread, [this, err_code]()
	{
		if (MSP_SUCCESS != err_code)
		{
			UE_LOG(LogVoiceWakeUpSubsystem, Error, TEXT("ThreadID:%d, %s: Voice wake up runing error, error code : %d."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), err_code);
			check(false && "Voice wake up error!");
			// if (WakeUpFailedDelegate.IsBound() && IsInGameThread())
			// {
			// 	WakeUpFailedDelegate.Broadcast(err_code);
			// }
		}
		else
		{
			if (WakeUpCompletedDelegate.IsBound() && IsInGameThread())
			{
				WakeUpCompletedDelegate.Broadcast();
			}
		}
		
		UE_LOG(LogVoiceWakeUpSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice wake up completed."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	});
}

int UVoiceWakeUpSubsystem::OnWakeUpInternal(const char* sessionID, int msg, int param1, int param2, const void* info, void* userData)
{
	checkf(VoiceWakeUpSubsystem, TEXT("Voice wake up subsystem is nullptr!"));
	VoiceWakeUpSubsystem->bIsWakeUpContinue = false;

	if (MSP_IVW_MSG_ERROR == msg) //唤醒出错消息
	{
		AsyncTask(ENamedThreads::GameThread, [msg]()
		{
			UE_LOG(LogVoiceWakeUpSubsystem, Error, TEXT("ThreadID:%d, %s: Voice wake up failed."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
			check(VoiceWakeUpSubsystem->WakeUpTriggerDelegate.IsBound() && IsInGameThread() && "Wake up failed delegate broadcast failed!");
			VoiceWakeUpSubsystem->WakeUpFailedDelegate.Broadcast(msg);
		});
	}
	else if (MSP_IVW_MSG_WAKEUP == msg) //唤醒成功消息
	{
		AsyncTask(ENamedThreads::GameThread, []()
		{
			UE_LOG(LogVoiceWakeUpSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice wake up."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
			check(VoiceWakeUpSubsystem->WakeUpTriggerDelegate.IsBound() && IsInGameThread() && "Wake up trigger delegate broadcast failed!");
			VoiceWakeUpSubsystem->WakeUpTriggerDelegate.Broadcast();
			
		});
	}
	return 0;
}
