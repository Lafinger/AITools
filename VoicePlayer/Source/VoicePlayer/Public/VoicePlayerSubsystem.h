// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VoicePlayerSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTaskCompletedDelegate);

/**
 * 语音播放子系统，目前没有作用，语音播放的具体实现在VoicePlayerComponent
 */
UCLASS()
class VOICEPLAYER_API UVoicePlayerSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "VoicePlayer")
	FTaskCompletedDelegate TaskCompletedDelegate;
	
	UVoicePlayerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "VoicePlayer", meta = (WorldContext = "WorldContextObject"))
	void PlaySound(const UObject* WorldContextObject, USoundBase* Sound);

	UFUNCTION(BlueprintCallable, Category = "VoicePlayer")
	void Stop();
};
