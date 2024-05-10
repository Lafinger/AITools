// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IVXunFeiSpeechTypes.generated.h"

// 实时语音识别
DECLARE_DYNAMIC_DELEGATE_TwoParams(FInitRealTimeSTTDelegate, bool, bInitResult, FString,InitRespMessage);
DECLARE_DYNAMIC_DELEGATE_OneParam(FRealTimeSTTNoTranslateDelegate, FString, OutSrcText);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FRealTimeSTTTranslateDelegate, FString, OutSrcText, FString, OutDstText);

// 流式语音识别
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStreamingSTTInitdDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStreamingSTTResponseDelegate, FString, OutResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStreamingSTTCompletedDelegate, FString, FinalResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStreamingSTTErrorDelegate, FString, ErrorMessage);

// 翻译
DECLARE_DYNAMIC_DELEGATE_TwoParams(FTranslateCompletedDelegate, bool, bIsSuccess, FString, FinalResult);



/**
  *讯飞翻译参数
  */
USTRUCT(BlueprintType)
struct IVXUNFEISPEECH_API FXunFeiTranslateOptions
{
	GENERATED_BODY()

//业务参数
	// /**
	// * @brief 在平台申请的appid信息
	// */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Translate")
	// FString HeaderAppId;

	/**
	* @brief 请求状态，固定取值为：3（一次传完）
	* 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Translate")
	int32 HeaderStatus = 3;

	/**
	* @brief 1、个性化术语资源id
	*		 2、在机器翻译控制台自定义（翻译术语热词格式为：原文本1|目标文本1，如：开放平台|KFPT，一个术语资源id支持定义多个术语，多个术语之间用换行符间隔）
	*		 3、请注意使用参数值和控制台自定义的值保持一致
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Translate")
	FString HeaderResId;

	/**
	* @brief 源语种
	*	中文			cn
	*	英语	        en	捷克语		cs	豪萨语		ha
	*	日语	        ja	罗马尼亚语	ro	匈牙利语		hu
	*	韩语	        ko	瑞典语		sv	斯瓦希里语	sw
	*	泰语	        th	荷兰语		nl	乌兹别克语	uz
	*	俄语	        ru	波兰语		pl	祖鲁语		zu
	*	保加利亚语	bg	阿拉伯语		ar	希腊语		el
	*	乌克兰语	    uk	波斯语		fa	希伯来语		he
	*	越南语	    vi	普什图语		ps	亚美尼亚语	hy
	*	马来语		ms	乌尔都语		ur	格鲁吉亚语	ka
	*	印尼语		id	印地语		hi	广东话		yue
	*	菲律宾语		tl	孟加拉语		bn	彝语			ii
	*	德语			de	外蒙语		nm	壮语			zua
	*	西班牙语		es	外哈语		kk	内蒙语		mn
	*	法语			fr	土耳其语		tr	内哈萨克语	kk
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Translate")
	FString ParameterFrom;

	/**
	* @brief 目标语种
	*	中文			cn
	*	英语	        en	捷克语		cs	豪萨语		ha
	*	日语	        ja	罗马尼亚语	ro	匈牙利语		hu
	*	韩语	        ko	瑞典语		sv	斯瓦希里语	sw
	*	泰语	        th	荷兰语		nl	乌兹别克语	uz
	*	俄语	        ru	波兰语		pl	祖鲁语		zu
	*	保加利亚语	bg	阿拉伯语		ar	希腊语		el
	*	乌克兰语	    uk	波斯语		fa	希伯来语		he
	*	越南语	    vi	普什图语		ps	亚美尼亚语	hy
	*	马来语		ms	乌尔都语		ur	格鲁吉亚语	ka
	*	印尼语		id	印地语		hi	广东话		yue
	*	菲律宾语		tl	孟加拉语		bn	彝语			ii
	*	德语			de	外蒙语		nm	壮语			zua
	*	西班牙语		es	外哈语		kk	内蒙语		mn
	*	法语			fr	土耳其语		tr	内哈萨克语	kk
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Translate")
	FString ParameterTo;

	// /**
	// * @brief 响应结果字段名，固定传{}即可，为保留字段
	// */ 
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Translate")
	// FString ParameterResult;

