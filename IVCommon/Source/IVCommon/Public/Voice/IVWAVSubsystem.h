// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "IVWAVSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class IVCOMMON_API UIVWAVSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "IVCommon | Voice")
	void SetAudioComponent(class UAudioComponent* AudioComponent);
	
	UFUNCTION(BlueprintCallable, Category = "IVCommon | Voice")
	void InsertSoundWaveQueue( class USoundWave* const SoundWave) ;

	UFUNCTION(BlueprintCallable, Category = "IVCommon | Voice")
	void InitializeQueueExecution() ;

	UFUNCTION(BlueprintCallable, Category = "IVCommon | Voice")
	void StopSoundWaveQueueExecution();
	
private:
	bool IsValidAudioComponent();
	void PlayAudioFinish(UAudioComponent* InAudioComponent);
private:
	 TArray<USoundWave*> SoundWaveArray;
	
	 UAudioComponent* AudioComponent=nullptr;
};
