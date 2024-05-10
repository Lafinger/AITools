// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IVProcessTypes.h"
#include "Config/IVDeveloperSettings.h"
#include "IVExecutableExtensionSettings.generated.h"




/**
 *用于启动进程和第三方程序配置
 */
UCLASS(Config = "iVisual")
class IVCOMMON_API UIVExecutableExtensionSettings : public UIVDeveloperSettings
{
	GENERATED_BODY()
public:
	UIVExecutableExtensionSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
public:
	UFUNCTION(BlueprintPure, Category = "iVisual|Process",DisplayName = "GetExecutableExtensionSettings")
	static UIVExecutableExtensionSettings* Get();

public:
	UPROPERTY(BlueprintReadOnly, Config,EditAnywhere, Category = "iVisual|Process")
	TArray<FIVProcInfo> ProcessList;

public:
	//Called after the C++ constructor
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

private:
	bool bCreatProcess;
};