	/**
	* @brief 文本编码，如utf8
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Translate")
	FString PayloadEncoding = TEXT("utf8");

	/**
	* @brief 数据状态，固定取值为：3（一次传完）
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Translate")
	int32 PayloadStatus = 3;

	/**
	* @brief 待翻译文本的base64编码，字符要大于0且小于5000
	*/
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Translate")
	FString PayloadText;
	
	FString GenerateURL(const FString& Host, const FString& Path);

	FString GenerateRequireParams();
};

/**
  *讯飞流式语音听写参数
  */
USTRUCT(BlueprintType)
struct IVXUNFEISPEECH_API FXunFeiStreamingSTTOptions
{
	GENERATED_BODY()

//业务参数
	/**
	* @brief 语种
			zh_cn：中文（支持简单的英文识别）
			en_us：英文
			其他小语种：可到控制台-语音听写（流式版）-方言/语种处添加试用或购买，添加后会显示该小语种参数值，若未授权无法使用会报错11200。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	FString Language = TEXT("zh_cn");

	/**
	* @brief 应用领域
			iat：日常用语
			medical：医疗
			gov-seat-assistant：政务坐席助手
			seat-assistant：金融坐席助手
			gov-ansys：政务语音分析
			gov-nav：政务语音导航
			fin-nav：金融语音导航
			fin-ansys：金融语音分析
			注：除日常用语领域外其他领域若未授权无法使用，可到控制台-语音听写（流式版）-高级功能处添加试用或购买；若未授权无法使用会报错11200。
			坐席助手、语音导航、语音分析相关垂直领域仅适用于8k采样率的音频数据，另外三者的区别详见下方。
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	FString Domain = TEXT("iat");

	/**
	* @brief 方言，当前仅在language为中文时，支持方言选择。
			mandarin：中文普通话、其他语种
			其他方言：可到控制台-语音听写（流式版）-方言/语种处添加试用或购买，添加后会显示该方言参数值；方言若未授权无法使用会报错11200。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	FString Accent = TEXT("mandarin");

	/**
	* @brief 用于设置端点检测的静默时间，单位是毫秒。
			即静默多长时间后引擎认为音频结束。
			默认2000（小语种除外，小语种不设置该参数默认为未开启VAD）。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	int32 Vad_eos = 3000;

	/**
	* @brief （仅中文普通话支持）动态修正
			wpgs：开启流式结果返回功能
			注：该扩展功能若未授权无法使用，可到控制台-语音听写（流式版）-高级功能处免费开通；若未授权状态下设置该参数并不会报错，但不会生效。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	FString Dwa = TEXT("wpgs");

	/**
	* @brief （仅中文支持）领域个性化参数
			game：游戏
			health：健康
			shopping：购物
			trip：旅行
			注：该扩展功能若未授权无法使用，可到控制台-语音听写（流式版）-高级功能处添加试用或购买；若未授权状态下设置该参数并不会报错，但不会生效。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	FString Pd = TEXT("trip");

	/**
	* @brief （仅中文支持）是否开启标点符号添加
			1：开启（默认值）
			0：关闭
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	int32 Ptt = 1;

	/**
	* @brief （仅中文支持）字体
			zh-cn :简体中文（默认值）
			zh-hk :繁体香港
			注：该繁体功能若未授权无法使用，可到控制台-语音听写（流式版）-高级功能处免费开通；若未授权状态下设置为繁体并不会报错，但不会生效。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	FString Rlang = TEXT("zh-cn");

	/**
	* @brief 返回子句结果对应的起始和结束的端点帧偏移值。端点帧偏移值表示从音频开头起已过去的帧长度。
			0：关闭（默认值）
			1：开启
			开启后返回的结果中会增加data.result.vad字段，详见下方返回结果。
			注：若开通并使用了动态修正功能，则该功能无法使用。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	int32 Vinfo = 0;

	/**
	* @brief （中文普通话和日语支持）将返回结果的数字格式规则为阿拉伯数字格式，默认开启
			0：关闭
			1：开启
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	int32 Nunum = 1;

	/**
	* @brief speex音频帧长，仅在speex音频时使用
			1 当speex编码为标准开源speex编码时必须指定
			2 当speex编码为讯飞定制speex编码时不要设置
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	int32 Speex_Size = 10;

	/**
	* @brief 取值范围[1,5]，通过设置此参数，获取在发音相似时的句子多侯选结果。设置多候选会影响性能，响应时间延迟200ms左右。
			注：该扩展功能若未授权无法使用，可到控制台-语音听写（流式版）-高级功能处免费开通；若未授权状态下设置该参数并不会报错，但不会生效。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	int32 Nbest = 1;

	/**
	* @brief 取值范围[1,5]，通过设置此参数，获取在发音相似时的词语多侯选结果。设置多候选会影响性能，响应时间延迟200ms左右。
			注：该扩展功能若未授权无法使用，可到控制台-语音听写（流式版）-高级功能处免费开通；若未授权状态下设置该参数并不会报错，但不会生效。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	int32 Wbest = 1;
	
	
//业务数据流参数
	/**
	* @brief 音频的状态
			0 :第一帧音频
			1 :中间的音频
			2 :最后一帧音频，最后一帧必须要发送
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	int32 Status = 0;

	/**
	* @brief 音频的采样率支持16k和8k
			16k音频：audio/L16;rate=16000
			8k音频：audio/L16;rate=8000
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	FString Format = TEXT("audio/L16;rate=16000");

	/**
	* @brief 音频数据格式
			raw：原生音频（支持单声道的pcm）
			speex：speex压缩后的音频（8k）
			speex-wb：speex压缩后的音频（16k）
			请注意压缩前也必须是采样率16k或8k单声道的pcm。
			lame：mp3格式（仅中文普通话和英文支持，方言及小语种暂不支持）
			样例音频请参照音频样例
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	FString Encoding = TEXT("raw");

	/*音频内容，采用base64编码*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "IV XunFei Streaming STT")
	FString Audio;
	
	FString GenerateURL(const FString& Host, const FString& Path);

	FString GenerateRequireParams();
};

/**
  *讯飞实时语音转写参数
  */
USTRUCT(BlueprintType)
struct IVXUNFEISPEECH_API FXunFeiRealTimeSTTOptions
{
	GENERATED_BODY()

protected:

