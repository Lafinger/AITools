// Copyright 2023 AIRT,  All Rights Reserved.


#include "iVisualFileHeplerBPLibrary.h"


#include "AudioThread.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "IVCommon.h"
#include "IVPlatformLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Async/Async.h"
#include "EditorFramework/AssetImportData.h"
#include "Interfaces/IPluginManager.h"
#include "Kismet/KismetStringLibrary.h"
#include "Misc/FileHelper.h"
#include "UObject/SavePackage.h"
#include "Misc/MonitoredProcess.h"

#if WITH_EDITOR
#include "DesktopPlatformModule.h"
#include "Engine/GameViewportClient.h"
#include "IDesktopPlatform.h"
#include "Engine/Engine.h"
#include "Widgets/SWindow.h"
#endif

#if PLATFORM_WINDOWS
#include "HAL/FileManager.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Microsoft/COMPointer.h"
#include <commdlg.h>
#include <shlobj.h>
#include <Winver.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif
//渲染线程wrapper struct
struct FUpdateTextureData
{
	UTexture2D* Texture2D;
	FUpdateTextureRegion2D Region;
	uint32 Pitch;
	TArray64<uint8> BufferArray;
	TSharedPtr<IImageWrapper> Wrapper;	
};




TMap<FString, TSharedPtr<FMonitoredProcess>> UiVisualFileHeplerBPLibrary::CurrentProcRegistry;

USoundWave* UiVisualFileHeplerBPLibrary::ConvertAudioDataToSoundWave(const TArray<uint8>& RawData, const FString& OutputModule, const FString& RelativeOutputDirectory, const FString& OutputAssetName)
{
#if PLATFORM_ANDROID
	if (!CheckAndroidPermission("android.permission.WRITE_EXTERNAL_STORAGE"))
	{
		return nullptr;
	}
#endif

	if (!IsAudioDataValid(RawData))
	{
		UE_LOG(LogIVFileHepler, Error, TEXT("%s: RawData is empty"), *FString(__func__));
		return nullptr;
	}

	USoundWave* SoundWave = nullptr;

	FWaveModInfo WaveInfo;
	WaveInfo.ReadWaveInfo(RawData.GetData(), RawData.Num());


	const int32 ChannelCount = static_cast<int32>(*WaveInfo.pChannels);
	const int32 SizeOfSample = (*WaveInfo.pBitsPerSample) / 8;
	const int32 NumSamples = WaveInfo.SampleDataSize / SizeOfSample;
	const int32 NumFrames = NumSamples / ChannelCount;

	bool bCreatedNewPackage = false;

	if (IVCommon::HasEmptyParam(OutputModule) || IVCommon::HasEmptyParam(OutputAssetName))
	{
		//Create a new object from the transient package
		SoundWave = NewObject<USoundWave>(GetTransientPackage(), *OutputAssetName);
	}
	else
	{
		if (!UIVPlatformLibrary::IsContentModuleAvailable(OutputModule))
		{
			UE_LOG(LogIVFileHepler, Error, TEXT("%s: Module '%s' is not available"), *FString(__func__), *OutputModule);
			return nullptr;
		}

		FString TargetFilename = FPaths::Combine(UIVPlatformLibrary::QualifyModulePath(OutputModule), RelativeOutputDirectory, OutputAssetName);
		FPaths::NormalizeFilename(TargetFilename);

		UPackage* const Package = CreatePackage(*TargetFilename);

		if (USoundWave* const ExistingSoundWave = FindObject<USoundWave>(Package, *OutputAssetName))
		{

			FAudioThread::RunCommandOnAudioThread([ExistingSoundWave]() { ExistingSoundWave->FreeResources(); });
			SoundWave = ExistingSoundWave;
		}
		else
		{
			SoundWave = NewObject<USoundWave>(Package, *OutputAssetName, RF_Public | RF_Standalone);
		}

		bCreatedNewPackage = true;
	}

	if (SoundWave)
	{
#if WITH_EDITORONLY_DATA
		SoundWave->RawData.UpdatePayload(FSharedBuffer::Clone(RawData.GetData(), RawData.Num()));

#endif

		SoundWave->RawPCMDataSize = WaveInfo.SampleDataSize;
		SoundWave->RawPCMData = static_cast<uint8*>(FMemory::Malloc(WaveInfo.SampleDataSize));
		FMemory::Memcpy(SoundWave->RawPCMData, WaveInfo.SampleDataStart, WaveInfo.SampleDataSize);

		SoundWave->Duration = static_cast<float>(NumFrames) / *WaveInfo.pSamplesPerSec;
		SoundWave->SetSampleRate(*WaveInfo.pSamplesPerSec);
		SoundWave->NumChannels = ChannelCount;
		SoundWave->TotalSamples = *WaveInfo.pSamplesPerSec * SoundWave->Duration;

		SoundWave->SetImportedSampleRate(*WaveInfo.pSamplesPerSec);

		SoundWave->CuePoints.Reset(WaveInfo.WaveCues.Num());
		for (FWaveCue& WaveCue : WaveInfo.WaveCues)
		{
			FSoundWaveCuePoint NewCuePoint;
			NewCuePoint.CuePointID = static_cast<int32>(WaveCue.CuePointID);
			NewCuePoint.FrameLength = static_cast<int32>(WaveCue.SampleLength);
			NewCuePoint.FramePosition = static_cast<int32>(WaveCue.Position);
			NewCuePoint.Label = WaveCue.Label;

			SoundWave->CuePoints.Add(NewCuePoint);
		}

#if WITH_EDITORONLY_DATA 
		if (WaveInfo.TimecodeInfo.IsValid())
		{
			SoundWave->SetTimecodeInfo(*WaveInfo.TimecodeInfo);
		}
#endif

		if (bCreatedNewPackage)
		{
#if WITH_EDITORONLY_DATA 
			if (const UAudioSettings* const AudioSettings = GetDefault<UAudioSettings>())
			{
				FAudioThread::RunCommandOnAudioThread([AudioSettings, SoundWave]() { SoundWave->SetSoundAssetCompressionType(Audio::ToSoundAssetCompressionType(AudioSettings->DefaultAudioCompressionType)); });
			}
			else
			{
				FAudioThread::RunCommandOnAudioThread([SoundWave]() { SoundWave->SetSoundAssetCompressionType(ESoundAssetCompressionType::BinkAudio); });
			}

#endif

			SoundWave->MarkPackageDirty();
			FAssetRegistryModule::AssetCreated(SoundWave);

			const FString TempPackageName = SoundWave->GetPackage()->GetName();
			const FString TempPackageFilename = FPackageName::LongPackageNameToFilename(TempPackageName, FPackageName::GetAssetPackageExtension());

			FSavePackageArgs SaveArgs;
			SaveArgs.SaveFlags = RF_Public | RF_Standalone;
			UPackage::SavePackage(SoundWave->GetPackage(), SoundWave, *TempPackageFilename, SaveArgs);

#if WITH_EDITOR
			TArray<FAssetData> SyncAssets;
			SyncAssets.Add(FAssetData(SoundWave));
			GEditor->SyncBrowserToObjects(SyncAssets);
#endif
		}

		UE_LOG(LogIVFileHepler, Display, TEXT("%s: Result: Success"), *FString(__func__));
		return SoundWave;
	}

	UE_LOG(LogIVFileHepler, Error, TEXT("%s: Cannot create a new Sound Wave"), *FString(__func__));
	return nullptr;
}

