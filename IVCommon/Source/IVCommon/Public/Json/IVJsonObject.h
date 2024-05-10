// Copyright 2023 ARIT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Dom/JsonObject.h"
#include "IVJsonObject.generated.h"


class IVBJsonValue;
/**
 *  Blueprintable FJsonObject wrapper
 */
UCLASS(BlueprintType, Blueprintable)
class IVCOMMON_API UIVJsonObject : public UObject
{
	GENERATED_UCLASS_BODY()
	
	/** 创建新的 Json object*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Construct Json Object", KeyWords = "create make",HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "IVJson")
	static UIVJsonObject* ConstructJsonObject(UObject* WorldContextObject);

	/** 重置所有数据 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void Reset();


	//////////////////////////////////////////////////////////////////////////
	// Serialization

	/** 将Json序列化为字符串（换行符的格式） */
	UFUNCTION(BlueprintCallable, Category = "IVJson",meta=(CompactNodeTitle = "->", BlueprintAutocast))
	FString EncodeJson(bool IsCondensedString=true) const;

	/** Json序列化为字符串（单个字符串不换行） */
	UFUNCTION(BlueprintCallable, Category = "IVJson",meta=(CompactNodeTitle = "->", BlueprintAutocast))
	FString EncodeJsonToSingleString() const;

	/** 从字符串构造 Json 对象 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	bool DecodeJson(const FString& JsonString);


	//////////////////////////////////////////////////////////////////////////
	// FJsonObject API

	/** 返回object中存在的字段名列表 */
	UFUNCTION(BlueprintPure, Category = "IVJson")
	TArray<FString> GetFieldNames();

	/** 检查object中是否存在 FieldName */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	bool HasField(const FString& FieldName) const;

	/** 删除名为 FieldName 的字段 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void RemoveField(const FString& FieldName);

	/** 获取名为 FieldName 的字段作为 JsonValue */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	UIVJsonValue* GetField(const FString& FieldName) const;

	/** 添加一个名为 FieldName 的字段 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void SetField(const FString& FieldName, UIVJsonValue* JsonValue);

	/** 获取名为 FieldName 的字段作为 Json 数组 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	TArray<UIVJsonValue*> GetArrayField(const FString& FieldName);

	/** 设置一个名为 FieldName 的 ObjectField 和 Json 数组的值 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void SetArrayField(const FString& FieldName, const TArray<UIVJsonValue*>& InArray);

	/** 将一个 json 对象中的所有字段添加到这个对象中 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void MergeJsonObject(UIVJsonObject* InJsonObject, bool Overwrite);


	//////////////////////////////////////////////////////////////////////////
	// FJsonObject API Helpers (easy to use with simple Json objects)

	/** 获取名为 FieldName 的字段作为数字。 确保该字段存在并且是 Json 数字类型. */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	float GetNumberField(const FString& FieldName) const;

	/**添加一个名为 FieldName 的字段，其值为 Number*/
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void SetNumberField(const FString& FieldName, float Number);

	/** 获取名为 FieldName 的字段作为字符串. */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	FString GetStringField(const FString& FieldName) const;

	/** 添加一个名为 FieldName 的字段，值为 StringValue */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void SetStringField(const FString& FieldName, const FString& StringValue);

	/**获取名为 FieldName 的字段作为 boolean. */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	bool GetBoolField(const FString& FieldName) const;

	/** 设置一个名为 FieldName 的布尔字段和 InValue 的值 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void SetBoolField(const FString& FieldName, bool InValue);

	/** 获取名为 FieldName 的字段作为 Json Object. */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	UIVJsonObject* GetObjectField(const FString& FieldName) const;

	/** 设置一个名为 FieldName 的 ObjectField 和 JsonObject 的值 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void SetObjectField(const FString& FieldName, UIVJsonObject* JsonObject);

	/** 获取名为 FieldName 的字段作为 binary buffer array. */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void GetBinaryField(const FString& FieldName, TArray<uint8>& OutBinary) const;

	/** 设置一个名为 FieldName 的 BinaryField 和 binary buffer array */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void SetBinaryField(const FString& FieldName, const TArray<uint8>& Bytes);


	//////////////////////////////////////////////////////////////////////////
	// Array fields helpers (uniform arrays)

	/** 将名为 FieldName 的字段设置为数字数组。 仅当您确定数组是统一的时才使用它！*/
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	TArray<float> GetNumberArrayField(const FString& FieldName);

	/**设置一个名为 FieldName 的 ObjectField 和 Number Array 的值 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void SetNumberArrayField(const FString& FieldName, const TArray<float>& NumberArray);

	/** 获取名为 FieldName 的字段作为字符串数组。 仅当您确定数组是统一的时才使用它！ */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	TArray<FString> GetStringArrayField(const FString& FieldName);

	/** 设置一个名为 FieldName 的 ObjectField 和 String Array 的值 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void SetStringArrayField(const FString& FieldName, const TArray<FString>& StringArray);

	/** 获取名为 FieldName 的字段作为 Bool 数组。 仅当您确定数组是统一的时才使用它！ */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	TArray<bool> GetBoolArrayField(const FString& FieldName);

	/** 设置一个名为 FieldName 的 ObjectField 和 Bool 数组的值 */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void SetBoolArrayField(const FString& FieldName, const TArray<bool>& BoolArray);

	/** 获取名为 FieldName 的字段作为对象数组。 仅当您确定数组是统一的时才使用它! */
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	TArray<UIVJsonObject*> GetObjectArrayField(const FString& FieldName);

	/** 设置一个名为 FieldName 的 ObjectField 和 Ob Array 的值*/
	UFUNCTION(BlueprintCallable, Category = "IVJson")
	void SetObjectArrayField(const FString& FieldName, const TArray<UIVJsonObject*>& ObjectArray);


public:
	/** 获取 root Json object */
	TSharedPtr<FJsonObject>& GetRootObject();

	/** 设置  root Json object */
	void SetRootObject(const TSharedPtr<FJsonObject>& JsonObject);
	//////////////////////////////////////////////////////////////////////////
	// Data

private:
	/** Internal JSON data */
	TSharedPtr<FJsonObject> JsonObj;
};
