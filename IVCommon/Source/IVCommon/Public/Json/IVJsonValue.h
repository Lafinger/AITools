// Copyright 2023 ARIT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonValue.h"
#include "Misc/Base64.h"
#include "IVJsonValue.generated.h"


class UIVJsonObject;

/**
 * Json类型
 */
UENUM(BlueprintType)
namespace EIVJson
{
	enum Type
	{
		None,
		Null,
		String,
		Number,
		Boolean,
		Array,
		Object,
		Binary
	};
}

class IVCOMMON_API FJsonValueBinary : public FJsonValue
{
public:

	FJsonValueBinary(const TArray<uint8>& InBinary) : Value(InBinary) { Type = EJson::String; }

	virtual bool TryGetString(FString& OutString) const override
	{
		//OutString = FString::FromHexBlob(Value.GetData(), Value.Num());	//HEX encoding
		OutString = FBase64::Encode(Value);									//Base64 encoding
		return true;
	}
	virtual bool TryGetNumber(double& OutDouble) const override
	{
		OutDouble = Value.Num();
		return true;
	}

	//hackery: we use this as an indicator we have a binary (strings don't normally do this)
	virtual bool TryGetBool(bool& OutBool) const override { return false; }

	/** 返回二进制数据. */
	TArray<uint8> AsBinary() { return Value; }

	/** 判断FJsonValue是否为Binary. */
	static bool IsBinary(const TSharedPtr<FJsonValue>& InJsonValue);

	/** 获取Brinary数组若要使用此方法,最好用IsBinary做判断. */
	static TArray<uint8> AsBinary(const TSharedPtr<FJsonValue>& InJsonValue);

protected:
	TArray<uint8> Value;

	virtual FString GetType() const override { return TEXT("Binary"); }
};

/**
 * Blueprintable FJsonValue wrapper
 */
UCLASS(BlueprintType, Blueprintable)
class IVCOMMON_API UIVJsonValue : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** 创建新的 Json 数字值*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct Json Number Value"), Category = "IVJson")
	static UIVJsonValue* ConstructJsonValueNumber(float Number);

	/**创建新的 Json 字符串值  */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct Json String Value"), Category = "IVJson")
	static UIVJsonValue* ConstructJsonValueString(const FString& StringValue);

	/** 创建新的 Json Bool 值 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct Json Bool Value"), Category = "IVJson")
	static UIVJsonValue* ConstructJsonValueBool( bool InValue);

	/** 创建新的 Json 数组值 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct Json Array Value"), Category = "IVJson")
	static UIVJsonValue* ConstructJsonValueArray(const TArray<UIVJsonValue*>& InArray);

	/**创建新的 Json Object value */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct Json Object Value"), Category = "IVJson")
	static UIVJsonValue* ConstructJsonValueObject(UIVJsonObject* JsonObject);

	/** 创建新的 Json Binary value */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct Json Binary Value"), Category = "IVJson")
	static UIVJsonValue* ConstructJsonValueBinary(TArray<uint8> ByteArray);

	/** 从 FJsonValue 创建新的 Json 值（从 UIVJsonObject 使用） */
	static UIVJsonValue* ConstructJsonValue(const TSharedPtr<FJsonValue>& InValue);

	/** 从 JSON 编码的字符串创建新的 Json 值*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Value From Json String"), Category = "IVJson")
	static UIVJsonValue* ValueFromJsonString(const FString& StringValue);

	/** 获取root Json 值 */
	TSharedPtr<FJsonValue>& GetRootValue();

	/** 设置 root Json 值 */
	void SetRootValue(TSharedPtr<FJsonValue>& JsonValue);


	//////////////////////////////////////////////////////////////////////////
	// FJsonValue API

	/** 获取 Json 值的类型 (Enum) */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	EIVJson::Type GetType() const;

	/**获取 Json value (String) */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	FString GetTypeString() const;

	/** 如果此值为“null”，则返回真 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	bool IsNull() const;

	/**将此值作为双精度值返回，如果这不是 Json Number 则抛出错误 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	float AsNumber() const;

	/**将此值作为String返回，如果这不是 Json 字符串则抛出错误 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	FString AsString() const;

	/**将此值作为布尔值返回，如果这不是 Json Bool 则抛出错误 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	bool AsBool() const;

	/** 将此值作为array返回，如果这不是 Json 数组则抛出错误 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	TArray<UIVJsonValue*> AsArray() const;

	/** 将此值作为对象返回，如果这不是 Json 对象则抛出错误 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	UIVJsonObject* AsObject();

	//将消息转换为二进制数据
	UFUNCTION(BlueprintPure, Category = "IVJson")
	TArray<uint8> AsBinary();

	UFUNCTION(BlueprintCallable, Category = "IVJson",meta=(CompactNodeTitle = "->", BlueprintAutocast))
	FString EncodeJson() const;

	//////////////////////////////////////////////////////////////////////////
	// Data

private:
	/** Internal JSON data */
	TSharedPtr<FJsonValue> JsonVal;

	static TSharedPtr<FJsonValue> JsonStringToJsonValue(const FString& JsonString);

	static TSharedPtr<FJsonObject> ToJsonObject(const FString& JsonString);

	static FString ToJsonString(const TSharedPtr<FJsonObject>& JsonObject);
	static FString ToJsonString(const TSharedPtr<FJsonValue>& JsonValue);
	static FString ToJsonString(const TArray<TSharedPtr<FJsonValue>>& JsonValueArray);
protected:
	/** Simple error logger */
	void ErrorMessage(const FString& InType) const;

};
