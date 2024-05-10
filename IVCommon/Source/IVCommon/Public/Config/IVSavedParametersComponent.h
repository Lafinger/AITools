// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IVSavedParametersComponent.generated.h"


UCLASS( ClassGroup=(IVConfig), meta=(BlueprintSpawnableComponent) )
class IVCOMMON_API UIVSavedParametersComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UIVSavedParametersComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
public:
	//~ Begin UObject Interface.
	virtual void PostReinitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnActorEditChangeProperty(UObject* Actor, struct FPropertyChangedEvent& PropertyChangedEvent);
#endif
	//~ End UObject Interface.
	protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	//用于保存json路径
	FString FilePath;

		
};
