// Copyright 2023 ARIT All Rights Reserved.


#include "Json/IVJsonLibrary.h"

#include "IVCommon.h"
#include "Json/IVJsonValue.h"
#include "Kismet/KismetStringLibrary.h"
#include "iVisualFileHeplerBPLibrary.h"
#include "Misc/Base64.h"

FString UIVJsonLibrary::PercentEncode(const FString& Source)
{
	
	FString OutText = Source;

	OutText = OutText.Replace(TEXT(" "), TEXT("%20"));
	OutText = OutText.Replace(TEXT("!"), TEXT("%21"));
	OutText = OutText.Replace(TEXT("\""), TEXT("%22"));
	OutText = OutText.Replace(TEXT("#"), TEXT("%23"));
	OutText = OutText.Replace(TEXT("$"), TEXT("%24"));
	OutText = OutText.Replace(TEXT("&"), TEXT("%26"));
	OutText = OutText.Replace(TEXT("'"), TEXT("%27"));
	OutText = OutText.Replace(TEXT("("), TEXT("%28"));
	OutText = OutText.Replace(TEXT(")"), TEXT("%29"));
	OutText = OutText.Replace(TEXT("*"), TEXT("%2A"));
	OutText = OutText.Replace(TEXT("+"), TEXT("%2B"));
	OutText = OutText.Replace(TEXT(","), TEXT("%2C"));
	OutText = OutText.Replace(TEXT("/"), TEXT("%2F"));
	OutText = OutText.Replace(TEXT(":"), TEXT("%3A"));
	OutText = OutText.Replace(TEXT(";"), TEXT("%3B"));
	OutText = OutText.Replace(TEXT("="), TEXT("%3D"));
	OutText = OutText.Replace(TEXT("?"), TEXT("%3F"));
	OutText = OutText.Replace(TEXT("@"), TEXT("%40"));
	OutText = OutText.Replace(TEXT("["), TEXT("%5B"));
	OutText = OutText.Replace(TEXT("]"), TEXT("%5D"));
	OutText = OutText.Replace(TEXT("{"), TEXT("%7B"));
	OutText = OutText.Replace(TEXT("}"), TEXT("%7D"));

	return OutText;
}

FString UIVJsonLibrary::Base64Encode(const FString& Source)
{
	return FBase64::Encode(Source);
}

FString UIVJsonLibrary::Base64EncodeBytes(const TArray<uint8>& Source)
{
	return FBase64::Encode(Source);
}

bool UIVJsonLibrary::Base64Decode(const FString& Source, FString& Dest)
{
	return FBase64::Decode(Source, Dest);
}

bool UIVJsonLibrary::Base64DecodeBytes(const FString& Source, TArray<uint8>& Dest)
{
	return FBase64::Decode(Source, Dest);
}

bool UIVJsonLibrary::StringToJsonValueArray(const FString& JsonString, TArray<UIVJsonValue*>& OutJsonValueArray)
{
	TArray < TSharedPtr<FJsonValue>> RawJsonValueArray;
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(*JsonString);
	FJsonSerializer::Deserialize(Reader, RawJsonValueArray);

	for (auto Value : RawJsonValueArray)
	{
		auto SJsonValue = NewObject<UIVJsonValue>();
		SJsonValue->SetRootValue(Value);
		OutJsonValueArray.Add(SJsonValue);
	}

	return OutJsonValueArray.Num() > 0;
}

UIVJsonValue* UIVJsonLibrary::Conv_ArrayToJsonValue(const TArray<UIVJsonValue*>& InArray)
{
	return UIVJsonValue::ConstructJsonValueArray(InArray);
}

UIVJsonValue* UIVJsonLibrary::Conv_JsonObjectToJsonValue(UIVJsonObject* InObject)
{
	return UIVJsonValue::ConstructJsonValueObject(InObject);
}

UIVJsonValue* UIVJsonLibrary::Conv_BytesToJsonValue(const TArray<uint8>& InBytes)
{
	return UIVJsonValue::ConstructJsonValueBinary( InBytes);
}

UIVJsonValue* UIVJsonLibrary::Conv_StringToJsonValue(const FString& InString)
{
	return UIVJsonValue::ConstructJsonValueString(InString);
}

