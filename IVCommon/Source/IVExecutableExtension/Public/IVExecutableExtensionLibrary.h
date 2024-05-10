// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IVExecutableExtensionTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#ifdef _WIN64
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows.h"
#include "stdio.h"
#include "TlHelp32.h"
#include "Misc/Paths.h"
#include "Containers/Array.h"
#include "Algo/Reverse.h"
#include "psapi.h"
#include "tchar.h"
#include "Winternl.h"
#include "iphlpapi.h"
#include "winuser.h"

#include "cstdio"
#include "iostream"
#include "memory"
#include "stdexcept"
#include "vector"
#endif

#include "IVExecutableExtensionLibrary.generated.h"


#pragma warning (disable : 4668)

/**
 * 
 */
UCLASS()
class IVEXECUTABLEEXTENSION_API UIVExecutableExtensionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/**
	 *@brief 启动进程
	 *@param Path 程序路径
	 *@param CommandLineArguments 程序启动参数
	 *@param bMinimize 是否最小化（若支持最小化）
	 *@param bInBackground 是否后台进程（若支持，将覆盖'bMinimize'）
	 *@param PriorityModifier 确定此程序的优先级（低优先级=其他优先级高的进程可以在计算机过载时限制此进程）
	 *@return 如果找不到路径，未使用Windows或执行失败，则返回false
	 */
	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = 1, PriorityModifier = Normal), Category = "iVisual|Process")
	static bool ExeExector(FString Path,FString CommandLineArguments,bool bMinimize,bool bInBackground,EPriorityModifier PriorityModifier);

	/**
	 * @brief 终止进程
	 * @param Name 程序名
	 * @param bUsePID 启用此功能后，您可以在此节点中输入 PID 而不是名称
	 * @param bForceToBeTerminated 这将强制终止进程
	 * @param bTerminateAllLaunchedChildProcesses 将终止由此程序启动的所有进程
	 * @param Filter 应用过滤器来选择一组任务。您可以使用多个过滤器或使用通配符 (*) 来指定所有任务或图像名称。 （可以在文档中找到过滤器列表）
	 * @return 如果未找到进程、未使用 Windows 或终止失败，则返回 false
	 */
	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = 1, bForceToBeTerminated = true), Category = "iVisual|Process")
	static bool ExeTerminator(FString Name, bool bUsePID, bool bForceToBeTerminated, bool bTerminateAllLaunchedChildProcesses, FString Filter);

	/**
	 *@brief 返回所有当前正在运行的进程及其每个进程信息
	 *@return 若无法返回内容或不是Windows，则不返回任何信息
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static TArray<FProcessInformations> ListProcesses(int& Length);

	/**
	 * @brief 判断程序是否运行
	 * @param Name 程序名
	 * @return 程序未运行或未使用windows则为false
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static bool IsProcessRunning(FString Name);

	/**
	 * @brief 获取一个进程的所有信息
	 * @param Name 程序名
	 * @param bUsePID 启用此功能后，您可以在此节点中输入 PID 而不是名称
	 * @param Informations 该进程信息
	 * @return 如果没有找到该进程，未使用Windows或信息收集失败，则返回false
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static bool GetProcessInformations(FString Name, bool bUsePID, FProcessInformations& Informations);

	/**
	 * @brief 对所有进程信息进行排序
	 * @param InputInformations 输入信息
	 * @param SortingType 排序类型
	 * @param bReturnReverse Reverses a range
	 * @return 返回排序后信息
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static TArray<FProcessInformations> SortProcessList(TArray<FProcessInformations> InputInformations, ESortingType SortingType, bool bReturnReverse);

	/**
	 * @brief 过滤进程信息
	 * @param InputInformations 输入信息
	 * @param FilterType 过滤类型
	 * @param RangeMin 最小
	 * @param RangeMax 最大
	 * @return 过滤后信息
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static TArray<FProcessInformations> FilterProcessList(TArray<FProcessInformations> InputInformations, EFilterType FilterType, int RangeMin, int RangeMax);

	/**
	 * @brief 获取此进程的进程标识符
	 * @return ID
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static int32 GetCurrentProcessId();

	/**
	 * @brief 获取进程的路径
	 * @param PID 该进程ID
	 * @param Path 该进程路径
	 * @return 如果没有找到该进程，未使用Windows或信息收集失败，则返回false
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static bool GetCurrentProcessPath(int32 PID, FString& Path);

	/**
	 * @brief 获取进程的路径
	 * @param PID 该进程ID
	 * @param Modules 返回的模块
	 * @return 如果没有找到该进程，未使用Windows或信息收集失败，则返回false
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static bool GetAllProcessModuleInformations(int32 PID, TArray<FModuleInformations>& Modules);

	/**
	 * @brief 返回当前运行的可执行文件的名称
	 * @return 以字符串形式返回
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static FString GetMemoryStatistics();


	/**
	 * @brief 返回进程的内存统计信息
	 * @param PID 进程ID
	 * @return 内存统计信息
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static FMemoryStats GetProcessMemoryStatistics(int32 PID);

	/**
	 * @brief 将字节转换为兆字节
	 * @param Bytes 字节
	 * @return 兆字节
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static double BytesToMegabytes(int64 Bytes);

	/**
	 * @brief Enumerates进程使用的文件
	 * @param PID  进程ID
	 * @return 返回信息
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static TArray<FFileInformations> EnumerateProcessFiles(int32 PID);

	/**
	 * @brief 获取计算机用户在前台/重点的当前进程
	 * @param PID 进程ID
	 * @return 如果没有找到该进程，未使用Windows或信息收集失败，则返回false
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static bool GetForegroundProcess(int& PID);

	/**
	 * @brief 确定进程是否在前台/重点
	 * @param PID 进程ID
	 * @return 如果没有找到该进程，未使用Windows或信息收集失败，则返回false
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static bool IsProcessInForeground(int PID);

	UFUNCTION(BlueprintCallable, Category = "iVisual|Process")
	static TArray<FWindowInformations> ListWindowInformationsOfProcess(int PID);


private:
	// Callback functions
	static BOOL CALLBACK GetEnumWindowsProc(HWND HWND, LPARAM LParam);

	static TArray<TEnumAsByte<EFileAttributeConstants>> GetFileAttributes(DWORD DWFileAttributes);
};