bool UiVisualFileHeplerBPLibrary::LoadProc(FString Path, FString CommandLineArguments, bool bHiddenWindow,
                                             bool bLogCatch, FString& OutProcessID)
{
	
	
	const FString PathPrefix="LocalExe://";
	const FString PathPrefixLocalSaved="LocalSaved://";
	if((UKismetStringLibrary::StartsWith(Path,PathPrefix,ESearchCase::IgnoreCase)))
	{
		FString File=UKismetStringLibrary::RightChop(Path,PathPrefix.Len());
		Path=FPaths::Combine(UiVisualFileHeplerBPLibrary::GetExtrasExeDir(TEXT("IVCommon"))/File);
		FPaths::NormalizeFilename(Path);
	}else if((UKismetStringLibrary::StartsWith(Path,PathPrefixLocalSaved,ESearchCase::IgnoreCase)))
	{
		FString File=UKismetStringLibrary::RightChop(Path,PathPrefixLocalSaved.Len());
		Path=FPaths::Combine(FPaths::ProjectSavedDir()/File);
		FPaths::NormalizeFilename(Path);
	}
	FString ProcItWorkingDir = FPaths::GetPath(Path);
	
	
	TSharedPtr<FMonitoredProcess> CurrentProc = MakeShareable(new FMonitoredProcess(Path, CommandLineArguments, ProcItWorkingDir, bHiddenWindow, bLogCatch));
	FString CurrentProcLaunchInfo = FString::Printf(TEXT("ProcPath:%s Parms:%s WorkdingDir:%s"), *Path, *CommandLineArguments, *ProcItWorkingDir);
	UE_LOG(LogIVFileHepler, Verbose, TEXT("Launching: %s"), *CurrentProcLaunchInfo);

	if (bLogCatch)
	{
		CurrentProc->OnOutput().BindStatic(&UiVisualFileHeplerBPLibrary::OutputProcLog, Path);
	}
	
	FPlatformProcess::Sleep(0.1f);

	if (!CurrentProc.IsValid() || !CurrentProc->Launch())
	{
		FString ErrorMsg = FString::Printf(TEXT("Failed To Launch: %s "), *CurrentProcLaunchInfo);
		UE_LOG(LogIVFileHepler, Error, TEXT("%s"), *ErrorMsg);
		return false;
	}

	OutProcessID = FGuid::NewGuid().ToString();

	CurrentProcRegistry.Add(OutProcessID, CurrentProc);
	return true;
}