UIVJsonValue* UIVJsonLibrary::Conv_IntToJsonValue(int32 InInt)
{
	TSharedPtr<FJsonValue> NewVal = MakeShareable(new FJsonValueNumber(InInt));

	UIVJsonValue* NewValue = NewObject<UIVJsonValue>();
	NewValue->SetRootValue(NewVal);

	return NewValue;
}

UIVJsonValue* UIVJsonLibrary::Conv_FloatToJsonValue(float InFloat)
{
	return UIVJsonValue::ConstructJsonValueNumber(InFloat);
}

UIVJsonValue* UIVJsonLibrary::Conv_BoolToJsonValue(bool InBool)
{
	return UIVJsonValue::ConstructJsonValueBool(InBool);
}

FString UIVJsonLibrary::Conv_JsonValueToString(UIVJsonValue* InValue)
{
	if (InValue)
	{
		return InValue->AsString();
	}

	return TEXT("");
}

int32 UIVJsonLibrary::Conv_JsonValueToInt(UIVJsonValue* InValue)
{
	
	if(InValue)
	{
		return (int32)InValue->AsNumber();
	}

	return 0;
}

float UIVJsonLibrary::Conv_JsonValueToFloat(UIVJsonValue* InValue)
{
	if (InValue)
	{
		return InValue->AsNumber();
	}

	return 0.f;
}

bool UIVJsonLibrary::Conv_JsonValueToBool(UIVJsonValue* InValue)
{
	if (InValue)
	{
		return InValue->AsBool();
	}

	return false;
}

TArray<uint8> UIVJsonLibrary::Conv_JsonValueToBytes(UIVJsonValue* InValue)
{
	if (InValue)
	{
		return InValue->AsBinary();
	}

	return TArray<uint8>();
}

/**FString UIVJsonLibrary::Conv_JsonObjectToString(UIVJsonObject* InObject)
{
	if(InObject)
	{
		return InObject->EncodeJson();
	}

	return "";
}*/

UIVJsonObject* UIVJsonLibrary::ConvString_ToJsonObject(const FString& InString)
{
	TSharedPtr<FJsonObject> JsonObj;
	UIVJsonObject* Json=NewObject<UIVJsonObject>();
	
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(*InString);
	if (FJsonSerializer::Deserialize(Reader, JsonObj) && JsonObj.IsValid())
	{
		Json->SetRootObject(JsonObj);
		return Json;
	}
	
	UE_LOG(LogIVJson, Error, TEXT("Json decoding failed for: %s"), *InString);
	return  Json;
}

UIVJsonObject* UIVJsonLibrary::Conv_JsonValueToJsonObject(UIVJsonValue* InValue)
{
	if(InValue)
	{
		return InValue->AsObject();
	}

	return nullptr;
}

bool UIVJsonLibrary::LoadJsonFromFile(UIVJsonObject*& JsonObject,FString FilePath/**=TEXT("LocalData://")*/ )
{
	JsonObject=NewObject<UIVJsonObject>();
	TSharedPtr<FJsonObject> RootJson;
	if(!DeserializeJsonFromFile(FilePath,RootJson))
	{
		return false;
	}

	JsonObject->SetRootObject(RootJson);
	return true;
}


bool UIVJsonLibrary::DeserializeJson(const FString& JsonString, TSharedPtr<FJsonObject>& JsonObject)
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if(!FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		UE_LOG(LogIVJson, Error, TEXT("Failed to deserialize JSON string"));
		
		return  false;
	}
	
	return true;
}

bool UIVJsonLibrary::DeserializeJsonFromFile(FString FilePath, TSharedPtr<FJsonObject>& JsonObject)
{
	//注意：此操作只针对放入iVisual插件对应文件夹有用
	const FString PathPrefix="LocalData://";
	if((UKismetStringLibrary::StartsWith(FilePath,PathPrefix,ESearchCase::IgnoreCase)))
	{
		FString File=UKismetStringLibrary::RightChop(FilePath,PathPrefix.Len());
		FilePath=FPaths::Combine(UiVisualFileHeplerBPLibrary::GetExtrasDataDir(TEXT("IVCommon"))/File);
	}

	if (!FPaths::FileExists(FilePath))
	{
		UE_LOG(LogIVJson, Error, TEXT("No Valid Json File Found At %s"), *FilePath);
		return false;
	}
	
	FString Contents;
	if(!FFileHelper::LoadFileToString(Contents,*FilePath))
	{
		return false;
	}
	
	return DeserializeJson(Contents,JsonObject);
}
