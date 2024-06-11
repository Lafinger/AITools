#include "VoiceWakeUp.h"

#include "Interfaces/IPluginManager.h"

DEFINE_LOG_CATEGORY(LogFVoiceWakeUpModule);

#define LOCTEXT_NAMESPACE "FVoiceWakeUpModule"

template<typename Ty>
constexpr const bool HasEmptyParam(const Ty& Arg1)
{
	if constexpr (std::is_base_of<FString, Ty>())
	{
		return Arg1.IsEmpty();
	}
	else if constexpr (std::is_base_of<FName, Ty>())
	{
		return Arg1.IsNone();
	}
	else if constexpr (std::is_base_of<FText, Ty>())
	{
		return Arg1.IsEmptyOrWhitespace();
	}
	else if constexpr (std::is_base_of<std::string, Ty>())
	{
		return Arg1.empty();
	}
	else
	{
#if ENGINE_MAJOR_VERSION >= 5
		return Arg1.IsEmpty();
#else
		return Arg1.Num() == 0;
#endif
	}
}

template<typename Ty, typename ...Args>
constexpr const bool HasEmptyParam(const Ty& Arg1, Args&& ...args)
{
	return HasEmptyParam(Arg1) || HasEmptyParam(std::forward<Args>(args)...);
}


// 类型察擦除迭代器
// template<typename ReturnTy, typename IteratorTy>
// 		constexpr const ReturnTy GetDataFromMapGroup(const FName& InGroup, const TArray<IteratorTy> InContainer)
// {
// 	if (HasEmptyParam(InGroup))
// 	{
// 		return ReturnTy();
// 	}
//
// 	for (const IteratorTy& IteratorData : InContainer)
// 	{
// 		if (IteratorData.GroupName.IsEqual(InGroup))
// 		{
// 			if (HasEmptyParam(IteratorData.Data))
// 			{
// 				return ReturnTy();
// 			}
//
// 			if constexpr (std::is_base_of<FAzSpeechRecognitionMap, ReturnTy>())
// 			{
// 				return IteratorData;
// 			}
// 			else
// 			{
// 				return IteratorData.Data;
// 			}
// 		}
// 	}
//
// 	return ReturnTy();
// }


TArray<FString> GetWhitelistedRuntimeLibs()
{
	TArray<FString> WhitelistedLibs;

	const FString WhitelistedLibsDef(VOICEWAKEUP_WHITELISTED_BINARIES);
	WhitelistedLibsDef.ParseIntoArray(WhitelistedLibs, TEXT(";"));

	return WhitelistedLibs;
}

