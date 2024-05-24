// Fill out your copyright notice in the Description page of Project Settings.


#include "VoicePlayerComponent.h"

DEFINE_LOG_CATEGORY(LogVoicePlayerComponent);


// Sets default values for this component's properties
UVoicePlayerComponent::UVoicePlayerComponent() : bIsRunning(false), bIsTaskCompleted(true), CanQueueComplete(false)
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UVoicePlayerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OnAudioFinished.AddDynamic(this, &UVoicePlayerComponent::OnTaskCompleted);
}
bool UVoicePlayerComponent::PlayVoiceOnce(USoundWave* InSoundWave, FPlayVoiceOnceCompletedDelegate InPlayVoiceOnceCompletedDelegate)
{
	StopVoicePlayer();
	OnAudioFinished.RemoveDynamic(this, &UVoicePlayerComponent::OnPlayVoiceOnceCompleted);
	
	if(!InSoundWave || !InSoundWave->IsValidLowLevel() || !InSoundWave->RawPCMData || InSoundWave->RawPCMDataSize == 0 || !InSoundWave->GetDuration())
	{
		UE_LOG(LogVoicePlayerComponent, Error, TEXT("ThreadID:%d, %s: Input sound wave invalid!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return false;
	}
	PlayVoiceOnceCompletedDelegate = InPlayVoiceOnceCompletedDelegate;
	OnAudioFinished.AddDynamic(this, &UVoicePlayerComponent::OnPlayVoiceOnceCompleted);
	SetSound(InSoundWave);
	Play(0);

	return true;
}

void UVoicePlayerComponent::SetVoicePlayerVolume(float InVolume)
{
	SetVolumeMultiplier(InVolume);
}

bool UVoicePlayerComponent::StartVoicePlayer()
{
	if(bIsRunning)
	{
		UE_LOG(LogVoicePlayerComponent, Error, TEXT("ThreadID:%d, %s: Voice player is runing!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return false;
	}
	
	UE_LOG(LogVoicePlayerComponent, Warning, TEXT("ThreadID:%d, %s: Voice player run!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	if(TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	bIsRunning = true;
	bIsTaskCompleted = true;
	CanQueueComplete = false;
	VoicePlayerStartedDelegate.Broadcast();

	RunInternal(0.1);
	
	return true;
}

void UVoicePlayerComponent::StopVoicePlayer()
{
	UE_LOG(LogVoicePlayerComponent, Warning, TEXT("ThreadID:%d, %s: Voice player stop!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
	
	if(TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	Stop();
	ResetParameters();
	bIsRunning = false;
	bIsTaskCompleted = true;
	CanQueueComplete = false;
	while (!TasksQueue.IsEmpty())
	{
		FTaskWrapper TaskWrapper;
		TasksQueue.Dequeue(TaskWrapper);
		TaskWrapper.SoundWave->RemoveFromRoot();
	}
	TasksQueue.Empty();
}

bool UVoicePlayerComponent::EnqueueTaskToQueue(const FString& InSourceText, USoundWave* InSoundWave)
{
	if(InSourceText.IsEmpty() || !InSoundWave || !InSoundWave->IsValidLowLevel())
	{
		UE_LOG(LogVoicePlayerComponent, Error, TEXT("ThreadID:%d, %s: Enqueue params error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return false;;
	}

	FTaskWrapper TaskWrapper;
	TaskWrapper.SourceText = InSourceText;
	TaskWrapper.SoundWave = InSoundWave;
	TaskWrapper.SoundWave->AddToRoot();
	TasksQueue.Enqueue(TaskWrapper);

	return true;
}

void UVoicePlayerComponent::CanQueueCompletedWhileQueueEmpty()
{
	CanQueueComplete = true;
}

void UVoicePlayerComponent::OnTaskCompleted()
{
	bIsTaskCompleted = true;
}

void UVoicePlayerComponent::OnPlayVoiceOnceCompleted()
{
	PlayVoiceOnceCompletedDelegate.ExecuteIfBound();
	OnAudioFinished.RemoveDynamic(this, &UVoicePlayerComponent::OnPlayVoiceOnceCompleted);
}

void UVoicePlayerComponent::RunInternal(float QueueOperatingFrequency)
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		if(!this->bIsRunning)
		{
			UE_LOG(LogVoicePlayerComponent, Warning, TEXT("ThreadID:%d, %s: Voice player is not running!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
			this->StopVoicePlayer();
			return;
		}

		if(this->CanQueueComplete && this->TasksQueue.IsEmpty() && this->bIsTaskCompleted)
		{
			UE_LOG(LogVoicePlayerComponent, Warning, TEXT("ThreadID:%d, %s: Can voice player complete!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
			this->StopVoicePlayer();
			check(IsInGameThread() && "There is not in game thread!");
			this->VoicePlayerCompletedDelegate.Broadcast();
			return;
		}

		if(this->TasksQueue.IsEmpty() || !this->bIsTaskCompleted)
		{
			// UE_LOG(LogVoicePlayerComponent, Warning, TEXT("ThreadID:%d, %s: Voice player Waiting..."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
			return;
		}

		FTaskWrapper TaskWrapper;
		this->TasksQueue.Dequeue(TaskWrapper);
		FString TaskSourceText = TaskWrapper.SourceText;
		USoundWave* TaskSoundWave = TaskWrapper.SoundWave;
		this->TaskStartedDelegate.Broadcast(TaskSourceText, TaskSoundWave->Duration);
		this->SetSound(TaskSoundWave);
		this->Play(0);
		this->bIsTaskCompleted = false;
		TaskSoundWave->RemoveFromRoot();

		// UE_LOG(LogVoicePlayerComponent, Warning, TEXT("ThreadID:%d, %s: Voice player working... CanQueueComplete : %d, TasksQueue : %d, bIsTaskCompleted : %d!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), CanQueueComplete, TasksQueue.IsEmpty(), bIsTaskCompleted);

	}, QueueOperatingFrequency, true);
}

