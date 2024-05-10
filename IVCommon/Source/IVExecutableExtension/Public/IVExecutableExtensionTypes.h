// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IVExecutableExtensionTypes.generated.h"



UENUM(BlueprintType)
enum EPriorityModifier
{
	PriorityModifier_Idle     UMETA(DisplayName = "Idle"),
	PriorityModifier_Low      UMETA(DisplayName = "Low"),
	PriorityModifier_Normal   UMETA(DisplayName = "Normal"),
	PriorityModifier_High     UMETA(DisplayName = "High"),
	PriorityModifier_Higher   UMETA(DisplayName = "Higher"),
};

UENUM(BlueprintType)
enum ESortingType
{
	SortingType_Alphabetically	UMETA(DisplayName = "Alphabetically"),
	SortingType_Threads			UMETA(DisplayName = "Number of Threads"),
	SortingType_PID				UMETA(DisplayName = "ProcessID", Tooltip = "Process Identifier"),
	SortingType_PPID			UMETA(DisplayName = "Parent ProcessID", Tooltip = "Parent Process Identifier"),
	SortingType_PIV				UMETA(DisplayName = "Pri Class Base", Tooltip = "The base priority of any threads created by this process"),
};

UENUM(BlueprintType)
enum EFilterType
{
	FilterType_Duplicates	UMETA(DisplayName = "Remove duplicates (Name)"),
	FilterType_Threads		UMETA(DisplayName = "Remove items whose number of threads is not in range"),
	FilterType_PID			UMETA(DisplayName = "Remove items whose process identifier is not in range", Tooltip = "Process Identifier"),
	FilterType_PPID			UMETA(DisplayName = "Remove items whose parent process identifier is not in range", Tooltip = "Parent Process Identifier"),
	FilterType_PIV			UMETA(DisplayName = "Remove items whose pri class base is not in range", Tooltip = "The base priority of any threads created by this process"),
};

UENUM(BlueprintType)
enum EFileAttributeConstants
{
	FileAttributeConstants_ReadOnly				UMETA(DisplayName = "Read Only"),
	FileAttributeConstants_Hidden				UMETA(DisplayName = "Hidden"),
	FileAttributeConstants_System				UMETA(DisplayName = "System"),
	FileAttributeConstants_Directory			UMETA(DisplayName = "Directory"),
	FileAttributeConstants_Archive				UMETA(DisplayName = "Archive"),
	FileAttributeConstants_Device				UMETA(DisplayName = "Device"),
	FileAttributeConstants_Normal				UMETA(DisplayName = "Normal"),
	FileAttributeConstants_Temporary			UMETA(DisplayName = "Temporary"),
	FileAttributeConstants_SparseFile			UMETA(DisplayName = "Sparse File"),
	FileAttributeConstants_ReparsePoint			UMETA(DisplayName = "Reparse Point"),
	FileAttributeConstants_Compressed			UMETA(DisplayName = "Compressed"),
	FileAttributeConstants_Offline				UMETA(DisplayName = "Offline"),
	FileAttributeConstants_NotContentIndexed	UMETA(DisplayName = "Not Content Indexed"),
	FileAttributeConstants_Encrypted			UMETA(DisplayName = "Encrypted"),
	FileAttributeConstants_IntegrityStream		UMETA(DisplayName = "Integrity Stream"),
	FileAttributeConstants_Virtual				UMETA(DisplayName = "Virtual"),
	FileAttributeConstants_NoScrubData			UMETA(DisplayName = "No Scrub Data"),
	FileAttributeConstants_EA					UMETA(DisplayName = "EA"),
	FileAttributeConstants_Pinned				UMETA(DisplayName = "Pinned"),
	FileAttributeConstants_Unpinned				UMETA(DisplayName = "Unpinned"),
	FileAttributeConstants_RecallOnOpen			UMETA(DisplayName = "Recall On Open"),
	FileAttributeConstants_RecallOnDataAccess	UMETA(DisplayName = "Recall On Data Access"),
};

