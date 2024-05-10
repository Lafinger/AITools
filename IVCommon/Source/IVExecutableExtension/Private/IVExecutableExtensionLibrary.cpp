// Copyright 2023 AIRT,  All Rights Reserved.


#include "IVExecutableExtensionLibrary.h"

#include "IVExecutableExtension.h"
#include "IVExecutableExtensionTypes.h"
#include "iVisualFileHeplerBPLibrary.h"
#include "Kismet/KismetStringLibrary.h"


bool UIVExecutableExtensionLibrary::ExeExector(FString Path, FString CommandLineArguments, bool bMinimize,
                                               bool bInBackground, EPriorityModifier PriorityModifier)
{
#ifdef _WIN64
	//注意：此操作只针对放入config.json插件对应文件夹有用
	const FString PathPrefix="LocalExe://";
	if((UKismetStringLibrary::StartsWith(Path,PathPrefix,ESearchCase::IgnoreCase)))
	{
		FString File=UKismetStringLibrary::RightChop(Path,PathPrefix.Len());
		Path=FPaths::Combine(UiVisualFileHeplerBPLibrary::GetExtrasExeDir(TEXT("IVCommon"))/File);
		FPaths::NormalizeFilename(Path);
	}

	if (FPaths::FileExists(Path) && Path.Contains(".exe"))
	{
		int LocalInt = (int)PriorityModifier - 2;
		FPlatformProcess::CreateProc(*Path,
			nullptr,
			false,
			bMinimize,
			bInBackground,
			nullptr,
			LocalInt,
			nullptr,
			nullptr);
		
		return true;
	}
#endif

	return false;
}


bool UIVExecutableExtensionLibrary::ExeTerminator(FString Name, bool bUsePID, bool bForceToBeTerminated,
                                                  bool bTerminateAllLaunchedChildProcesses, FString Filter)
{
#ifdef _WIN64
	bool LocalBool = false;
	HANDLE LocalHANDLE = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (LocalHANDLE != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 LocalPROCESSENTRY32;
		LocalPROCESSENTRY32.dwSize = sizeof(LocalPROCESSENTRY32);
		if (::Process32First(LocalHANDLE, &LocalPROCESSENTRY32))
		{
			do {
				if (bUsePID)
					{
					if (LocalPROCESSENTRY32.th32ProcessID == FCString::Atoi(*Name))
					{
						LocalBool = true;
					}
				}
				else
					{
					if (LocalPROCESSENTRY32.szExeFile == Name)
					{
						LocalBool = true;
					}
				}
			} while (::Process32Next(LocalHANDLE, &LocalPROCESSENTRY32));
		}
	}
	::CloseHandle(LocalHANDLE);
	if (LocalBool)
		{
		FString LocalString = ("taskkill");
		if (bForceToBeTerminated)
		{
			LocalString += (" /f");
		}
			
		if (bUsePID)
		{
			LocalString += (" /pid " + Name);
		}
		else
		{
			LocalString += (" /im " + Name);
		}
		if (bTerminateAllLaunchedChildProcesses)
		{
			LocalString += (" /t");
		}
			
		if (Filter.Len() > 0)
		{
			LocalString += (" /fi \"" + Filter + "\"");
		}
			
		char* LocalChar = TCHAR_TO_ANSI(*LocalString);
		system(LocalChar);
		return true;
	}
#endif


	return false;
}

