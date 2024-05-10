// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ChatModelsAsyncActionBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChatModelsAsyncActionBaseDelegate, FString, Message);

UCLASS(Abstract)
class CHATMODELS_API UChatModelsAsyncActionBase  : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	// 每次的响应
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Response"))
	FChatModelsAsyncActionBaseDelegate ResponseDelegate;
	
	// 完成
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Completed"))
	FChatModelsAsyncActionBaseDelegate CompletedDelegate;
	
	// 取消
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Cancel"))
	FChatModelsAsyncActionBaseDelegate CancelDelegate;

	// 失败
	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Error"))
	FChatModelsAsyncActionBaseDelegate ErrorDelegate;
	
	UChatModelsAsyncActionBase(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	virtual void Cancel();

protected:

	// 是否取消
	bool bIsCancel;
};