bool UiVisualFileHeplerBPLibrary::UnLoadProc(FString ProcessID)
{
	if (ProcessID.IsEmpty())
	{
		return false;
	}

	TSharedPtr<FMonitoredProcess> TargetProc = *CurrentProcRegistry.Find(ProcessID);
	if (!TargetProc.IsValid() || !TargetProc->Update())
	{
		return false;
	}

	TargetProc->Cancel(true);
	CurrentProcRegistry.Remove(ProcessID);
	return true;
}

void UiVisualFileHeplerBPLibrary::UnLoadProcList()
{
	TArray<TSharedPtr<FMonitoredProcess>> CurrentProcList;
	CurrentProcRegistry.GenerateValueArray(CurrentProcList);
	for (const TSharedPtr<FMonitoredProcess>& ProcIt : CurrentProcList)
	{
		if (ProcIt.IsValid() && ProcIt->Update())
		{
			ProcIt->Cancel(true);
		}
	}

	CurrentProcList.Empty();
	CurrentProcRegistry.Empty();
}

void UiVisualFileHeplerBPLibrary::OutputProcLog(FString Log, FString ProcPath)
{
	FString LogMsg = FString::Printf(TEXT("%s: %s"), *FPaths::GetCleanFilename(ProcPath), *Log);
	UE_LOG(LogIVFileHepler, Log, TEXT("%s"), *LogMsg);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("IVProcess:Screen: %s"), *LogMsg));
	}
}


void FIVLambdaRunnable::RunLambdaOnBackGroundThread(TFunction<void()> InFunction)
{
	Async(EAsyncExecution::Thread, InFunction);
}

void FIVLambdaRunnable::RunLambdaOnBackGroundThreadPool(TFunction<void()> InFunction)
{
	Async(EAsyncExecution::ThreadPool, InFunction);
}

FGraphEventRef FIVLambdaRunnable::RunShortLambdaOnGameThread(TFunction<void()> InFunction)
{
	return FFunctionGraphTask::CreateAndDispatchWhenReady(InFunction, TStatId(), nullptr, ENamedThreads::GameThread);
}

FGraphEventRef FIVLambdaRunnable::RunShortLambdaOnBackGroundTask(TFunction<void()> InFunction)
{
	return FFunctionGraphTask::CreateAndDispatchWhenReady(InFunction, TStatId(), nullptr, ENamedThreads::AnyThread);
}

void FIVLambdaRunnable::SetTimeout(TFunction<void()> OnDone, float DurationInSec, bool bCallbackOnGameThread)
{
	RunLambdaOnBackGroundThread([OnDone, DurationInSec, bCallbackOnGameThread]()
	{
		FPlatformProcess::Sleep(DurationInSec);

		if (bCallbackOnGameThread)
		{
			RunShortLambdaOnGameThread(OnDone);
		}
		else
		{
			OnDone();
		}
	});
}

FEnginePath UiVisualFileHeplerBPLibrary::GetEngineDirectories()
{
	FEnginePath EnginePath;
	
	EnginePath.Directory = FPaths::ConvertRelativePathToFull(FPaths::EngineDir());
	EnginePath.Config = FPaths::ConvertRelativePathToFull(FPaths::EngineConfigDir());
	EnginePath.Content = FPaths::ConvertRelativePathToFull(FPaths::EngineContentDir());
	EnginePath.Intermediate = FPaths::ConvertRelativePathToFull(FPaths::EngineIntermediateDir());
	EnginePath.Plugins = FPaths::ConvertRelativePathToFull(FPaths::EnginePluginsDir());
	EnginePath.Saved =FPaths::ConvertRelativePathToFull( FPaths::EngineSavedDir());
	EnginePath.User = FPaths::ConvertRelativePathToFull(FPaths::EngineUserDir());
	EnginePath.DefaultLayout = FPaths::ConvertRelativePathToFull(FPaths::EngineDefaultLayoutDir());
	EnginePath.PlatformExtensions = FPaths::ConvertRelativePathToFull(FPaths::EnginePlatformExtensionsDir());
	EnginePath.UserLayout = FPaths::ConvertRelativePathToFull(FPaths::EngineUserLayoutDir());
	
	return EnginePath;
}

FProjectPath UiVisualFileHeplerBPLibrary::GetProjectDirectories()
{
	FProjectPath ProjectPath;
	
	ProjectPath.Directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	ProjectPath.Config = FPaths::ConvertRelativePathToFull(FPaths::ProjectConfigDir());
	ProjectPath.Content = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
	ProjectPath.Intermediate = FPaths::ConvertRelativePathToFull(FPaths::ProjectIntermediateDir());
	ProjectPath.Log = FPaths::ConvertRelativePathToFull(FPaths::ProjectLogDir());
	ProjectPath.Mods = FPaths::ConvertRelativePathToFull(FPaths::ProjectModsDir());
	ProjectPath.Plugins = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir());
	ProjectPath.Saved = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	ProjectPath.User = FPaths::ConvertRelativePathToFull(FPaths::ProjectUserDir());
	ProjectPath.PersistentDownload = FPaths::ConvertRelativePathToFull(FPaths::ProjectPersistentDownloadDir());
	ProjectPath.PlatformExtensions = FPaths::ConvertRelativePathToFull(FPaths::ProjectPlatformExtensionsDir());
	
	return ProjectPath;
}

