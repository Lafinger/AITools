// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ComfyUIFunctionLibrary.generated.h"

UCLASS()
class STABLEDIFFUSIONSERVICES_API UComfyUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// UFUNCTION(BlueprintCallable, Category = "ComfyUI")
	// static void InsertPromptToWorkflowAPI(const FString& InPrompt);
};
