// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "IVDeveloperSettings.generated.h"

/**
 * 用于参数项目配置，若要保存为Json，请标记宏EditorConfig
 */
UCLASS(Abstract,DefaultConfig)
class IVCOMMON_API UIVDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UIVDeveloperSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	// UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;

public:
	//Called after the C++ constructor
	virtual void PostInitProperties() override;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	FString FilePath;
};
