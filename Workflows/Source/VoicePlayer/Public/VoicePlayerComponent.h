// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "VoicePlayerComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVoicePlayerComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVoicePlayerStartedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVoicePlayerCompletedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTaskStartedDelegate, const FString&, SourceText, float, Duration);

DECLARE_DYNAMIC_DELEGATE(FPlayVoiceOnceCompletedDelegate);

/** 语音播放任务格式 */
struct FTaskWrapper
{
	FString SourceText;
	USoundWave* SoundWave;
};

/**
 * 语音播放组件，使用了阻塞式队列（一个一个语音任务播放，队列起缓冲作用）
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VOICEPLAYER_API UVoicePlayerComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	/** 语音播放队列工作开始的委托 */
	UPROPERTY(BlueprintAssignable, Category = "VoicePlayer")
	FVoicePlayerStartedDelegate VoicePlayerStartedDelegate;

	/** 语音播放队列工作结束的委托 */
	UPROPERTY(BlueprintAssignable, Category = "VoicePlayer")
	FVoicePlayerCompletedDelegate VoicePlayerCompletedDelegate;

	/** 语音播放队列中一个任务开始的委托 */
	UPROPERTY(BlueprintAssignable, Category = "VoicePlayer")
	FTaskStartedDelegate TaskStartedDelegate;
	
	// Sets default values for this component's properties
	UVoicePlayerComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/**
	* 只播放一个音频
	* @param InSoundWave 播放的音频。
	* @param InPlayVoiceOnceCompletedDelegate 播放完成的委托。
	*/
	UFUNCTION(BlueprintCallable, Category = "VoicePlayer")
	bool PlayVoiceOnce(USoundWave* InSoundWave, FPlayVoiceOnceCompletedDelegate InPlayVoiceOnceCompletedDelegate);

	/**
	* 设置播放音频的音量大小
	* @param InVolume 需要设置音量大小。
	*/
	UFUNCTION(BlueprintCallable, Category = "VoicePlayer")
	void SetVoicePlayerVolume(float InVolume);

	/** 开始语音播放队列工作 */
	UFUNCTION(BlueprintCallable, Category = "VoicePlayer")
	bool StartVoicePlayer();

	/** 停止语音播放队列工作 */
	UFUNCTION(BlueprintCallable, Category = "VoicePlayer")
	void StopVoicePlayer();

	/**
	* 语音播放任务入队操作
	* @param InSourceText 需要播放音频的文本，通常用于UI展示。
	* @param InSoundWave 需要播放的音频。
	*/
	UFUNCTION(BlueprintCallable, Category = "VoicePlayer")
	bool EnqueueTaskToQueue(const FString& InSourceText, USoundWave* InSoundWave);

	/** 当队列为空时，可以结束语音合成队列工作 */
	UFUNCTION(BlueprintCallable, Category = "VoicePlayer")
	void CanQueueCompletedWhileQueueEmpty();

	/** 用于语音播放队列，每一个语音播放完成会触发的函数 */
	UFUNCTION()
	void OnTaskCompleted();

	/** 用于只播放一个音频，播放一个语音播放完成会触发的函数 */
	UFUNCTION()
	void OnPlayVoiceOnceCompleted();
	
	FPlayVoiceOnceCompletedDelegate PlayVoiceOnceCompletedDelegate;
	
private:
	/**
	* 队列工作实现
	* @param QueueOperatingFrequency 以秒为单位的队列工作频率。
	*/
	void RunInternal(float QueueOperatingFrequency = 0.1);

	FTimerHandle TimerHandle;
	bool bIsRunning;
	bool bIsTaskCompleted;
	bool CanQueueComplete;
	TQueue<FTaskWrapper> TasksQueue;
};