TArray<FProcessInformations> UIVExecutableExtensionLibrary::ListProcesses(int& Length)
{
	TArray<FProcessInformations> LocalProcessInformationsArray;
	
#ifdef _WIN64
	FProcessInformations LocalProcessInformations;
	
	HANDLE LocalHANDLE = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (LocalHANDLE != INVALID_HANDLE_VALUE)
		{
		PROCESSENTRY32 LocalPROCESSENTRY32;
		LocalPROCESSENTRY32.dwSize = sizeof(LocalPROCESSENTRY32);
		if (::Process32First(LocalHANDLE, &LocalPROCESSENTRY32))
			{
			do
			{
				LocalProcessInformations.Name = LocalPROCESSENTRY32.szExeFile;
				LocalProcessInformations.Threads = LocalPROCESSENTRY32.cntThreads;
				LocalProcessInformations.ProcessID = LocalPROCESSENTRY32.th32ProcessID;
				LocalProcessInformations.ParentProcessID = LocalPROCESSENTRY32.th32ParentProcessID;
				LocalProcessInformations.PIV = LocalPROCESSENTRY32.pcPriClassBase;
				LocalProcessInformationsArray.Add(LocalProcessInformations);
				
			} while (::Process32Next(LocalHANDLE, &LocalPROCESSENTRY32));
		}
	}
	::CloseHandle(LocalHANDLE);
	
	Length = LocalProcessInformationsArray.Num();
	
#endif
	return LocalProcessInformationsArray;
}

bool UIVExecutableExtensionLibrary::IsProcessRunning(FString Name)
{
	bool LocalBool = false;
#ifdef _WIN64
	HANDLE LocalHANDLE = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (LocalHANDLE != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 LocalPROCESSENTRY32;
		LocalPROCESSENTRY32.dwSize = sizeof(LocalPROCESSENTRY32);
		if (::Process32First(LocalHANDLE, &LocalPROCESSENTRY32))
			{
			do {
				if (LocalPROCESSENTRY32.szExeFile == Name)
				{
					LocalBool = true;
				}
			} while (::Process32Next(LocalHANDLE, &LocalPROCESSENTRY32));
		}

	}
	::CloseHandle(LocalHANDLE);
#endif
	return LocalBool;
}

bool UIVExecutableExtensionLibrary::GetProcessInformations(FString Name, bool bUsePID,
	FProcessInformations& Informations)
{
	bool LocalBool = false;
#ifdef _WIN64
	HANDLE LocalHANDLE = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (LocalHANDLE != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 LocalPROCESSENTRY32;
		LocalPROCESSENTRY32.dwSize = sizeof(LocalPROCESSENTRY32);
		if (::Process32First(LocalHANDLE, &LocalPROCESSENTRY32))
		{
			do {
				if (bUsePID)
				{
					if (LocalPROCESSENTRY32.th32ProcessID == FCString::Atoi(*Name))
					{
						Informations.Name = LocalPROCESSENTRY32.szExeFile;
						Informations.Threads = LocalPROCESSENTRY32.cntThreads;
						Informations.ProcessID = LocalPROCESSENTRY32.th32ProcessID;
						Informations.ParentProcessID = LocalPROCESSENTRY32.th32ParentProcessID;
						Informations.PIV = LocalPROCESSENTRY32.pcPriClassBase;
						LocalBool = true;
					}
				}
				else
				{
					if (LocalPROCESSENTRY32.szExeFile == Name)
					{
						Informations.Name = LocalPROCESSENTRY32.szExeFile;
						Informations.Threads = LocalPROCESSENTRY32.cntThreads;
						Informations.ProcessID = LocalPROCESSENTRY32.th32ProcessID;
						Informations.ParentProcessID = LocalPROCESSENTRY32.th32ParentProcessID;
						Informations.PIV = LocalPROCESSENTRY32.pcPriClassBase;
						LocalBool = true;
					}
				}
			} while (::Process32Next(LocalHANDLE, &LocalPROCESSENTRY32));
		}

	}
	::CloseHandle(LocalHANDLE);
#endif
	return LocalBool;
}

