// Fill out your copyright notice in the Description page of Project Settings.


#include "StableDiffusionHTTPAsyncActionBase.h"

DEFINE_LOG_CATEGORY(StableDiffusionHTTPAsyncAction);

UStableDiffusionHTTPAsyncActionBase::UStableDiffusionHTTPAsyncActionBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer), bIsCancel(false)
{
	AddToRoot();
}

void UStableDiffusionHTTPAsyncActionBase::SetReadyToDestroy()
{
	RemoveFromRoot();
	
	Super::SetReadyToDestroy();
}

void UStableDiffusionHTTPAsyncActionBase::Cancel()
{
	bIsCancel = true;
	CancelDelegate.Broadcast(FString(), nullptr);

	SetReadyToDestroy();
}