FString UiVisualFileHeplerBPLibrary::GetProjectDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
}

FString UiVisualFileHeplerBPLibrary::GetProjectConfigDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectConfigDir());
}

FString UiVisualFileHeplerBPLibrary::GetProjectContentDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
}

FString UiVisualFileHeplerBPLibrary::GetProjectLogDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectLogDir());
}

FString UiVisualFileHeplerBPLibrary::GetProjectModsDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectModsDir());
}

FString UiVisualFileHeplerBPLibrary::GetProjectPluginsDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir());
}

FString UiVisualFileHeplerBPLibrary::GetProjectSavedDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
}

FString UiVisualFileHeplerBPLibrary::GetProjectUserDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectUserDir());
}

FString UiVisualFileHeplerBPLibrary::GetProjectPersistentDownloadDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectPersistentDownloadDir());
}

FString UiVisualFileHeplerBPLibrary::GetProjectPlatformExtensionsDir()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectPlatformExtensionsDir());
}

FString UiVisualFileHeplerBPLibrary::GetExtrasDir(const FString Plugin, const FString FileName)
{
	FString PluginDir = IPluginManager::Get().FindPlugin(Plugin)->GetBaseDir();
	FString ExtrasDir = FPaths::Combine(PluginDir, TEXT("Extras"), FileName);

	return (FPaths::ConvertRelativePathToFull(ExtrasDir));
}

FString UiVisualFileHeplerBPLibrary::GetExtrasDataDir(const FString Plugin)
{
	return  GetExtrasDir(Plugin,"Data");
}

FString UiVisualFileHeplerBPLibrary::GetExtrasExeDir(const FString Plugin)
{
	return  GetExtrasDir(Plugin,"Exe");
}

FString UiVisualFileHeplerBPLibrary::GetExtrasImageDir(const FString Plugin)
{
	return  GetExtrasDir(Plugin,"Image");
}

FString UiVisualFileHeplerBPLibrary::GetIVActorJsonConfigDir()
{
	return FPaths::ProjectConfigDir() / "ActorJsonConfig";
}

FString UiVisualFileHeplerBPLibrary::GetIVProjectJsonConfigDir()
{
	return FPaths::ProjectConfigDir() / "ProjectJsonConfig";
}

bool UiVisualFileHeplerBPLibrary::SaveBytesToFile(const TArray<uint8>& Bytes, const FString& Directory,
                                              const FString& FileName, bool bLogSave)
{
	IPlatformFile& PlatformFile=FPlatformFileManager::Get().GetPlatformFile();
	
	if (PlatformFile.CreateDirectoryTree(*Directory))
	{
		FString AbsoluteFilePath;

		// 获取绝对路径
		if (Directory.EndsWith(TEXT("/")))
		{
			AbsoluteFilePath = FPaths::ConvertRelativePathToFull(Directory + FileName);
		}
		else
		{
			AbsoluteFilePath = FPaths::ConvertRelativePathToFull(Directory + "/" + FileName);
		}
		
		bool bSaveSuccesful = FFileHelper::SaveArrayToFile(Bytes, *AbsoluteFilePath);

		if (bLogSave)
		{
			if (bSaveSuccesful)
			{
				UE_LOG(LogIVFileHepler, Log, TEXT("Saved: %s with %d bytes"), *AbsoluteFilePath, Bytes.Num());
			}
			else
			{
				UE_LOG(LogIVFileHepler, Log, TEXT("Failed to save: %s"), *AbsoluteFilePath);
			}
		}

		return bSaveSuccesful;
	}

	return false;
}

bool UiVisualFileHeplerBPLibrary::ReadBytesFromFile(const FString& Directory, const FString& FileName,
	TArray<uint8>& OutBytes)
{
	FString AbsoluteFilePath = Directory + "/" + FileName;
	
	return FFileHelper::LoadFileToArray(OutBytes, *AbsoluteFilePath);
}