TArray<FProcessInformations> UIVExecutableExtensionLibrary::SortProcessList(
	TArray<FProcessInformations> InputInformations, ESortingType SortingType, bool ReturnReverse)
{
	TArray<FProcessInformations> LocalProcessInformationsArray;
#ifdef _WIN64
	TArray<FString> CopiedLocalName;
	TArray<int> CopiedLocalThreads;
	TArray<int> CopiedLocalPID;
	TArray<int> CopiedLocalPPID;
	TArray<int> CopiedLocalPIV;
	TArray<FString> LocalName;
	TArray<int> LocalThreads;
	TArray<int> LocalPID;
	TArray<int> LocalPPID;
	TArray<int> LocalPIV;
	
	for (int i = 0; i < InputInformations.Num(); i++)
		{
		CopiedLocalName.Add(InputInformations[i].Name);
		CopiedLocalThreads.Add(InputInformations[i].Threads);
		CopiedLocalPID.Add(InputInformations[i].ProcessID);
		CopiedLocalPPID.Add(InputInformations[i].ParentProcessID);
		CopiedLocalPIV.Add(InputInformations[i].PIV);
		LocalName.Add(InputInformations[i].Name);
		LocalThreads.Add(InputInformations[i].Threads);
		LocalPID.Add(InputInformations[i].ProcessID);
		LocalPPID.Add(InputInformations[i].ParentProcessID);
		LocalPIV.Add(InputInformations[i].PIV);
	}
	
	switch (SortingType)
	{
	case ESortingType::SortingType_Alphabetically:
		LocalName.Sort();
		for (int i = 0; i < LocalName.Num(); i++)
			LocalProcessInformationsArray.Add(InputInformations[CopiedLocalName.Find(LocalName[i])]);
		break;
	case ESortingType::SortingType_Threads:
		LocalThreads.Sort();
		for (int i = 0; i < LocalThreads.Num(); i++)
			LocalProcessInformationsArray.Add(InputInformations[CopiedLocalThreads.Find(LocalThreads[i])]);
		break;
	case ESortingType::SortingType_PID:
		LocalPID.Sort();
		for (int i = 0; i < LocalPID.Num(); i++)
			LocalProcessInformationsArray.Add(InputInformations[CopiedLocalPID.Find(LocalPID[i])]);
		break;
	case ESortingType::SortingType_PPID:
		LocalPPID.Sort();
		for (int i = 0; i < LocalPPID.Num(); i++)
			LocalProcessInformationsArray.Add(InputInformations[CopiedLocalPPID.Find(LocalPPID[i])]);
		break;
	case ESortingType::SortingType_PIV:
		LocalPIV.Sort();
		for (int i = 0; i < LocalPIV.Num(); i++)
			LocalProcessInformationsArray.Add(InputInformations[CopiedLocalPIV.Find(LocalPIV[i])]);
		break;
	}
	if (!ReturnReverse)
		Algo::Reverse(LocalProcessInformationsArray);
#endif
	return LocalProcessInformationsArray;
}

TArray<FProcessInformations> UIVExecutableExtensionLibrary::FilterProcessList(
	TArray<FProcessInformations> InputInformations, EFilterType FilterType, int RangeMin, int RangeMax)
{
	TArray<FProcessInformations> LocalProcessInformationsArray;
	
#ifdef _WIN64
	TArray<FString> NoDuplicates;
	for (int i = 0; i < InputInformations.Num(); i++)
	{
		switch (FilterType)
		{
		case EFilterType::FilterType_Duplicates:
			//Add Unique somehow doens't work here :)
			if (!NoDuplicates.Contains(InputInformations[i].Name))
			{
				NoDuplicates.Add(InputInformations[i].Name);
				LocalProcessInformationsArray.Add(InputInformations[i]);
			}
			break;
		case EFilterType::FilterType_Threads:
			if (InputInformations[i].Threads > RangeMin - 1 && InputInformations[i].Threads < RangeMax + 1)
				LocalProcessInformationsArray.Add(InputInformations[i]);
			break;
		case EFilterType::FilterType_PID:
			if (InputInformations[i].ProcessID > RangeMin - 1 && InputInformations[i].ProcessID < RangeMax + 1)
				LocalProcessInformationsArray.Add(InputInformations[i]);
			break;
		case EFilterType::FilterType_PPID:
			if (InputInformations[i].ParentProcessID > RangeMin - 1 && InputInformations[i].ParentProcessID < RangeMax + 1)
				LocalProcessInformationsArray.Add(InputInformations[i]);
			break;
		case EFilterType::FilterType_PIV:
			if (InputInformations[i].PIV > RangeMin - 1 && InputInformations[i].PIV < RangeMax + 1)
				LocalProcessInformationsArray.Add(InputInformations[i]);
			break;
		}
	}
#endif
	
	return LocalProcessInformationsArray;
}

