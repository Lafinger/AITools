// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatModelsAsyncActionBase.h"

UChatModelsAsyncActionBase::UChatModelsAsyncActionBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer), bIsCancel(false)
{
}

void UChatModelsAsyncActionBase::Activate()
{
	Super::Activate();
}

void UChatModelsAsyncActionBase::Cancel()
{
	bIsCancel = true;
	CancelDelegate.Broadcast(FString());
}