UTexture2D* UiVisualFileHeplerBPLibrary::Conv_BytesToTexture(const TArray<uint8>& InBytes)
{
	////将 UTexture2D 转换为image
	UTexture2D* Texture = nullptr;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	EImageFormat DetectedFormat = ImageWrapperModule.DetectImageFormat(InBytes.GetData(), InBytes.Num());
	DetectedFormat=EImageFormat::PNG;
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(DetectedFormat);

	////设置compressed bytes——我们需要游戏线程上的这些信息来确定纹理大小，否则我们将需要一个完整的异步回调
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(InBytes.GetData(), InBytes.Num()))
	{
		//创建给定尺寸的图像
		Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
		Texture->UpdateResource();

		//在后台线程池上解压缩
		FIVLambdaRunnable::RunLambdaOnBackGroundThreadPool([ImageWrapper, Texture] {
			TArray64<uint8> UncompressedBGRA;
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
			{

				FUpdateTextureData* UpdateData = new FUpdateTextureData;
				UpdateData->Texture2D = Texture;
				UpdateData->Region = FUpdateTextureRegion2D(0, 0, 0, 0, ImageWrapper->GetWidth(), ImageWrapper->GetHeight());
				UpdateData->BufferArray = UncompressedBGRA;
				UpdateData->Pitch = ImageWrapper->GetWidth() * 4;
				UpdateData->Wrapper = ImageWrapper;
				
				ENQUEUE_RENDER_COMMAND(BytesToTextureCommand)(
					[UpdateData](FRHICommandList& CommandList)
				{
					RHIUpdateTexture2D(
						((FTextureResource*)UpdateData->Texture2D->GetResource())->TextureRHI->GetTexture2D(),
						0,
						UpdateData->Region,
						UpdateData->Pitch,
						UpdateData->BufferArray.GetData()
					);
					delete UpdateData; //释放数据
				});//End Enqueue
			}
		});
	}
	else
	{
		UE_LOG(LogIVFileHepler,Warning, TEXT("Invalid image format cannot decode %d"), (int32)DetectedFormat);
	}

	return Texture;
}

UTexture2D* UiVisualFileHeplerBPLibrary::LoadImageFromFile(FString ImagePath /**=TEXT("LocalImage://")*/)
{
	//注意：此操作只针对放入iVisual插件对应文件夹有用
	const FString PathPrefix="LocalImage://";
	if((UKismetStringLibrary::StartsWith(ImagePath,PathPrefix,ESearchCase::IgnoreCase)))
	{
		FString File=UKismetStringLibrary::RightChop(ImagePath,PathPrefix.Len());
		ImagePath=FPaths::Combine(UiVisualFileHeplerBPLibrary::GetExtrasImageDir(TEXT("IVCommon"))/File);
	}

	if (!FPaths::FileExists(ImagePath))
	{
		UE_LOG(LogIVFileHepler, Error, TEXT("No Valid Image File Found At %s"), *ImagePath);
		return nullptr;
	}

	// 获取图片字节数据
	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *ImagePath))
	{
		return nullptr;
	}

	return  Conv_BytesToTexture(FileData);
	
}

UTexture2D* UiVisualFileHeplerBPLibrary::LoadImageFromExplorer(const FString& DefaultPath)
{
	TArray<FString> OutFilenames;

	FString	FileTypes = TEXT("All Files (*.png;*.jpg;)|*.png;*.jpg");

#if WITH_EDITOR
	if (GEngine && GEngine->GameViewport)
	{
		void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform)
		{
			//Opening the file picker!
			uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
			DesktopPlatform->OpenFileDialog(ParentWindowHandle, TEXT("Choose A File"), DefaultPath, FString(""), FileTypes, SelectionFlag, OutFilenames);
		}
	}
#elif PLATFORM_WINDOWS
	TComPtr<IFileDialog> FileDialog;
	if (SUCCEEDED(::CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_IFileOpenDialog, IID_PPV_ARGS_Helper(&FileDialog))))
	{
		// Set up common settings
		FileDialog->SetTitle(TEXT("Choose A File"));
		if (!DefaultPath.IsEmpty())
		{
			// SHCreateItemFromParsingName requires the given path be absolute and use \ rather than / as our normalized paths do
			FString DefaultWindowsPath = FPaths::ConvertRelativePathToFull(DefaultPath);
			DefaultWindowsPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);

			TComPtr<IShellItem> DefaultPathItem;
			if (SUCCEEDED(::SHCreateItemFromParsingName(*DefaultWindowsPath, nullptr, IID_PPV_ARGS(&DefaultPathItem))))
			{
				FileDialog->SetFolder(DefaultPathItem);
			}
		}

		// Set-up the file type filters
		TArray<FString> UnformattedExtensions;
		TArray<COMDLG_FILTERSPEC> FileDialogFilters;
		{
			const FString DefaultFileTypes = TEXT("All Files (*.*)|*.*");
			DefaultFileTypes.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);

			if (UnformattedExtensions.Num() % 2 == 0)
			{
				FileDialogFilters.Reserve(UnformattedExtensions.Num() / 2);
				for (int32 ExtensionIndex = 0; ExtensionIndex < UnformattedExtensions.Num();)
				{
					COMDLG_FILTERSPEC& NewFilterSpec = FileDialogFilters[FileDialogFilters.AddDefaulted()];
					NewFilterSpec.pszName = *UnformattedExtensions[ExtensionIndex++];
					NewFilterSpec.pszSpec = *UnformattedExtensions[ExtensionIndex++];
				}
			}
		}
		FileDialog->SetFileTypes(FileDialogFilters.Num(), FileDialogFilters.GetData());

		// Show the picker
		if (SUCCEEDED(FileDialog->Show(NULL)))
		{
			int32 OutFilterIndex = 0;
			if (SUCCEEDED(FileDialog->GetFileTypeIndex((UINT*)&OutFilterIndex)))
			{
				OutFilterIndex -= 1; // GetFileTypeIndex returns a 1-based index
			}

			TFunction<void(const FString&)> AddOutFilename = [&OutFilenames](const FString& InFilename)
			{
				FString& OutFilename = OutFilenames.Add_GetRef(InFilename);
				OutFilename = IFileManager::Get().ConvertToRelativePath(*OutFilename);
				FPaths::NormalizeFilename(OutFilename);
			};

			{
				IFileOpenDialog* FileOpenDialog = static_cast<IFileOpenDialog*>(FileDialog.Get());

				TComPtr<IShellItemArray> Results;
				if (SUCCEEDED(FileOpenDialog->GetResults(&Results)))
				{
					DWORD NumResults = 0;
					Results->GetCount(&NumResults);
					for (DWORD ResultIndex = 0; ResultIndex < NumResults; ++ResultIndex)
					{
						TComPtr<IShellItem> Result;
						if (SUCCEEDED(Results->GetItemAt(ResultIndex, &Result)))
						{
							PWSTR pFilePath = nullptr;
							if (SUCCEEDED(Result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
							{
								AddOutFilename(pFilePath);
								::CoTaskMemFree(pFilePath);
							}
						}
					}
				}
			}
		}
	}
#endif

	UTexture2D* Texture=nullptr;
	if (OutFilenames.Num() > 0)
	{
		UE_LOG(LogIVFileHepler,Log,TEXT("Read image:%s"),*FPaths::ConvertRelativePathToFull(OutFilenames[0]))
		return  UiVisualFileHeplerBPLibrary::LoadImageFromFile(FPaths::ConvertRelativePathToFull(OutFilenames[0]));
	}
	else
	{
		UE_LOG(LogIVFileHepler, Warning, TEXT("not valid texture"));
		return nullptr;
	}
}