	/**
	 * 当前时间戳，从1970年1月1日0点0分0秒开始到现在的秒数
	 */
	UPROPERTY()
	FString TS;

	/**
	 * 加密数字签名（基于HMACSHA1算法）
	 */
	UPROPERTY()
	FString Signa;

public:

	/**
	 * 实时语音转写语种，不传默认为中文
	 * Example:语种类型：中文、中英混合识别：cn；英文：en；小语种及方言可到控制台-实时语音转写-方言/语种处添加，添加后会显示该方言/语种参数值。传参示例如："lang=en"若未授权无法使用会报错10110
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	FString Lang = TEXT("cn");

	/**
	 * 是否开启语音翻译，需控制台开通翻译功能
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	bool bEnableTranslate = false;


	/**
	 * normal表示普通翻译，默认值normal；
	 * Note: 注意：需控制台开通翻译功能
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT", meta = (EditConditionHides, EditCondition = "bEnableTranslate==false"))
	FString TransType = TEXT("normal");

	/**
	 * 翻译类型
	 * 策略1，转写的vad结果直接送去翻译；策略2，返回中间过程中的结果；策略3，按照结束性标点拆分转写结果请求翻译；建议使用策略2
	 * Note: 需控制台开通翻译功能
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT",meta = (EditConditionHides, EditCondition = "bEnableTranslate==false"))
	int32 TransStrategy = 2;


	/**
	 * 控制把源语言转换成什么类型的语言；
	 * Description:
	中文：cn
	英文：en
	日语：ja
	韩语：ko
	俄语：ru
	法语：fr
	西班牙语：es
	越南语：vi
	广东话：cn_cantonese
	 * Note: 请注意类似英文转成法语必须以中文为过渡语言，即英-中-法，暂不支持不含中文语种之间的直接转
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT",meta = (EditConditionHides, EditCondition = "bEnableTranslate==false"))
	FString TargetLang = TEXT("en");

	/**
	 *标点过滤控制，默认返回标点，punc=0会过滤结果中的标点
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	FString Punc = TEXT("None");

	/**
	 * 垂直领域个性化参数:
	法院: court
	教育: edu
	金融: finance
	医疗: medical
	科技: tech
	运营商: isp
	政府: gov
	电商: ecom
	军事: mil
	企业: com
	生活: life
	汽车: carcom
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	FString PD = TEXT("None");


	/**
	 * 远近场切换
	 * 不传此参数或传1代表远场，传2代表近场
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	int32 VadMdn = -1;

	/**
	 * 是否开角色分离，默认不开启，传2开启
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	int32 RoleType = -1;

	/**
	 *语言识别模式，默认为模式1中英文模式
	1：自动中英文模式
	2：中文模式，可能包含少量英文
	4：纯中文模式，不包含英文
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	int32 EngLangType = -1;
	
	FString GenerateRequireParams();

protected:

	FString GenerateSigna(const FString& InAppid, const FString& InTS, const FString& InAPIKey);

};


USTRUCT(BlueprintType)
struct FXunFeiRealTImeSTTRespInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	FString action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	FString code;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	FString data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	FString desc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IV XunFei Real Time STT")
	FString sid;
};



///////////////////////////讯飞星火///////////////////////////////////////

/**
 * 指定访问的领域,general指向V1.5版本,generalv2指向V2版本,generalv3指向V3版本 ,generalv3_5指向V3.5版本。注意：不同的取值对应的url也不一样！
 */
UENUM(BlueprintType)
enum class EDomainType:uint8
{
	general,
	generalv2,
	generalv3,
	generalv3_5
};

USTRUCT(BlueprintType)
struct  FSparkDeskOptions
{
	GENERATED_BODY()
	
public:

	/**指定访问的领域,general指向V1.5版本,generalv2指向V2版本,generalv3指向V3版本 。注意：不同的取值对应的url也不一样！*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "XunFeiSparkDest")
	EDomainType Domain;

	/**配置核采样阈值，默认值0.5，向上调整可以增加结果的随机程度*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "XunFeiSparkDest", meta = (ClampMin = 0, ClampMax = 1))
	float Temperature;
	
	/**模型回答的tokens的最大长度
	* V1.5取值为[1,4096]
	* V2.0取值为[1,8192]，默认为2048。
	* V3.0取值为[1,8192]，默认为2048。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "XunFeiSparkDest", meta = (ClampMin = 1, ClampMax = 8192))
	int32 MaxTokens;

	/**从k个候选中随机选择⼀个（⾮等概率）*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "XunFeiSparkDest", meta = (ClampMin = 1, ClampMax = 4))
	int32 TopK;
	
	/** 配置内容审核策略,strict表示严格审核策略；moderate表示中等审核策略；default表示默认的审核程度*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "XunFeiSparkDest")
	FString Auditing;

	FSparkDeskOptions()
		:Domain(EDomainType::generalv3)
		,Temperature(0.5f)
		,MaxTokens(2048)
		,TopK(4)
		,Auditing(TEXT("default"))
	{}

	FString GetStringDomain();
	FString GetVersion();
	void AddQueryField(TSharedPtr<FJsonObject> JsonObject,bool bImageChat);
	
};


USTRUCT(BlueprintType)
struct IVXUNFEISPEECH_API FXunFeiTTSOptions
{
	GENERATED_BODY()


public:
	/**
	* 音频编码，可选值：
	raw：未压缩的pcm
	lame：mp3 (当aue=lame时需传参sfl=1)
	speex-org-wb;7： 标准开源speex（for speex_wideband，即16k）数字代表指定压缩等级（默认等级为8）
	speex-org-nb;7： 标准开源speex（for speex_narrowband，即8k）数字代表指定压缩等级（默认等级为8）
	speex;7：压缩格式，压缩等级1~10，默认为7（8k讯飞定制speex）
	speex-wb;7：压缩格式，压缩等级1~10，默认为7（16k讯飞定制speex）
	NOTE:现阶段只支持raw
	 */
	UPROPERTY()
	FString aue = TEXT("raw");