USTRUCT(BlueprintType)
struct FProcessInformations
{
	GENERATED_BODY()

public:

	/**该进程名字*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The process name"))
	FString Name;

	/**进程启动的执行线程数*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The number of execution threads started by the process"))
	int32 Threads;

	/**进程标识符*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "Process Identifier"))
	int32 ProcessID;

	/**Parent Process Identifier - 创建此进程的进程的标识符*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "Parent Process Identifier - The identifier of the process that created this process"))
	int32 ParentProcessID;

	/**此进程创建的任何线程的基本优先级*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The base priority of any threads created by this process"))
	int32 PIV;

	FProcessInformations()
		:Name(FString())
		,Threads(0)
		,ProcessID(0)
		,ParentProcessID(0)
		,PIV(0)
	{
		
	}
};

USTRUCT(BlueprintType)
struct FModuleInformations
{
	GENERATED_BODY()
	
public:

	/**模块名称*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The module name"))
	FString Name;

	/**模块路径*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The module path"))
	FString Path;

	/**模块大小（bytes）*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The size of the module, in bytes"))
	int32 Size;

	FModuleInformations()
		:Name(FString())
		,Path(FString())
		,Size(0)
	{
		
	}
};

USTRUCT(BlueprintType)
struct FMemoryStats
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	int64 PageFaultCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The peak working set size, in bytes"))
	int64 PeakWorkingSetSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The current working set size, in bytes"))
	int64 WorkingSetSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The peak paged pool usage, in bytes"))
	int64 QuotaPeakPagedPoolUsage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The current paged pool usage, in bytes"))
	int64 QuotaPagedPoolUsage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The peak nonpaged pool usage, in bytes"))
	int64 QuotaPeakNonPagedPoolUsage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The current nonpaged pool usage, in bytes"))
	int64 QuotaNonPagedPoolUsage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The Commit Charge value in bytes for this process. Commit Charge is the total amount of memory that the memory manager has committed for a running process"))
	int64 PagefileUsage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The peak value in bytes of the Commit Charge during the lifetime of this process"))
	int64 PeakPagefileUsage;

	FMemoryStats()
		:PageFaultCount(0)
		,PeakWorkingSetSize(0)
		,WorkingSetSize(0)
		,QuotaPeakPagedPoolUsage(0)
		,QuotaPagedPoolUsage(0)
		,QuotaPeakNonPagedPoolUsage(0)
		,QuotaNonPagedPoolUsage(0)
		,PagefileUsage(0)
		,PeakPagefileUsage(0)
	{
	}
};

USTRUCT(BlueprintType)
struct FWindowInformations
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The name of the window"))
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "The ID of the window"))
	int64 WindowID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	bool Visible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process", meta = (Tooltip = "When testing in the editor and run on begin play there might me multiple focused windows"))
	bool IsFocused;

	FWindowInformations()
	:Name(FString())
	,WindowID(0)
	,Visible(true)
	,IsFocused(false)
	{
		
	}
};

USTRUCT(BlueprintType)
struct FFileInformations
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	FString AlternativeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	TArray<TEnumAsByte<EFileAttributeConstants>> FileAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	FDateTime CreationTimeHigh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	FDateTime CreationTimeLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	FDateTime LastAccessTimeHigh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	FDateTime LastAccessTimeLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	FDateTime LastWriteTimeHigh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	FDateTime LastWriteTimeLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	int32 FileSizeHigh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	int32 FileSizeLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	int32 Reserved0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "iVisual|Process")
	int32 Reserved1;

	FFileInformations()
		:Name(FString())
		,AlternativeName(FString())
		,FileAttributes()
		,CreationTimeHigh(FDateTime())
		,CreationTimeLow(FDateTime())
		,LastAccessTimeHigh(FDateTime())
		,LastAccessTimeLow(FDateTime())
		,LastWriteTimeHigh(FDateTime())
		,LastWriteTimeLow(FDateTime())
		,FileSizeHigh(0)
		,FileSizeLow(0)
		,Reserved0(0)
		,Reserved1(0)
	{
		
	}
};