int32 UIVExecutableExtensionLibrary::GetCurrentProcessId()
{
	return FPlatformProcess::GetCurrentProcessId();
}

bool UIVExecutableExtensionLibrary::GetCurrentProcessPath(int32 PID, FString& Path)
{
#ifdef _WIN64
	TCHAR LocalPath[MAX_PATH];
	HANDLE LocalHANDLE = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
	if (LocalHANDLE != INVALID_HANDLE_VALUE)
	{
		if (GetModuleFileNameEx(LocalHANDLE, NULL, LocalPath, MAX_PATH) == 0)
		{
			return false;
		}
		else
		{
			Path = LocalPath;
		}
		::CloseHandle(LocalHANDLE);
	}
	else
	{
		return false;
	}
	return true;
#else
	return false;
#endif
}

bool UIVExecutableExtensionLibrary::GetAllProcessModuleInformations(int32 PID, TArray<FModuleInformations>& Modules)
{
#ifdef _WIN64
	bool LocalBool = false;
	HANDLE LocalHANDLE = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	
	TArray<FModuleInformations> LocalModuleInformationsArray;
	FModuleInformations LocalModuleInformations;
	
	if (LocalHANDLE != INVALID_HANDLE_VALUE)
		{
		MODULEENTRY32 LocalMODULEENTRY32;
		LocalMODULEENTRY32.dwSize = sizeof(LocalMODULEENTRY32);
		if (::Module32First(LocalHANDLE, &LocalMODULEENTRY32))
		{
			do
			{
				LocalModuleInformations.Name = LocalMODULEENTRY32.szModule;
				LocalModuleInformations.Path = LocalMODULEENTRY32.szExePath;
				LocalModuleInformations.Size = LocalMODULEENTRY32.modBaseSize;
				LocalModuleInformationsArray.Add(LocalModuleInformations);
				LocalBool = true;
			} while (::Module32Next(LocalHANDLE, &LocalMODULEENTRY32));
		}

	}
	::CloseHandle(LocalHANDLE);
	if (LocalBool)
	{
		Modules = LocalModuleInformationsArray;
		return true;
	}
#endif
	return false;
}

FString UIVExecutableExtensionLibrary::GetMemoryStatistics()
{
	return FPlatformProcess::ExecutableName(false);
}

FMemoryStats UIVExecutableExtensionLibrary::GetProcessMemoryStatistics(int32 PID)
{
	FMemoryStats LocalMemoryStats;
#ifdef _WIN64
	PROCESS_MEMORY_COUNTERS LocalPROCESS_MEMORY_COUNTERS;
	HANDLE LocalHANDLE = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
	
	if (GetProcessMemoryInfo(LocalHANDLE, &LocalPROCESS_MEMORY_COUNTERS, sizeof(LocalPROCESS_MEMORY_COUNTERS)))
	{
		LocalMemoryStats.PageFaultCount = LocalPROCESS_MEMORY_COUNTERS.PageFaultCount;
		LocalMemoryStats.PeakWorkingSetSize = LocalPROCESS_MEMORY_COUNTERS.PeakWorkingSetSize;
		LocalMemoryStats.WorkingSetSize = LocalPROCESS_MEMORY_COUNTERS.WorkingSetSize;
		LocalMemoryStats.QuotaPeakPagedPoolUsage = LocalPROCESS_MEMORY_COUNTERS.QuotaPeakPagedPoolUsage;
		LocalMemoryStats.QuotaPagedPoolUsage = LocalPROCESS_MEMORY_COUNTERS.QuotaPagedPoolUsage;
		LocalMemoryStats.QuotaPeakNonPagedPoolUsage = LocalPROCESS_MEMORY_COUNTERS.QuotaPeakNonPagedPoolUsage;
		LocalMemoryStats.QuotaNonPagedPoolUsage = LocalPROCESS_MEMORY_COUNTERS.QuotaNonPagedPoolUsage;
		LocalMemoryStats.PagefileUsage = LocalPROCESS_MEMORY_COUNTERS.PagefileUsage;
		LocalMemoryStats.PeakPagefileUsage = LocalPROCESS_MEMORY_COUNTERS.PeakPagefileUsage;
		CloseHandle(LocalHANDLE);
	}
#endif
	return LocalMemoryStats;
}

