#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoiceWakeUpSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVoiceWakeUpSubsystem, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWakeUpDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWakeUpFailedDelegate, int, ErrorCode);

/** 同一时刻只能开启一个语音唤醒，不允许并发 */
UCLASS()
class VOICEWAKEUP_API UVoiceWakeUpSubsystem :public UGameInstanceSubsystem
{

	GENERATED_BODY()

public:
	/** 语音唤醒触发委托 */
	UPROPERTY(BlueprintAssignable, Category = "VoiceWakeUp")
	FWakeUpDelegate WakeUpTriggerDelegate;
	
	/** 语音唤醒完成委托 */
	UPROPERTY(BlueprintAssignable, Category = "VoiceWakeUp")
	FWakeUpDelegate WakeUpCompletedDelegate;
	
	/** 语音唤醒出错委托 */
	UPROPERTY(BlueprintAssignable, Category = "VoiceWakeUp")
	FWakeUpFailedDelegate WakeUpFailedDelegate;
	
	UVoiceWakeUpSubsystem();

	/** 登录科大讯飞，若登录失败直接check */
	UFUNCTION(BlueprintCallable, Category = "VoiceWakeUp")
	bool MsgLogin();

	/** 登出科大讯飞（注意：登出时会阻塞GameThread，直到语音唤醒完成了） */
	UFUNCTION(BlueprintCallable, Category = "VoiceWakeUp")
	bool MsgLogout();

	/** 开始语音唤醒监听，运行在后台线程（注意：不保证执行，因为不支持并发） */
	UFUNCTION(BlueprintCallable, Category = "VoiceWakeUp")
	bool Start();

	/** 停止语音唤醒监听 */
	UFUNCTION(BlueprintCallable, Category = "VoiceWakeUp")
	void Stop();

private:
	void StartInternal();
	static int OnWakeUpInternal(const char* sessionID, int msg, int param1, int param2, const void* info, void* userData);
	
	bool bIsLogin = false;

	/** 语音唤醒是否完成，为了确保同一时刻只能开启一个语音唤醒，不允许并发 */
	bool bIsCompleted = true;
	bool bIsWakeUpContinue = false;

	/** 采集音频时的数据缓存 */
	BYTE* pBuffer = NULL;
};

/** 单例指针，方便第三方库全局函数引用 */
static UVoiceWakeUpSubsystem* VoiceWakeUpSubsystem;