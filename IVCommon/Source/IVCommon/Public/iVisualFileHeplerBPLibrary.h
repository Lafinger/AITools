// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "iVisualFileHeplerBPLibrary.generated.h"

/** Wrapper for EImageFormat::Type for BP */
UENUM()
enum class EIVImageFormatBPType : uint8
{
	/** 无效或无法识别的格式。 */
	Invalid = 254,
	
	PNG = 0,
	
	JPEG,

	/** 单通道 JPEG。 */
	GrayscaleJPEG,

	/** Windows Bitmap. */
	BMP,

	/** Windows 图标资源。 */
	ICO,

	/** OpenEXR (HDR) 图像文件格式。 */
	EXR,

	/** Mac icon. */
	ICNS
};

USTRUCT(BlueprintType)
struct FProjectPath
{
	GENERATED_BODY();
public:
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Directory;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Config;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Content;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Intermediate;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Log;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Mods;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Plugins;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Saved;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString User;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString PersistentDownload;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString PlatformExtensions;
};

USTRUCT(BlueprintType)
struct FEnginePath
{
	GENERATED_BODY();
public:
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Directory;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Config;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Content;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Intermediate;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Plugins;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString Saved;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString User;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString DefaultLayout;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString PlatformExtensions;
	
	UPROPERTY(BlueprintReadOnly, Category = "iVisual|FileHepler")
	FString UserLayout;
};

/**通用线程/任务工作流的 wrappers。 在线程上运行后台任务，通过游戏线程上的任务回调*/
class IVCOMMON_API FIVLambdaRunnable
{
public:
	/**
	*	在后台线程上运行传递的 lambda，每次调用新线程
	*/
	static void RunLambdaOnBackGroundThread(TFunction< void()> InFunction);

	/**
	*	在后台线程池上运行传递的 lambda
	*/
	static void RunLambdaOnBackGroundThreadPool(TFunction< void()> InFunction);

	/**
	*	通过task graph system在游戏线程上运行一个简短的 lambda
	*/
	static FGraphEventRef RunShortLambdaOnGameThread(TFunction< void()> InFunction);

	/**
	*	通过task graph system在后台线程上运行一个简短的 lambda
	*/
	static FGraphEventRef RunShortLambdaOnBackGroundTask(TFunction< void()> InFunction);