FString GetRuntimeLibsDirectory()
{
	FString BinariesDirectory;

#if WITH_EDITOR
#ifdef VOICEWAKEUP_THIRDPARTY_BINARY_SUBDIR
	const TSharedPtr<IPlugin> PluginInterface = IPluginManager::Get().FindPlugin("Workflows");
	BinariesDirectory = FPaths::Combine(PluginInterface->GetBaseDir(), TEXT("Source"), TEXT("ThirdParty"), TEXT("VoiceWakeUpSdk"), TEXT(VOICEWAKEUP_THIRDPARTY_BINARY_SUBDIR));
#endif
#else
	BinariesDirectory = FPaths::GetPath(FPlatformProcess::ExecutablePath());
#endif

	if (HasEmptyParam(BinariesDirectory))
	{
		UE_LOG(LogFVoiceWakeUpModule, Error, TEXT("ThreadID:%d, %s: Failed to get the location of the runtime libraries. Please check and validate your installation."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return FString();
	}

	FPaths::NormalizeDirectoryName(BinariesDirectory);

#if PLATFORM_HOLOLENS
	FPaths::MakePathRelativeTo(BinariesDirectory, *(FPaths::RootDir() + TEXT("/")));
#endif

	return BinariesDirectory;
}

void LogLastError(const FString& FailLib)
{
	const uint32 ErrorID = FPlatformMisc::GetLastError();
	TCHAR ErrorBuffer[MAX_SPRINTF];
	FPlatformMisc::GetSystemErrorMessage(ErrorBuffer, MAX_SPRINTF, ErrorID);

	UE_LOG(LogFVoiceWakeUpModule, Warning, TEXT("ThreadID:%d, %s: Failed to load runtime library \"%s\": %u (%s)."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *FailLib, ErrorID, ErrorBuffer);
}

void FVoiceWakeUpModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	const TSharedPtr<IPlugin> PluginInterface = IPluginManager::Get().FindPlugin("Workflows");
	UE_LOG(LogFVoiceWakeUpModule, Display, TEXT("ThreadID:%d, Initializing plugin %s version %s."), FPlatformTLS::GetCurrentThreadId(), *PluginInterface->GetFriendlyName(), *PluginInterface->GetDescriptor().VersionName);

// #if !PLATFORM_ANDROID && !UE_BUILD_SHIPPING
// 	if (FPaths::DirectoryExists(UAzSpeechHelper::GetAzSpeechLogsBaseDir()))
// 	{
// 		IFileManager::Get().DeleteDirectory(*UAzSpeechHelper::GetAzSpeechLogsBaseDir(), false, true);
// 	}
// #endif

#ifdef VOICEWAKEUP_WHITELISTED_BINARIES
	LoadRuntimeLibraries();
#endif

// #if WITH_EDITOR && !AZSPEECH_SUPPORTED_PLATFORM
// 	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Currently, AzSpeech does not officially support the platform you're using/targeting. If you encounter any issue and can/want to contribute, get in touch! :)\n\nRepository Link: github.com/lucoiso/UEAzSpeech")));
// #endif
}

void FVoiceWakeUpModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	const TSharedPtr<IPlugin> PluginInterface = IPluginManager::Get().FindPlugin("Workflows");
	UE_LOG(LogFVoiceWakeUpModule, Display, TEXT("ThreadID:%d, Shutting down plugin %s version %s."), FPlatformTLS::GetCurrentThreadId(), *PluginInterface->GetFriendlyName(), *PluginInterface->GetDescriptor().VersionName);

#ifdef VOICEWAKEUP_WHITELISTED_BINARIES
	UnloadRuntimeLibraries();
#endif
}

void FVoiceWakeUpModule::LoadRuntimeLibraries()
{
	const FString BinariesDirectory = GetRuntimeLibsDirectory();
	const TArray<FString> WhitelistedLibs = GetWhitelistedRuntimeLibs();

	UE_LOG(LogFVoiceWakeUpModule, Display, TEXT("ThreadID:%d, %s: Loading runtime libraries in directory \"%s\"."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *BinariesDirectory);

	FPlatformProcess::PushDllDirectory(*BinariesDirectory);

	for (const FString& RuntimeLib : WhitelistedLibs)
	{
		void* Handle = nullptr;

		// Attempt to load the file more than one time in case of a temporary lock
		constexpr unsigned int MaxAttempt = 5u;
		constexpr float AttemptSleepDelay = 0.5f;
		for (unsigned int Attempt = 1u; !Handle && Attempt <= MaxAttempt; ++Attempt)
		{
			if (Attempt > 1u)
			{
				FPlatformProcess::Sleep(AttemptSleepDelay);
			}

			UE_LOG(LogFVoiceWakeUpModule, Display, TEXT("ThreadID:%d, %s: Attempting to load runtime library \"%s\" (%u/%u)."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *RuntimeLib, Attempt, MaxAttempt);
			if (Handle = FPlatformProcess::GetDllHandle(*RuntimeLib); Handle)
			{
				break;
			}
		}

		if (!Handle)
		{
			LogLastError(FPaths::Combine(BinariesDirectory, RuntimeLib));
			continue;
		}

		UE_LOG(LogFVoiceWakeUpModule, Display, TEXT("ThreadID:%d, %s: Loaded runtime library \"%s\"."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *RuntimeLib);
		RuntimeLibraries.Add(Handle);
	}

	FPlatformProcess::PopDllDirectory(*BinariesDirectory);
}

void FVoiceWakeUpModule::UnloadRuntimeLibraries()
{
	UE_LOG(LogFVoiceWakeUpModule, Display, TEXT("ThreadID:%d, %s: Unloading runtime libraries."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));

	for (void*& Handle : RuntimeLibraries)
	{
		if (!Handle)
		{
			continue;
		}

		FPlatformProcess::FreeDllHandle(Handle);
		Handle = nullptr;
	}

	RuntimeLibraries.Empty();
}


#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FVoiceWakeUpModule, VoiceWakeUp)