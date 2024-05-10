// Fill out your copyright notice in the Description page of Project Settings.


#include "IVXunFeiSpeechTypes.h"
#include"IVXunFeiSpeechSettings.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "hmac_sha256/hmac_sha256.h"

#define SHA256_HASH_SIZE 32

FString FXunFeiTranslateOptions::GenerateURL(const FString& Host, const FString& Path)
{
	FString AppID = UIVXunFeiSpeechSettings::Get()->TranslateParams.APPID;
	FString APIKey = UIVXunFeiSpeechSettings::Get()->TranslateParams.APIKey;
	FString APISecret = UIVXunFeiSpeechSettings::Get()->TranslateParams.APISecret;
	
	FString Date =  FDateTime::UtcNow().ToHttpDate();
	
	FString SignatureOrigin = FString("host: ") + Host + "\n";
	SignatureOrigin += FString("date: ") + Date + "\n";
	SignatureOrigin += FString("POST ") + Path + " HTTP/1.1";
	
	uint8 Source[SHA256_HASH_SIZE];
	uint32 SecretSize = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*APISecret));
	uint32 TmpSize = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*SignatureOrigin));
	uint32 Size = hmac_sha256(TCHAR_TO_UTF8(*APISecret), SecretSize, TCHAR_TO_UTF8(*SignatureOrigin), TmpSize, Source, SHA256_HASH_SIZE);

	if (Size == 0) {
		UE_LOG(LogTemp, Error, TEXT("hmac_sha256 failed"));
		return FString();
	}

	FString signature = FBase64::Encode(Source, SHA256_HASH_SIZE);

	FString Algorithm = "hmac-sha256";
	FString Authorization_Origin = FString::Format(TEXT("api_key=\"{0}\", algorithm=\"{1}\", headers=\"host date request-line\", signature=\"{2}\""), TArray<FStringFormatArg>({ APIKey, Algorithm, signature }));
	uint32 Length = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*Authorization_Origin));
	FString UnencodedString = FBase64::Encode((uint8*)TCHAR_TO_UTF8(*Authorization_Origin), Length);

	FString EncodeAuthorization = FGenericPlatformHttp::UrlEncode(UnencodedString);
	FString EncodeDate = FGenericPlatformHttp::UrlEncode(Date);
	FString Url = FString::Format(TEXT("{0}?authorization={1}&date={2}&host={3}"), TArray<FStringFormatArg>({ "https://"+Host+Path, EncodeAuthorization, EncodeDate, Host }));

	return Url;
}

FString FXunFeiTranslateOptions::GenerateRequireParams()
{
	FString AppID = UIVXunFeiSpeechSettings::Get()->TranslateParams.APPID;
	FString APIKey = UIVXunFeiSpeechSettings::Get()->TranslateParams.APIKey;
	FString APISecret = UIVXunFeiSpeechSettings::Get()->TranslateParams.APISecret;
	
	FString RequireParams = FString::Format(TEXT("{\"header\":{\"app_id\":\"{0}\",\"status\":{1},\"res_id\":\"{2}\"},\"parameter\":{\"its\":{\"from\":\"{3}\",\"to\":\"{4}\",\"result\":{}}},\"payload\": {\"input_data\":{\"encoding\":\"{5}\",\"status\":{6},\"text\":\"{7}\"}}}")
		, TArray<FStringFormatArg>({AppID, HeaderStatus, HeaderResId, ParameterFrom, ParameterTo, PayloadEncoding, PayloadStatus, PayloadText}));
	
	return RequireParams;
}

