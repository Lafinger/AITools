#include "VoiceSynthesisSubsystem.h"
#include "IVXunFeiNodes.h"

DEFINE_LOG_CATEGORY(LogVoiceSynthesisSubsystem);

UVoiceSynthesisSubsystem::UVoiceSynthesisSubsystem() : TaskCount(0), bIsQueueWorking(false), bCanQueueCompleted(false), bIsTaskCompleted(true)
{
}

void UVoiceSynthesisSubsystem::TestFunc(const UObject* WorldContextObject, FString XX)
{
	FXunFeiTTSOptions XunFeiTTSOptions;
	
	UIVXunFeiTTSWebSocketAsyncProxy* TTSWebSocketAsync = UIVXunFeiTTSWebSocketAsyncProxy::XunFeiTextToSoundWave(WorldContextObject, XX, XunFeiTTSOptions);
	TTSWebSocketAsync->OnCloseC.BindLambda([this](USoundWave* SoundWave, const float& SoundDuration)
		{
		UE_LOG(LogVoiceSynthesisSubsystem, Warning, TEXT("ThreadID:%d, %s: !!!!!!!!!!! OnClose"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		});
}

bool UVoiceSynthesisSubsystem::EnqueueVoiceSynthesisTask(const UObject* WorldContextObject, const FString& VoiceText, const FString& SourceText, const FXunFeiTTSOptions& XunFeiTTSOptions)
{
	if (VoiceText.IsEmpty())
	{
		UE_LOG(LogVoiceSynthesisSubsystem, Error, TEXT("ThreadID:%d, %s: Voice synthesis task enqueue failed!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return false;
	}
	
	TFunction<void()> Task = [this, VoiceText, SourceText, XunFeiTTSOptions, WorldContextObject]()
	{
		UIVXunFeiTTSWebSocketAsyncProxy* TTSWebSocketAsync = UIVXunFeiTTSWebSocketAsyncProxy::XunFeiTextToSoundWave(WorldContextObject, VoiceText, XunFeiTTSOptions);
		TTSWebSocketAsync->OnCloseC.BindLambda([this, VoiceText, SourceText](USoundWave* SoundWave, const float& SoundDuration)
		{
			if(!this->bIsQueueWorking)
			{
				UE_LOG(LogVoiceSynthesisSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice synthesis is not working!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
				return;
			}
			this->bIsTaskCompleted = true;
			check(IsInGameThread() && "Is not in game thread");
			this->TaskCompletedDelegate.Broadcast(VoiceText, SourceText, FString(), SoundWave);
			
			UE_LOG(LogVoiceSynthesisSubsystem, Display, TEXT("ThreadID:%d, %s: Voice synthesis task has completed."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		});

		TTSWebSocketAsync->OnConnectionErrorC.BindLambda([this, WorldContextObject](USoundWave* SoundWave, const float& Duration)
		{
			UE_LOG(LogVoiceSynthesisSubsystem, Error, TEXT("ThreadID:%d, %s: Task working error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
			this->StopQueueWorking(WorldContextObject);
			
			check(IsInGameThread() && "Is not in game thread");
			TaskErrorDelegate.Broadcast(TEXT("Task working error!"));
		});

		UE_LOG(LogVoiceSynthesisSubsystem, Display, TEXT("ThreadID:%d, %s: Voice synthesis task working."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	};

	VoiceSynthesisTaskQueue.Enqueue(Task);
	// TaskOrderQueue.Enqueue(TaskCount);
	++TaskCount;

	UE_LOG(LogVoiceSynthesisSubsystem, Display, TEXT("ThreadID:%d, %s: Voice synthesis task enqueue success."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));

	return true;
}

bool UVoiceSynthesisSubsystem::StartQueueWorking(const UObject* WorldContextObject, bool IsConcurrent)
{
	check(!IsConcurrent); // 目前不支持并发
	// if (bIsQueueWorking || VoiceSynthesisTaskQueue.IsEmpty())
	if (bIsQueueWorking)
	{
		
		UE_LOG(LogVoiceSynthesisSubsystem, Error, TEXT("ThreadID:%d, %s: Voice synthesis task queue start working failed!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return false;
	}
	TaskCount = 0;
	bIsQueueWorking = true;
	bCanQueueCompleted = false;
	bIsTaskCompleted = true;

	UE_LOG(LogVoiceSynthesisSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice synthesis task queue start working!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	QueueWorkingStartDelegate.Broadcast();

	RunInternal(WorldContextObject, IsConcurrent, 0.1);
	
	return true;
}

void UVoiceSynthesisSubsystem::StopQueueWorking(const UObject* WorldContextObject)
{
	UE_LOG(LogVoiceSynthesisSubsystem, Warning, TEXT("ThreadID:%d, %s: Voice synthesis stop!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));

	if (QueueWorkingTimerHandle.IsValid())
	{
		WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(QueueWorkingTimerHandle);
	}
	TaskCount = 0;
	bIsQueueWorking = false;
	bCanQueueCompleted = false;
	bIsTaskCompleted = true;

	VoiceSynthesisTaskQueue.Empty();
}

void UVoiceSynthesisSubsystem::CanQueueCompletedWhileQueueEmpty()
{
	UE_LOG(LogVoiceSynthesisSubsystem, Warning, TEXT("ThreadID:%d, %s: Can queue completed while queue empty!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	bCanQueueCompleted = true;
}

void UVoiceSynthesisSubsystem::RunInternal(const UObject* WorldContextObject, bool IsConcurrent, float QueueOperatingFrequency)
{
	// 队列工作循环
	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(QueueWorkingTimerHandle, [this, IsConcurrent, WorldContextObject]()
	{
		if (!this->bIsQueueWorking)
		{
			this->StopQueueWorking(WorldContextObject);
			return;
		}
		
		if (this->bCanQueueCompleted && this->VoiceSynthesisTaskQueue.IsEmpty() && this->bIsTaskCompleted)
		{
			UE_LOG(LogVoiceSynthesisSubsystem, Warning, TEXT("ThreadID:%d, %s: Can voice synthesis complete!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
			this->StopQueueWorking(WorldContextObject);
			this->QueueWorkingCompletedDelegate.Broadcast();
			return;
		}
		
		if (this->VoiceSynthesisTaskQueue.IsEmpty())
		{
			return;
		}
		
		// UE_LOG(LogVoiceSynthesisSubsystem, Display, TEXT("ThreadID:%d, %s: Voice synthesis task working."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		if (IsConcurrent) // 并发
		{
			// TFunction<void()> Task;
			// this->VoiceSynthesisTaskQueue.Dequeue(Task);
			// Task();
		}
		else // 阻塞
		{
			if (!this->bIsTaskCompleted)
			{
				return;
			}
			TFunction<void()> Task;
			this->VoiceSynthesisTaskQueue.Dequeue(Task);
			this->bIsTaskCompleted = false;
			Task();
			// UE_LOG(LogVoiceSynthesisSubsystem, Display, TEXT("ThreadID:%d, %s: Voice synthesis do task."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		}
		
	}, QueueOperatingFrequency, true);
}