double UIVExecutableExtensionLibrary::BytesToMegabytes(int64 Bytes)
{
	return Bytes * 0.00000095367432;
}

TArray<FFileInformations> UIVExecutableExtensionLibrary::EnumerateProcessFiles(int32 PID)
{
	TArray<FFileInformations> LocalFileInformationsArray;
	FFileInformations LocalFileInformations;
	
#ifdef _WIN64
	HANDLE LocalHANDLE = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
	WIN32_FIND_DATA LocalWIN32_FIND_DATA;
	HANDLE LocalHANDLE2 = FindFirstFileEx(L"C:\\*", FindExInfoStandard, &LocalWIN32_FIND_DATA, FindExSearchNameMatch, NULL, 0);
	
	if (LocalHANDLE2 != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (LocalWIN32_FIND_DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			LocalFileInformations.Name = LocalWIN32_FIND_DATA.cFileName;
			LocalFileInformations.AlternativeName = LocalWIN32_FIND_DATA.cAlternateFileName;
			LocalFileInformations.FileAttributes = GetFileAttributes(LocalWIN32_FIND_DATA.dwFileAttributes);
			LocalFileInformations.CreationTimeHigh = LocalWIN32_FIND_DATA.ftCreationTime.dwHighDateTime;
			LocalFileInformations.CreationTimeLow = LocalWIN32_FIND_DATA.ftCreationTime.dwLowDateTime;
			LocalFileInformations.LastAccessTimeHigh = LocalWIN32_FIND_DATA.ftLastAccessTime.dwHighDateTime;
			LocalFileInformations.LastAccessTimeLow = LocalWIN32_FIND_DATA.ftLastAccessTime.dwLowDateTime;
			LocalFileInformations.LastWriteTimeHigh = LocalWIN32_FIND_DATA.ftLastWriteTime.dwHighDateTime;
			LocalFileInformations.LastWriteTimeLow = LocalWIN32_FIND_DATA.ftLastWriteTime.dwLowDateTime;
			LocalFileInformations.FileSizeHigh = LocalWIN32_FIND_DATA.nFileSizeHigh;
			LocalFileInformations.FileSizeLow = LocalWIN32_FIND_DATA.nFileSizeLow;
			LocalFileInformations.Reserved0 = LocalWIN32_FIND_DATA.dwReserved0;
			LocalFileInformations.Reserved1 = LocalWIN32_FIND_DATA.dwReserved1;
			LocalFileInformationsArray.Add(LocalFileInformations);
		} while (FindNextFile(LocalHANDLE2, &LocalWIN32_FIND_DATA));
		FindClose(LocalHANDLE2);
	}
	CloseHandle(LocalHANDLE);
#endif
	return LocalFileInformationsArray;
}

bool UIVExecutableExtensionLibrary::GetForegroundProcess(int& PID)
{
#ifdef _WIN64
	HWND LocalHANDLE = NULL;
	DWORD LocalDWORD = 0;
	LocalHANDLE = GetForegroundWindow();
	if (LocalHANDLE != NULL) {
		if (GetWindowThreadProcessId(LocalHANDLE, &LocalDWORD)) {
			PID = (int)LocalDWORD;
			return true;
		}
	}
#endif
	return false;
}

bool UIVExecutableExtensionLibrary::IsProcessInForeground(int PID)
{
#ifdef _WIN64
	int LocalInt;
	if (GetForegroundProcess(LocalInt))
		if (LocalInt == PID)
			return true;
#endif
	return false;
}

#ifdef _WIN64
struct EnumData { DWORD LocalDWORD; TArray<FWindowInformations>* LocalWindowInformationsArray; };
#endif