FString UiVisualFileHeplerBPLibrary::LoadImageFromExplorerToBase64(const FString& DefaultPath)
{
		TArray<FString> OutFilenames;

	FString	FileTypes = TEXT("All Files (*.png;*.jpg;)|*.png;*.jpg");

#if WITH_EDITOR
	if (GEngine && GEngine->GameViewport)
	{
		void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform)
		{
			//Opening the file picker!
			uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
			DesktopPlatform->OpenFileDialog(ParentWindowHandle, TEXT("Choose A File"), DefaultPath, FString(""), FileTypes, SelectionFlag, OutFilenames);
		}
	}
#elif PLATFORM_WINDOWS
	TComPtr<IFileDialog> FileDialog;
	if (SUCCEEDED(::CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_IFileOpenDialog, IID_PPV_ARGS_Helper(&FileDialog))))
	{
		// Set up common settings
		FileDialog->SetTitle(TEXT("Choose A File"));
		if (!DefaultPath.IsEmpty())
		{
			// SHCreateItemFromParsingName requires the given path be absolute and use \ rather than / as our normalized paths do
			FString DefaultWindowsPath = FPaths::ConvertRelativePathToFull(DefaultPath);
			DefaultWindowsPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);

			TComPtr<IShellItem> DefaultPathItem;
			if (SUCCEEDED(::SHCreateItemFromParsingName(*DefaultWindowsPath, nullptr, IID_PPV_ARGS(&DefaultPathItem))))
			{
				FileDialog->SetFolder(DefaultPathItem);
			}
		}

		// Set-up the file type filters
		TArray<FString> UnformattedExtensions;
		TArray<COMDLG_FILTERSPEC> FileDialogFilters;
		{
			const FString DefaultFileTypes = TEXT("All Files (*.*)|*.*");
			DefaultFileTypes.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);

			if (UnformattedExtensions.Num() % 2 == 0)
			{
				FileDialogFilters.Reserve(UnformattedExtensions.Num() / 2);
				for (int32 ExtensionIndex = 0; ExtensionIndex < UnformattedExtensions.Num();)
				{
					COMDLG_FILTERSPEC& NewFilterSpec = FileDialogFilters[FileDialogFilters.AddDefaulted()];
					NewFilterSpec.pszName = *UnformattedExtensions[ExtensionIndex++];
					NewFilterSpec.pszSpec = *UnformattedExtensions[ExtensionIndex++];
				}
			}
		}
		FileDialog->SetFileTypes(FileDialogFilters.Num(), FileDialogFilters.GetData());

		// Show the picker
		if (SUCCEEDED(FileDialog->Show(NULL)))
		{
			int32 OutFilterIndex = 0;
			if (SUCCEEDED(FileDialog->GetFileTypeIndex((UINT*)&OutFilterIndex)))
			{
				OutFilterIndex -= 1; // GetFileTypeIndex returns a 1-based index
			}

			TFunction<void(const FString&)> AddOutFilename = [&OutFilenames](const FString& InFilename)
			{
				FString& OutFilename = OutFilenames.Add_GetRef(InFilename);
				OutFilename = IFileManager::Get().ConvertToRelativePath(*OutFilename);
				FPaths::NormalizeFilename(OutFilename);
			};

			{
				IFileOpenDialog* FileOpenDialog = static_cast<IFileOpenDialog*>(FileDialog.Get());

				TComPtr<IShellItemArray> Results;
				if (SUCCEEDED(FileOpenDialog->GetResults(&Results)))
				{
					DWORD NumResults = 0;
					Results->GetCount(&NumResults);
					for (DWORD ResultIndex = 0; ResultIndex < NumResults; ++ResultIndex)
					{
						TComPtr<IShellItem> Result;
						if (SUCCEEDED(Results->GetItemAt(ResultIndex, &Result)))
						{
							PWSTR pFilePath = nullptr;
							if (SUCCEEDED(Result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
							{
								AddOutFilename(pFilePath);
								::CoTaskMemFree(pFilePath);
							}
						}
					}
				}
			}
		}
	}
#endif

	UTexture2D* Texture=nullptr;
	if (OutFilenames.Num() > 0)
	{
		UE_LOG(LogIVFileHepler,Log,TEXT("Read image:%s"),*FPaths::ConvertRelativePathToFull(OutFilenames[0]))
		TArray<uint8> FileData;
		if (!FFileHelper::LoadFileToArray(FileData, *OutFilenames[0]))
		{
			return FString();
		}
		
		return  FBase64::Encode(FileData);
	}
	else
	{
		UE_LOG(LogIVFileHepler, Warning, TEXT("not valid texture"));
		return FString();
	}
}