FString FXunFeiStreamingSTTOptions::GenerateURL(const FString& Host, const FString& Path)
{
	FString AppID = UIVXunFeiSpeechSettings::Get()->StreamingSTTParams.APPID;
	FString APIKey = UIVXunFeiSpeechSettings::Get()->StreamingSTTParams.APIKey;
	FString APISecret = UIVXunFeiSpeechSettings::Get()->StreamingSTTParams.APISecret;
	
	FString Date =  FDateTime::UtcNow().ToHttpDate();
	
	FString SignatureOrigin = FString("host: ") + Host + "\n";
	SignatureOrigin += FString("date: ") + Date + "\n";
	SignatureOrigin += FString("GET ") + Path + " HTTP/1.1";
	
	uint8 Source[SHA256_HASH_SIZE];
	uint32 SecretSize = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*APISecret));
	uint32 TmpSize = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*SignatureOrigin));
	uint32 Size = hmac_sha256(TCHAR_TO_UTF8(*APISecret), SecretSize, TCHAR_TO_UTF8(*SignatureOrigin), TmpSize, Source, SHA256_HASH_SIZE);

	if (Size == 0) {
		UE_LOG(LogTemp, Error, TEXT("hmac_sha256 failed"));
		return FString();
	}

	FString signature = FBase64::Encode(Source, SHA256_HASH_SIZE);

	FString Algorithm = "hmac-sha256";
	FString Authorization_Origin = FString::Format(TEXT("api_key=\"{0}\", algorithm=\"{1}\", headers=\"host date request-line\", signature=\"{2}\""), TArray<FStringFormatArg>({ APIKey, Algorithm, signature }));
	uint32 Length = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*Authorization_Origin));
	FString UnencodedString = FBase64::Encode((uint8*)TCHAR_TO_UTF8(*Authorization_Origin), Length);

	FString EncodeAuthorization = FGenericPlatformHttp::UrlEncode(UnencodedString);
	FString EncodeDate = FGenericPlatformHttp::UrlEncode(Date);
	FString Url = FString::Format(TEXT("{0}?authorization={1}&date={2}&host={3}"), TArray<FStringFormatArg>({ "wss://"+Host+Path, EncodeAuthorization, EncodeDate, Host }));

	return Url;
}

FString FXunFeiStreamingSTTOptions::GenerateRequireParams()
{
	FString AppID = UIVXunFeiSpeechSettings::Get()->StreamingSTTParams.APPID;
	FString APIKey = UIVXunFeiSpeechSettings::Get()->StreamingSTTParams.APIKey;
	FString APISecret = UIVXunFeiSpeechSettings::Get()->StreamingSTTParams.APISecret;


	FString RequireParams = FString::Format(TEXT("{\"common\":{\"app_id\":\"{0}\"},\"business\":{\"language\":\"{1}\",\"domain\":\"{2}\",\"accent\": \"{3}\",\"vad_eos\":{4},\"dwa\":\"{5}\",\"pd\":\"{6}\",\"ptt\":{7},\"rlang\":\"{8}\",\"vinfo\":{9},\"nunum\":{10},\"speex_size\":{11},\"nbest\":{12},\"wbest\":{13}},\"data\":{\"status\":{14},\"format\":\"{15}\",\"encoding\": \"{16}\",\"audio\":\"{17}\"}}")
		, TArray<FStringFormatArg>({AppID, Language, Domain, Accent, Vad_eos, Dwa, Pd, Ptt, Rlang, Vinfo, Nunum, Speex_Size, Nbest, Wbest, Status, Format, Encoding, Audio}));
	
	return RequireParams;
}

FString FXunFeiRealTimeSTTOptions::GenerateRequireParams()
{
	FString RetStr=TEXT("");

	FString AppID = UIVXunFeiSpeechSettings::Get()->RealTimeSTTParams.APPID;
	FString APIKey = UIVXunFeiSpeechSettings::Get()->RealTimeSTTParams.APIKey;
	TS = FString::FromInt(FDateTime::Now().UtcNow().ToUnixTimestamp());

	Signa = GenerateSigna(AppID, TS, APIKey);

	RetStr += TEXT("appid=") + AppID;
	RetStr += TEXT("&ts=") + TS;
	RetStr += TEXT("&signa=") + Signa;

	if (!Lang.IsEmpty())
	{
		RetStr += TEXT("&lang=") + Lang;
	}

	if (bEnableTranslate)
	{
		RetStr += TEXT("&transType=") + TransType;
		RetStr += TEXT("&transStrategy=") + FString::FromInt(TransStrategy);
		RetStr += TEXT("&targetLang=") + TargetLang;
	}

	if (!Punc.IsEmpty()&&!Punc.Equals(TEXT("None")))
	{
		RetStr+= TEXT("&punc=") + Punc;
	}


	if (!PD.IsEmpty()&& !PD.Equals(TEXT("None")))
	{
		RetStr += TEXT("&pd=") + PD;
	}

	if (VadMdn!=-1)
	{
		RetStr += TEXT("&vadMdn=") + FString::FromInt(VadMdn);
	}


	if (RoleType != -1)
	{
		RetStr += TEXT("&roleType=") + FString::FromInt(RoleType);
	}



	if (EngLangType != -1)
	{
		RetStr += TEXT("&engLangType=") + FString::FromInt(EngLangType);
	}

	return RetStr;
}