TArray<FWindowInformations> UIVExecutableExtensionLibrary::ListWindowInformationsOfProcess(int PID)
{
	TArray<FWindowInformations> LocaWindowInformationsArray;
#ifdef _WIN64
	EnumData Data;
	Data.LocalDWORD = (DWORD)PID;
	Data.LocalWindowInformationsArray = &LocaWindowInformationsArray;
	EnumWindows(GetEnumWindowsProc, (LPARAM)&Data);
#endif
	return LocaWindowInformationsArray;
}



BOOL UIVExecutableExtensionLibrary::GetEnumWindowsProc(HWND HWND, LPARAM LParam)
{
	EnumData* LocalEnumData = (EnumData*)LParam;
	DWORD LocalDWORD;
	GetWindowThreadProcessId(HWND, &LocalDWORD);
	if (LocalDWORD == LocalEnumData->LocalDWORD)
	{
		int LocalInt = GetWindowTextLength(HWND);
		if (LocalInt > 0)
		{
			PWINDOWINFO pWInfo = new WINDOWINFO();
			pWInfo->cbSize = sizeof(WINDOWINFO);
			GetWindowInfo(HWND, pWInfo);
			FWindowInformations WInfo;

			TCHAR* buffer = new TCHAR[LocalInt + 1];
			GetWindowText(HWND, buffer, LocalInt + 1);
			FString WindowName(buffer);
			delete[] buffer;

			WInfo.Name = WindowName;
			WInfo.WindowID = (int64)HWND;
			WInfo.Visible = IsWindowVisible(HWND);
			WInfo.IsFocused = pWInfo->dwWindowStatus != 0;
			LocalEnumData->LocalWindowInformationsArray->Add(WInfo);
		}
	}
	return TRUE;
}

TArray<TEnumAsByte<EFileAttributeConstants>> UIVExecutableExtensionLibrary::GetFileAttributes(DWORD DWFileAttributes)
{
	TArray< TEnumAsByte<EFileAttributeConstants>> LocalFileAttributeConstants;
	if (DWFileAttributes & FILE_ATTRIBUTE_READONLY) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_ReadOnly);
	if (DWFileAttributes & FILE_ATTRIBUTE_HIDDEN) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Hidden);
	if (DWFileAttributes & FILE_ATTRIBUTE_SYSTEM) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_System);
	if (DWFileAttributes & FILE_ATTRIBUTE_DIRECTORY) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Directory);
	if (DWFileAttributes & FILE_ATTRIBUTE_ARCHIVE) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Archive);
	if (DWFileAttributes & FILE_ATTRIBUTE_DEVICE) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Device);
	if (DWFileAttributes & FILE_ATTRIBUTE_NORMAL) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Normal);
	if (DWFileAttributes & FILE_ATTRIBUTE_TEMPORARY) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Temporary);
	if (DWFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_SparseFile);
	if (DWFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_ReparsePoint);
	if (DWFileAttributes & FILE_ATTRIBUTE_COMPRESSED) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Compressed);
	if (DWFileAttributes & FILE_ATTRIBUTE_OFFLINE) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Offline);
	if (DWFileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_NotContentIndexed);
	if (DWFileAttributes & FILE_ATTRIBUTE_ENCRYPTED) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Encrypted);
	if (DWFileAttributes & FILE_ATTRIBUTE_INTEGRITY_STREAM) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_IntegrityStream);
	if (DWFileAttributes & FILE_ATTRIBUTE_VIRTUAL) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Virtual);
	if (DWFileAttributes & FILE_ATTRIBUTE_NO_SCRUB_DATA) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_NoScrubData);
	if (DWFileAttributes & FILE_ATTRIBUTE_EA) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_EA);
	if (DWFileAttributes & FILE_ATTRIBUTE_PINNED) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Pinned);
	if (DWFileAttributes & FILE_ATTRIBUTE_UNPINNED) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_Unpinned);
	if (DWFileAttributes & FILE_ATTRIBUTE_RECALL_ON_OPEN) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_RecallOnOpen);
	if (DWFileAttributes & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS) LocalFileAttributeConstants.Add(EFileAttributeConstants::FileAttributeConstants_RecallOnDataAccess);
	return LocalFileAttributeConstants;
}