bool UiVisualFileHeplerBPLibrary::Conv_TextureToBytes(UTexture2D* Texture, TArray<uint8>& OutBuffer,
                                                      EIVImageFormatBPType Format)
{
	if (!Texture || !Texture->IsValidLowLevel())
	{
		return false;
	}
	
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper((EImageFormat)Format);

	int32 Width = Texture->GetPlatformData()->Mips[0].SizeX;
	int32 Height = Texture->GetPlatformData()->Mips[0].SizeY;
	int32 DataLength = Width * Height * 4;

	void* TextureDataPointer = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_ONLY);

	ImageWrapper->SetRaw(TextureDataPointer, DataLength, Width, Height, ERGBFormat::BGRA, 8);
	
	OutBuffer = ImageWrapper->GetCompressed();

	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

	return true;
}

FString UiVisualFileHeplerBPLibrary::MakeTexture2Base64EncodeData(UTexture2D* Texture)
{
	if (!Texture)
	{
		UE_LOG(LogIVFileHepler, Warning, TEXT("Invalid texture"));
		return FString();
	}

	if (Texture->GetPlatformData() == nullptr)
	{
		//. maybe Dxt1 to PNG
		return FString();
	}

	if (Texture->GetPlatformData()->PixelFormat != PF_B8G8R8A8)
	{
		Texture = ConvertPixelFormatToB8G8R8A8(Texture);
	}

	const int32 Width = Texture->GetSizeX();
	const int32 Height = Texture->GetSizeY();

	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(Width * Height);
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];

#if WITH_EDITOR
	if (!Mip.BulkData.IsBulkDataLoaded())
	{
		Mip.BulkData.LoadBulkDataWithFileReader();
	}

#endif

	void* Data = nullptr;

	if (Mip.BulkData.IsLocked() == false)
	{

		Data = Mip.BulkData.Lock(LOCK_READ_ONLY);
	}
	else
	{
		Mip.BulkData.Unlock();
		Data = Mip.BulkData.Lock(LOCK_READ_ONLY);
	}
	if (Data)
	{
		FMemory::Memcpy(Pixels.GetData(), Data, Width * Height * sizeof(FColor));
		Mip.BulkData.Unlock();
	}
	else
	{
		UE_LOG(LogIVFileHepler, Warning, TEXT("Failed to lock texture data"));
		return FString();
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	ImageWrapper->SetRaw(Pixels.GetData(), Pixels.GetAllocatedSize(), Width, Height, ERGBFormat::BGRA, 8);
	const TArray64<uint8>& EncodedData = ImageWrapper->GetCompressed(100);

	FString Base64String;
	Base64String = FBase64::Encode(EncodedData.GetData(), EncodedData.Num());

	return Base64String;
}

