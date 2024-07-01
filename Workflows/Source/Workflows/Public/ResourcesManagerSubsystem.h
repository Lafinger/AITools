// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ResourcesManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogResourcesManagerSubsystem, Log, All);

USTRUCT(Blueprintable)
struct FTTSParameters

{
	GENERATED_BODY()

	/** 说话人音色 */
	FString Vcn;

	/** 说话人语速 */
	int32 Speed;

	/** 说话人音量 */
	int32 Volume;

	/** 说话人音调 */
	int32 Pitch;
	
};

/**
 * 
 */
UCLASS()
class WORKFLOWS_API UResourcesManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "AITools|Workflows", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	void UpdateQAExcelBookManually(const FString& InFileName, const FString& InExcelBookTitle, UPARAM(ref) URunTimeTable* InRunTimeTable);

	// UFUNCTION(BlueprintCallable, Category = "Workflows", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	// void UpdateLocalResourcesAutomatically(const UObject* WorldContextObject, const TArray<FString>& FilePath);


	UFUNCTION(BlueprintCallable, Category = "AITools|Workflows", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	bool UpdateOrAddLocalAudioFile_QA(const FString& InVoiceText, const FTTSParameters& InTTSParameters, const FString& InFilePath);
};
