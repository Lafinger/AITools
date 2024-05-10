// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/IVDeveloperSettings.h"
#include "Engine/DeveloperSettings.h"
#include "ChatModelsSettings.generated.h"

UENUM(BlueprintType)
enum class EChatModelType : uint8
{
	SparkModel UMETA(DisplayName="讯飞 星火大模型"),
	ERNIEBot UMETA(DisplayName="百度 文心一言大模型"),
	TongYiQianWen UMETA(DisplayName="阿里 通义千问大模型"),
	TuringNLG UMETA(DisplayName="微软 Turing-NLG大模型"),
	ChatGPT UMETA(DisplayName="OpenAI ChatGPT大模型"),
	Dify UMETA(DisplayName="Dify大模型集成平台")
};

// --------------------------------------------------目前该设置没作用--------------------------------------------------
/**
 * 指定访问的领域,general指向V1.5版本,generalv2指向V2版本,generalv3指向V3版本 。注意：不同的取值对应的url也不一样！
 */
UENUM(BlueprintType)
enum class ESparkDomainType : uint8
{
	general UMETA(DisplayName="大模型V1版本"),
	generalv2 UMETA(DisplayName="大模型V2版本"),
	generalv3 UMETA(DisplayName="大模型V3版本")
};

USTRUCT(BlueprintType)
struct FSparkModelSetting
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, config, Category = "Spark Model Setting")
	FString APPID;

	UPROPERTY(EditAnywhere, config, Category = "Spark Model Setting")
	FString APISecret;

	UPROPERTY(EditAnywhere, config, Category = "Spark Model Setting")
	FString APIKey;
	
	/** 指定访问的领域,general指向V1.5版本,generalv2指向V2版本,generalv3指向V3版本 。注意：不同的取值对应的url也不一样！ */
	UPROPERTY(EditAnywhere, config, Category = "Spark Model Setting")
	ESparkDomainType Domain;

	/** 配置核采样阈值，默认值0.5，向上调整可以增加结果的随机程度 */
	UPROPERTY(EditAnywhere, config, Category = "Spark Model Setting", meta = (ClampMin = 0, ClampMax = 1))
	float Temperature;
	
	/**
	* 模型回答的tokens的最大长度
	* V1.5取值为[1,4096]
	* V2.0取值为[1,8192]，默认为2048。
	* V3.0取值为[1,8192]，默认为2048。
	*/
	UPROPERTY(EditAnywhere, config, Category = "Spark Model Setting", meta = (ClampMin = 1, ClampMax = 8192))
	int32 MaxTokens;

	/** 从k个候选中随机选择⼀个（⾮等概率） */
	UPROPERTY(EditAnywhere, config, Category = "Spark Model Setting", meta = (ClampMin = 1, ClampMax = 4))
	int32 TopK;
	
	/** 配置内容审核策略,strict表示严格审核策略；moderate表示中等审核策略；default表示默认的审核程度 */
	UPROPERTY(EditAnywhere, config, Category = "Spark Model Setting")
	FString Auditing;

	FSparkModelSetting()
		:Domain(ESparkDomainType::generalv3)
		,Temperature(0.5f)
		,MaxTokens(2048)
		,TopK(4)
		,Auditing(TEXT("default"))
	{}

	FString GetStringDomain();
	FString GetVersion();
	void AddQueryField(TSharedPtr<FJsonObject> JsonObject,bool bImageChat);
	
};
// --------------------------------------------------目前该设置没作用--------------------------------------------------

UENUM(BlueprintType)
enum class EDifyModelType : uint8
{
	Spark UMETA(DisplayName="讯飞 星火大模型"),
	ERNIEBot UMETA(DisplayName="百度 文心一言大模型"),
	TongYiQianWen UMETA(DisplayName="阿里 通义千问大模型"),
	TuringNLG UMETA(DisplayName="微软 Turing-NLG大模型"),
	ChatGPT UMETA(DisplayName="OpenAI ChatGPT大模型"),
	GLM UMETA(DisplayName="智普 轻言AI大模型")
};

USTRUCT(BlueprintType)
struct FDifySetting
{
	GENERATED_BODY()
	
public:
	/**
	 * 用于通过"APIKey"和"Secrekey"获取的"Token"的URL
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category = "DifySetting")
	FString URL;
	
	/**
	 * 用于鉴权
	 *  Authorization: Bearer {API_KEY}
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category = "DifySetting")
	TMap<EDifyModelType, FString> APIKeys;

	/**
	 * 可以在Dify平台看到用户的访问记录
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category = "DifySetting")
	FString User;

	FDifySetting()
		: URL("https://api.dify.ai/v1/chat-messages")
		, APIKeys({{EDifyModelType::Spark, "app-ThROteEOp5gkbYPXFIJeEulK"}, {EDifyModelType::GLM, "app-sY19um3tr6O824d3dIMDNYc9"}, {EDifyModelType::ERNIEBot, "app-1A10O6NLB323I14MaSNqN3kl"}, {EDifyModelType::ChatGPT, "app-QMWXg0pnCVf5F49IM9KbzX4S"}})
		, User("Other")
	{
		
	}
};

/**
 * 继承了UIVDeveloperSettings，拥有将配置信息序列化成Json文件的能力，Json文件路径为：{项目根目录}/Config/ProjectJsonConfig/*。
 */
UCLASS(config = ChatModels, defaultconfig, meta = (DisplayName = "Chat Models Settings"))
class CHATMODELS_API UChatModelsSettings : public UIVDeveloperSettings
{
	GENERATED_BODY()
	
public:
	/** Gets the settings container name for the settings, either Project or Editor */
	virtual FName GetContainerName() const override
	{
		return FName("Project");
	}
	/** Gets the category for the settings, some high level grouping like, Editor, Engine, Game...etc. */
	virtual FName GetCategoryName() const override
	{

		return FName("ChatModels");
	}
	/** The unique name for your section of settings, uses the class's FName. */
	virtual FName GetSectionName() const override
	{
		return FName("Settings");
	}

public:
	/**
	* 目前只支持Dify
	*/
	UPROPERTY(EditAnywhere, config, Category = Settings)
	EChatModelType ChatModelType = EChatModelType::Dify;

	/**
	* 目前这个项目配置没用，因为与IVXunFeiSpeech里的大模型发生冲突
	*/
	UPROPERTY(EditAnywhere, config, Category = Settings)
	FSparkModelSetting SparkModelSetting;

	UPROPERTY(EditAnywhere, config, Category = Settings)
	FDifySetting DifySetting;
	
	static UChatModelsSettings* Get();
};