FString FXunFeiRealTimeSTTOptions::GenerateSigna(const FString& InAppid, const FString& InTS, const FString& InAPIKey)
{
	FString BaseString = InAppid+ InTS;
	FString MD5BaseString = FMD5::HashAnsiString(*BaseString);

	FSHAHash Hash;
	FSHA1 SHA1;
	SHA1.HMACBuffer(TCHAR_TO_ANSI(*InAPIKey), InAPIKey.Len(), TCHAR_TO_ANSI(*MD5BaseString), MD5BaseString.Len(), Hash.Hash);

	FString Signalocal = FBase64::Encode(Hash.Hash, 20);

	Signalocal.ReplaceInline(TEXT("%"), TEXT("%25"));
	Signalocal.ReplaceInline(TEXT("+"), TEXT("%2B"));
	Signalocal.ReplaceInline(TEXT(" "), TEXT("%20"));
	Signalocal.ReplaceInline(TEXT("/"), TEXT("%2F"));
	Signalocal.ReplaceInline(TEXT("?"), TEXT("%3F"));
	Signalocal.ReplaceInline(TEXT("#"), TEXT("%23"));
	Signalocal.ReplaceInline(TEXT("&"), TEXT("%26"));
	Signalocal.ReplaceInline(TEXT("="), TEXT("%3D"));

	return Signalocal;
}


FString FSparkDeskOptions::GetStringDomain()
{
	switch (Domain)
	{
	case EDomainType::general:
		return TEXT("general");
	case EDomainType::generalv2:
		return TEXT("generalv2");
	case EDomainType::generalv3:
		return TEXT("generalv3");
	case EDomainType::generalv3_5:
		return TEXT("generalv3.5");
	}

	return FString();
}

FString FSparkDeskOptions::GetVersion()
{
	switch (Domain)
	{
	case EDomainType::general:
		return TEXT("v1.1");
	case EDomainType::generalv2:
		return TEXT("v2.1");
	case EDomainType::generalv3:
		return TEXT("v3.1");
	case EDomainType::generalv3_5:
		return TEXT("v3.5");
	}

	return FString();
}

void FSparkDeskOptions::AddQueryField(TSharedPtr<FJsonObject> JsonObject,bool bImageChat)
{
	if(bImageChat)
	{
		JsonObject->SetStringField("domain","general");
	}
	else
	{
		JsonObject->SetStringField("domain",GetStringDomain());
	}
	
	JsonObject->SetNumberField("temperature",Temperature);
	JsonObject->SetNumberField("max_tokens",MaxTokens);
	JsonObject->SetNumberField("top_k",TopK);
}

FXunFeiTTSOptions::FXunFeiTTSOptions()
{
}

void FXunFeiTTSOptions::AddQueryField(TSharedPtr<FJsonObject> JsonObject)
{
	JsonObject->SetStringField("aue",aue);
	JsonObject->SetNumberField("sfl",sfl);
	JsonObject->SetStringField("auf",auf);
	JsonObject->SetStringField("vcn",vcn);
	JsonObject->SetNumberField("speed",speed);
	JsonObject->SetNumberField("volume",volume);
	JsonObject->SetNumberField("bgs",bgs);
	JsonObject->SetNumberField("sfl",sfl);
	JsonObject->SetStringField("tte",tte);
	JsonObject->SetStringField("reg",reg);
	JsonObject->SetStringField("rdn",rdn);
	
}
