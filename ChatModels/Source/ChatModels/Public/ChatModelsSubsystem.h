// Copyright 2023 AIRT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "ChatModelsSettings.h"
#include "ChatModelsSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogChatModelsSubsystem, Log, All);

/**
 * 上下文关联历史记录
 * 
 */
USTRUCT(BlueprintType)
struct CHATMODELS_API FContextHistory
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Chat Models")
	FString Role;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Chat Models")
	FString Context;
};

/**
 * 大模型管理子系统
 * 
 */
UCLASS()
class CHATMODELS_API UChatModelsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
public:
	void AddHistory(const FContextHistory& InHistory);
	
	UFUNCTION(BlueprintCallable, Category = "Chat Models")
	void ClearHistories();

	UFUNCTION(BlueprintCallable, Category = "Chat Models")
	bool CheckDifyModelType(EDifyModelType ModelType);

	UPROPERTY(BlueprintReadOnly, Category = "Chat Models")
	TArray<FContextHistory> ContextHistories;

	UFUNCTION(BlueprintCallable, Category = "Chat Models")
	FString RemoveStringLineBreak(const FString& String);

	UFUNCTION(BlueprintCallable, Category = "Chat Models")
	TArray<FString> GetSpecialSplitStrings(const FString& LongString, const FString& Delimiter, int32 NoLessThan);
};