	/**
	* 在回调游戏线程之前运行一个空闲线程一段时间。
	* 由于上下文成本，建议持续时间 >0.1 秒。
	*/
	static void SetTimeout(TFunction<void()>OnDone, float DurationInSec, bool bCallbackOnGameThread = true);
};
UCLASS()
class IVCOMMON_API UiVisualFileHeplerBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/**获取引擎路径*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FEnginePath GetEngineDirectories();

	/**获取项目路径*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FProjectPath GetProjectDirectories();

	/**获取项目路径*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FString GetProjectDir();

	/**获取项目 config 路径*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FString GetProjectConfigDir();

	/**获取项目 Content 路径*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FString GetProjectContentDir();

	/**获取项目 Log 路径*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FString GetProjectLogDir();

	/**获取项目 mods 路径*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FString GetProjectModsDir();

	/**获取项目 Plugins 路径*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FString GetProjectPluginsDir();

	/**获取项目 Saved 路径*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FString GetProjectSavedDir();

	/**获取项目 User 路径*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FString GetProjectUserDir();

	/**获取跨游戏会话保存下载数据的目录*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FString GetProjectPersistentDownloadDir();

	/**获取项目平台扩展所在的目录*/
	UFUNCTION(BlueprintPure,Category = "iVisual|FileHepler")
	static FString GetProjectPlatformExtensionsDir();

	/**
	*获取插件内Extras的文件夹
	*
	*@param  Plugin 插件名
	*@param  FilenNmae 文件夹名
	*/
	UFUNCTION(BlueprintPure,Category="iVisual|FileHepler")
	static FString GetExtrasDir(const FString Plugin,const FString FileName);

	/**获取 extras 数据路径
	*@param  Plugin 插件名
	 */
	UFUNCTION(BlueprintPure,Category="iVisual|FileHepler")
	static FString GetExtrasDataDir(const FString Plugin);

	/**获取sttras exe路径
	 *@param  Plugin 插件名
	 */
	UFUNCTION(BlueprintPure,Category="iVisual|FileHepler")
	static FString GetExtrasExeDir(const FString Plugin);

	/**获取image路径
	 *@param  Plugin 插件名
	 */
	UFUNCTION(BlueprintPure,Category="iVisual|FileHepler")
	static FString GetExtrasImageDir(const FString Plugin);

	/**获取Actor配置的保存路径*/
	UFUNCTION(BlueprintPure,Category="iVisual|FileHepler")
	static FString GetIVActorJsonConfigDir();

	/**获取项目配置的保存路径*/
	UFUNCTION(BlueprintPure,Category="iVisual|FileHepler")
	static FString GetIVProjectJsonConfigDir();
	
	/**将array of bytes保存到指定目录的文件中
	*@param  Bytes 输出Bytes
	*@param  Directory 文件路径
	*@param  FileName 文件名
	*@param  bLogSave 是否打印日志
	*/
	UFUNCTION(BlueprintCallable, Category = "iVisual|FileHepler")
	static bool SaveBytesToFile(const TArray<uint8>& Bytes, const FString& Directory, const FString& FileName, bool bLogSave = false);

	/**从指定目录的文件中读取bytes */
	UFUNCTION(BlueprintCallable, Category = "iVisual|FileHepler")
	bool ReadBytesFromFile(const FString& Directory, const FString& FileName, TArray<uint8>& OutBytes);


	/**使用自动检测将字节转换为 UTexture2D - 已优化，但仍可能影响性能*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Texture2D (Bytes)", BlueprintAutocast), Category = "iVisual|FileHepler")
	static UTexture2D* Conv_BytesToTexture(const TArray<uint8>& InBytes);

	/**
	 * @加载本地图片
	 * @param ImagePath 图片路径，LocalImage://路径并不完整，需要补充完整，默认路径只放入iVisual插件对应Extras文件夹有用
	 * @return 图片对象
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual|FileHepler")
	static UTexture2D* LoadImageFromFile(FString ImagePath= TEXT("LocalImage://"));

	UFUNCTION(BlueprintCallable, Category = "iVisual|FileHepler")
	static UTexture2D* LoadImageFromExplorer(const FString& DefaultPath);

	UFUNCTION(BlueprintCallable, Category = "iVisual|FileHepler")
	static FString LoadImageFromExplorerToBase64(const FString& DefaultPath);
	
	/** 将 UTexture2D 转换为给定格式的字节 - 可能会影响性能*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Bytes (Texture2D)", BlueprintAutocast), Category = "iVisual|FileHepler")
	static bool Conv_TextureToBytes(UTexture2D* Texture, TArray<uint8>& OutBuffer, EIVImageFormatBPType Format = EIVImageFormatBPType::PNG);

	UFUNCTION(BlueprintPure, Category = "iVisual|FileHepler")
	static  FString MakeTexture2Base64EncodeData(UTexture2D* Texture);

	static UTexture2D* ConvertPixelFormatToB8G8R8A8(UTexture2D* SourceTexture);

	/**限定给定文件的扩展名 */
	UFUNCTION(BlueprintPure, Category = "iVisual|FileHepler")
	static const FString QualifyFileExtension(const FString& Path, const FString& Name, const FString& Extension);

	/* 将 WAV 文件路径 + 名称限定为单个字符串，例如 Full/File/Path/Filename.wav */
	UFUNCTION(BlueprintPure, Category = "iVisual | FileHepler|Audio", meta = (DisplayName = "Qualify WAV File Path"))
		static const FString QualifyWAVFileName(const FString& Path, const FString& Name)
	{
		return QualifyFileExtension(Path, Name, "wav");
	}
	
	/** 将路径字符串限定为单个字符串的辅助函数，例如 Full/File/Path/ */
	UFUNCTION(BlueprintPure, Category = "iVisual|FileHepler")
	static const FString QualifyPath(const FString& Path);

	/* 检查音频数据是否有效 */
	UFUNCTION(BlueprintPure, Category = "iVisual | Audio")
		static const bool IsAudioDataValid(const TArray<uint8>& RawData);

	/**
	 * 将wav音频转换为USoundWave
	 * @param FilePath 文件路径
	 * @param FileName 音频名
	 * @param OutputModule 将用于保存内容生成的模块名，如：Game
	 * @param RelativeOutputDirectory 保存目录
	 * @param OutputAssetName 保存名称
	 * @return
	 */
	UFUNCTION(BlueprintCallable, Category = "iVisual | FileHepler|Audio", Meta = (DisplayName = "Convert .wav file to USoundWave"))
		static USoundWave* ConvertWavFileToSoundWave(const FString& FilePath, const FString& FileName, const FString& OutputModule = "", const FString& RelativeOutputDirectory = "", const FString& OutputAssetName = "");


	/**
 * 将TArray<uint8>音频转换为USoundWave
 * @param RawData buffer
 * @param OutputModule 将用于保存内容生成的模块名，如：Game
 * @param RelativeOutputDirectory 保存目录
 * @param OutputAssetName 保存名称
 * @return
 */
	UFUNCTION(BlueprintCallable, Category = "iVisual | Audio")
		static USoundWave* ConvertAudioDataToSoundWave(const TArray<uint8>& RawData, const FString& OutputModule = "", const FString& RelativeOutputDirectory = "", const FString& OutputAssetName = "");

public:
	static bool LoadProc(FString Path,FString CommandLineArguments,bool bHiddenWindow,bool bLogCatch,FString& OutProcessID);
	static bool UnLoadProc(FString ProcessID);
	static void UnLoadProcList();

private:
	static TMap<FString, TSharedPtr< class FMonitoredProcess>> CurrentProcRegistry;

	static void OutputProcLog(FString Log,FString ProcPath);
};


namespace IVCommon
{
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
			return Arg1.IsEmpty();
		}
	}

	template<typename Ty, typename ...Args>
	constexpr const bool HasEmptyParam(const Ty& Arg1, Args&& ...args)
	{
		return HasEmptyParam(Arg1) || HasEmptyParam(std::forward<Args>(args)...);
	}

}