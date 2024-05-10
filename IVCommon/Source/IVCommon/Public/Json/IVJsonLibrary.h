// Copyright 2023 ARIT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IVJsonObject.h"
#include "IVJsonValue.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IVJsonLibrary.generated.h"

/**
 * 
 */
UCLASS()
class IVCOMMON_API UIVJsonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**对文本percent-encoding*/
	UFUNCTION(BlueprintCallable,Category="IVJson|Utility")
	static FString PercentEncode(const FString& Source);

	/**
	* 将 FString 编码为 Base64 字符串
	*
	* @param Source 要转换的字符串数据
	* @return 字符串，它以一种可以通过各种 Internet 协议安全传输的方式对二进制数据进行编码
	*/
	UFUNCTION(BlueprintCallable,Category="IVJson|Utility", meta = (DisplayName = "Base64 Encode (String)"))
	static FString Base64Encode(const FString& Source);

	/**
	* 将 Bytes 编码为 Base64 字符串
	*
	* @param Source 要转换的字符串数据
	* @return 字符串，它以一种可以通过各种 Internet 协议安全传输的方式对二进制数据进行编码
	*/
	UFUNCTION(BlueprintPure, Category = "IVJson|Utility", meta = (DisplayName = "Base64 Encode (Bytes)"))
	static FString Base64EncodeBytes(const TArray<uint8>& Source);

	/**
	* 将 Base64 字符串解码为 FString
	*
	* @param Source 要转换的字符串化数据
	* @param Dest 将填充解码数据的输出 buffer
	* @return 如果 buffer 被解码则为真，如果解码失败则为假
	*/
	UFUNCTION(BlueprintPure, Category = "IVJson|Utility", meta = (DisplayName = "Base64 Decode (To String)"))
	static bool Base64Decode(const FString& Source, FString& Dest);

	/**
	* 将 Base64 字符串解码为 Bytes
	*
	* @param Source 要转换的字符串化数据
	* @param Dest 将填充解码数据的输出 buffer
	* @return 如果buffer被解码则为真，如果解码失败则为假
	*/
	UFUNCTION(BlueprintPure, Category = "IVJson|Utility", meta = (DisplayName = "Base64 Decode (To Bytes)"))
	static bool Base64DecodeBytes(const FString& Source, TArray<uint8>& Dest);

	/**
	* 将 json 字符串解码为字符串化的 jsonValues
	*
	* @param JsonString 输入字符串化的 json
	* @param OutJsonValueArray 解码后的 JsonValue 数组
	*/
	UFUNCTION(BlueprintPure, Category = "IVJson|Utility")
	static bool StringToJsonValueArray(const FString& JsonString, TArray<UIVJsonValue*>& OutJsonValueArray);
	
	


	//To JsonValue (Array)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To JsonValue (Array)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static UIVJsonValue* Conv_ArrayToJsonValue(const TArray<UIVJsonValue*>& InArray);

	//To JsonValue (JsonObject)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To JsonValue (JsonObject)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static UIVJsonValue* Conv_JsonObjectToJsonValue(UIVJsonObject* InObject);

	//To JsonValue (Bytes)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To JsonValue (Bytes)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static UIVJsonValue* Conv_BytesToJsonValue(const TArray<uint8>& InBytes);

	//To JsonValue (String)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To JsonValue (String)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static UIVJsonValue* Conv_StringToJsonValue(const FString& InString);

	//To JsonValue (Integer)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To JsonValue (Integer)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static UIVJsonValue* Conv_IntToJsonValue(int32 InInt);

	//To JsonValue (Float)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To JsonValue (Float)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static UIVJsonValue* Conv_FloatToJsonValue(float InFloat);

	//To JsonValue (Bool)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To JsonValue (Bool)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static UIVJsonValue* Conv_BoolToJsonValue(bool InBool);

	//To String (JsonValue) - doesn't autocast due to get display name
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To String (JsonValue)", BlueprintAutocast), Category = "UIVJson|Convert")
	static FString Conv_JsonValueToString(class UIVJsonValue* InValue);

	//To Integer (JsonValue)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Integer (JsonValue)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static int32 Conv_JsonValueToInt(class UIVJsonValue* InValue);

	//To Float (JsonValue)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Float (JsonValue)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static float Conv_JsonValueToFloat(class UIVJsonValue* InValue);

	//To Bool (JsonValue)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Bool (JsonValue)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static bool Conv_JsonValueToBool(class UIVJsonValue* InValue);

	//To Bytes (JsonValue)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Bytes (JsonValue)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static TArray<uint8> Conv_JsonValueToBytes(class UIVJsonValue* InValue);

	//To String (JsonObject) - doesn't autocast due to get display name
	//UFUNCTION(BlueprintPure, meta = (DisplayName = "To String (JsonObject)",CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	//static FString Conv_JsonObjectToString(class UIVJsonObject* InObject);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To JsonObject (String)",CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static class UIVJsonObject*  ConvString_ToJsonObject(const FString& InString);

	//To Object (JsonValue)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Object (JsonValue)", CompactNodeTitle = "->", BlueprintAutocast), Category = "IVJson|Convert")
	static UIVJsonObject* Conv_JsonValueToJsonObject(class UIVJsonValue* InValue);

	/**
	 * @从本地加载json数据
	 * @param JsonObject 返回Json实例
	 * @param FilePath 文件路径，LocalData://路径并不完整，需要补充完整，默认路径只放入iVisual插件对应Extras文件夹有用
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = "IVJson|FileHepler")
	static bool LoadJsonFromFile(UIVJsonObject*& JsonObject,FString FilePath= TEXT("LocalData://"));
	
public:
	static bool DeserializeJson(const FString& JsonString,TSharedPtr<FJsonObject> &JsonObject);
	
	static bool DeserializeJsonFromFile( FString FilePath,TSharedPtr<FJsonObject> &JsonObject);
	
};
