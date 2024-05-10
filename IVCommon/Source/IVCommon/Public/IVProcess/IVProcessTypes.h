// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IVProcessTypes.generated.h"



/**创建进行所需参数*/
USTRUCT(BlueprintType)
struct FIVProcInfo
{
	GENERATED_BODY()

	/**是否自启动*/
	UPROPERTY(BlueprintReadOnly, Config,EditAnywhere, Category = "IVCmmon|Process")
	bool bActive;

	/**程序路径*/
	UPROPERTY(BlueprintReadOnly, Config,EditAnywhere, Category = "IVCmmon|Process")
	FString Path;

	/**程序名*/
	UPROPERTY(BlueprintReadOnly, Config,EditAnywhere, Category = "IVCmmon|Process")
	FString ExeName;

	/**程序启动参数*/
	UPROPERTY(BlueprintReadOnly, Config,EditAnywhere, Category = "IVCmmon|Process")
	FString CommandLineArguments;
	
	/**是否隐藏界面*/
	UPROPERTY(BlueprintReadOnly, Config,EditAnywhere, Category = "IVCmmon|Process")
	bool bWindowHidden;

	/**是否抓取信息*/
	UPROPERTY(BlueprintReadOnly, Config,EditAnywhere, Category = "IVCmmon|Process")
	bool bLogCatch;
	
	FIVProcInfo()
		:bActive(false)
		,Path(TEXT("LocalExe://"))
		,ExeName(FString())
		,CommandLineArguments(FString())
		,bWindowHidden(true)
		,bLogCatch(false)
	{
	}
};