UTexture2D* UiVisualFileHeplerBPLibrary::ConvertPixelFormatToB8G8R8A8(UTexture2D* SourceTexture)
{
	TextureCompressionSettings OldCompressionSettings = SourceTexture->CompressionSettings;
	bool OldSRGB = SourceTexture->SRGB;
	SourceTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
#if WITH_EDITOR
	SourceTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	SourceTexture->SRGB = false;
	SourceTexture->UpdateResource();

	int32 width = SourceTexture->GetPlatformData()->SizeX;
	int32 height = SourceTexture->GetPlatformData()->SizeY;

	const FColor* FormatedImageData = static_cast<const FColor*>(SourceTexture->GetPlatformData()->Mips[0].BulkData.LockReadOnly());
	SourceTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

	int32 Width = SourceTexture->GetPlatformData()->SizeX;
	int32 Height = SourceTexture->GetPlatformData()->SizeY;
	TArray<FColor> ColorArray;
	ColorArray.SetNumUninitialized(Width * Height);

	for (int32 X = 0; X < Width; X++)
	{
		for (int32 Y = 0; Y < Height; Y++)
		{
			FColor PixelColor = FormatedImageData[Y * Width + X];
			ColorArray[Y * Width + X] = PixelColor;
		}
	}

	if (ColorArray.Num() != Width * Height)
	{
		UE_LOG(LogIVFileHepler, Error, TEXT("ColorArray size does not match the specified width and height."));
		return nullptr;
	}

	UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	if (!NewTexture)
	{
		UE_LOG(LogIVFileHepler, Error, TEXT("Failed to create transient texture."));
		return nullptr;
	}

	FTexture2DMipMap& MipMap = NewTexture->GetPlatformData()->Mips[0];
	uint8* DestPixels = static_cast<uint8*>(MipMap.BulkData.Lock(LOCK_READ_WRITE));

	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++)
		{
			int32 Index = (y * Width + x);
			FColor Color = ColorArray[Index];
			reinterpret_cast<FColor*>(DestPixels)[Index] = Color;
		}
	}

	MipMap.BulkData.Unlock();

	NewTexture->UpdateResource();

	return NewTexture;
}

const FString UiVisualFileHeplerBPLibrary::QualifyFileExtension(const FString& Path, const FString& Name, const FString& Extension)
{
	if (IVCommon::HasEmptyParam(Path, Name, Extension))
	{
		UE_LOG(LogIVFileHepler, Error, TEXT("%s: Filepath, Filename or Extension is empty"), *FString(__func__));
		return FString();
	}

	const FString LocalPath = QualifyPath(Path);
	const FString LocalExtension = Extension.Contains(".") ? Extension : "." + Extension;

	FString LocalName = Name;
	if (!Name.Right(Name.Len() - LocalExtension.Len()).Contains(LocalExtension))
	{
		LocalName += LocalExtension;
	}

	FString QualifiedName = LocalPath + LocalName;
	FPaths::NormalizeFilename(QualifiedName);

	UE_LOG(LogIVFileHepler, Log, TEXT("%s: Qualified %s file path: %s"), *FString(__func__), *LocalExtension.ToUpper(), *QualifiedName);

	return QualifiedName;
}

const FString UiVisualFileHeplerBPLibrary::QualifyPath(const FString& Path)
{
	FString Output = Path;
	FPaths::NormalizeDirectoryName(Output);

	if (!Output.EndsWith("/") && !Output.EndsWith("\""))
	{
		Output += '/';
	}

	UE_LOG(LogIVFileHepler, Log, TEXT("%s: Qualified directory path: %s"), *FString(__func__), *Output);

	return Output;
}

const bool UiVisualFileHeplerBPLibrary::IsAudioDataValid(const TArray<uint8>& RawData)
{
	const bool bOutput = !IVCommon::HasEmptyParam(RawData);
	if (!bOutput)
	{
		UE_LOG(LogIVFileHepler, Error, TEXT("%s: Invalid audio data."), *FString(__func__));
	}

	return bOutput;
}

USoundWave* UiVisualFileHeplerBPLibrary::ConvertWavFileToSoundWave(const FString& FilePath, const FString& FileName, const FString& OutputModule, const FString& RelativeOutputDirectory, const FString& OutputAssetName)
{
	if (IVCommon::HasEmptyParam(FilePath, FileName))
	{
		UE_LOG(LogIVFileHepler, Error, TEXT("%s: Filepath or Filename is empty"), *FString(__func__));
		return nullptr;
	}
	
	if (const FString Full_FileName =QualifyWAVFileName(FilePath, FileName); IFileManager::Get().FileExists(*Full_FileName))
	{
#if PLATFORM_ANDROID
		if (!CheckAndroidPermission("android.permission.READ_EXTERNAL_STORAGE"))
		{
			return nullptr;
		}
#endif

		if (TArray<uint8> RawData; FFileHelper::LoadFileToArray(RawData, *Full_FileName))
		{
			UE_LOG(LogIVFileHepler, Display, TEXT("%s: Result: Success"), *FString(__func__));
			if (USoundWave* const SoundWave = ConvertAudioDataToSoundWave(RawData, OutputModule, RelativeOutputDirectory, OutputAssetName))
			{
#if WITH_EDITORONLY_DATA
				SoundWave->AssetImportData->Update(Full_FileName);
#endif
				return SoundWave;
			}
		}
		// else
		UE_LOG(LogIVFileHepler, Error, TEXT("%s: Result: Failed to load file '%s'"), *FString(__func__), *Full_FileName);
	}

	UE_LOG(LogIVFileHepler, Error, TEXT("%s: Result: Cannot find the specified file"), *FString(__func__));
	return nullptr;
}


