#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Public/Subsystems/EngineSubsystem.h"
#include "VoiceSynthesisSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVoiceSynthesisSubsystem, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTaskDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FTaskCompletedDelegate, const FString&, VoiceText, const FString&, SourceText, const FString&, FileAbsolutePath, USoundWave*, SoundWave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTaskErrorDelegate, const FString&, ErrorMessage);

/**
 * 语音合成子系统，使用了阻塞式队列（一个一个语音任务合成，队列起缓冲作用），目前不支持并发队列
 */
UCLASS()
class VOICESYNTHESIS_API UVoiceSynthesisSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	/** 语音合成队列工作开始的委托 */
	UPROPERTY(BlueprintAssignable, Category = "VoiceSynthesis")
	FTaskDelegate QueueWorkingStartDelegate;

	/** 语音合成队列工作结束的委托 */
	UPROPERTY(BlueprintAssignable, Category = "VoiceSynthesis")
	FTaskDelegate QueueWorkingCompletedDelegate;

	/** 语音合成队列中一个任务完成的委托 */
	UPROPERTY(BlueprintAssignable, Category = "VoiceSynthesis")
	FTaskCompletedDelegate TaskCompletedDelegate;

	/** 语音合成队列中一个任务出错的委托 */
	UPROPERTY(BlueprintAssignable, Category = "VoiceSynthesis")
	FTaskErrorDelegate TaskErrorDelegate;
	
	UVoiceSynthesisSubsystem();

	UFUNCTION(BlueprintCallable, Category = "VoiceSynthesis", meta = (WorldContext = "WorldContextObject"))
	void TestFunc(const UObject* WorldContextObject, FString XX);

	/**
	* 语音合成任务入队操作
	* @param VoiceText 需要合成音频的文本。
	* @param SourceText 原来的文本（注意：与VoiceText区别在于SourceText用于UI展示）。
	* @param XunFeiTTSOptions 科大讯飞语音合成功能需要的参数。TODO::后面需要制作成通配符的形式
	*/
	UFUNCTION(BlueprintCallable, Category = "VoiceSynthesis", meta = (WorldContext = "WorldContextObject"))
	bool EnqueueVoiceSynthesisTask(const UObject* WorldContextObject, const FString& VoiceText, const FString& SourceText, const FXunFeiTTSOptions& XunFeiTTSOptions);

	/**
	* 开始语音合成队列工作，目前不支持并发
	* @param IsConcurrent 是并发队列还是阻塞队列。
	*/
	UFUNCTION(BlueprintCallable, Category = "VoiceSynthesis", meta = (WorldContext = "WorldContextObject"))
	bool StartQueueWorking(const UObject* WorldContextObject, bool IsConcurrent = false);
	
	/** 停止语音合成队列工作 */
	UFUNCTION(BlueprintCallable, Category = "VoiceSynthesis", meta = (WorldContext = "WorldContextObject"))
	void StopQueueWorking(const UObject* WorldContextObject);
	
	/** 当队列为空时，可以结束语音合成队列工作 */
	UFUNCTION(BlueprintCallable, Category = "VoiceSynthesis")
	void CanQueueCompletedWhileQueueEmpty();
	
private:
	/**
	* 队列工作实现
	* @param IsConcurrent 是并发队列还是阻塞队列。
	* @param QueueOperatingFrequency 以秒为单位的队列工作频率。
	*/
	void RunInternal(const UObject* WorldContextObject, bool IsConcurrent, float QueueOperatingFrequency = 0.1);
	
	uint32 TaskCount;
	bool bIsQueueWorking;
	bool bCanQueueCompleted;
	FTimerHandle QueueWorkingTimerHandle;
	TQueue<TFunction<void()>> VoiceSynthesisTaskQueue;

	// 阻塞模式时候使用，目前不支持
	bool bIsTaskCompleted;
	
	// // 并发模式的时候使用，目前不支持
	// TQueue<uint32> TaskOrderQueue;
};