	/**
	* 需要配合aue=lame使用，开启流式返回
	mp3格式音频
	取值：1 开启
	NOTE:现阶段不支持
	 */
	UPROPERTY()
	int32 sfl = -1;

	/**
	 * 音频采样率，可选值：
	audio/L16;rate=8000：合成8K 的音频
	audio/L16;rate=16000：合成16K 的音频
	 *
	 */
	UPROPERTY()
	FString auf = TEXT("audio/L16;rate=16000");
	

	/**
	 * 发音人，可选值：请到控制台添加试用或购买发音人，添加后即显示发音人参数值
	 *
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG iFylTek TTSG")
	FString vcn = TEXT("xiaoyan");

	/**
	 * 语速，可选值：[0-100]，默认为50
	 *
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta=(ClampMin=0,ClampMax=100), Category = "XG iFylTek TTSG")
	int32 speed = 50;

	/**
	 * 音量，可选值：[0-100]，默认为50
	 *
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta=(ClampMin=0,ClampMax=100), Category = "XG iFylTek TTSG")
	int32 volume = 100;

	/**
	 * 音高，可选值：[0-100]，默认为50
	 *
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0,ClampMax=100),Category = "XG iFylTek TTSG")
	int32 pitch = 50;

	/**
	* 合成音频的背景音
	0:无背景音（默认值）
	1:有背景音
	 *
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG iFylTek TTSG")
	int32 bgs = 0;


	/**
	* 文本编码格式
	GB2312
	GBK
	BIG5
	UNICODE(小语种必须使用UNICODE编码，合成的文本需使用utf16小端的编码方式，详见java示例demo)
	GB18030
	UTF8（小语种）
	 *
	 */
	UPROPERTY()
	FString tte = TEXT("UTF8");

	/**
	*设置英文发音方式：
	0：自动判断处理，如果不确定将按照英文词语拼写处理（缺省）
	1：所有英文按字母发音
	2：自动判断处理，如果不确定将按照字母朗读
	默认按英文单词发音
	 *
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG iFylTek TTSG")
	FString reg = TEXT("2");

	/**
	* 合成音频数字发音方式
	0：自动判断（默认值）
	1：完全数值
	2：完全字符串
	3：字符串优先st
	 *
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XG iFylTek TTSG")
	FString rdn = TEXT("0");

	FXunFeiTTSOptions();
	
	void AddQueryField(TSharedPtr<FJsonObject> JsonObject);
};


USTRUCT()
struct IVXUNFEISPEECH_API FXunFeiTTSRespInfoData
{
	GENERATED_BODY()

public:

	
	UPROPERTY()
	FString audio=TEXT("");

	UPROPERTY()
	int32 status = -1;

	UPROPERTY()
	FString ced=TEXT("");
};





USTRUCT()
struct IVXUNFEISPEECH_API FXGXunFeiTTSRespInfo
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	int32 code=-1;
	
	UPROPERTY()
	FString message=TEXT("");
	
	UPROPERTY()
	FXunFeiTTSRespInfoData data;
	
	UPROPERTY()
	FString sid;
	
};
