// Copyright 2023 ARIT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IVJsonConfig.h"

#include "Containers/StringFwd.h"
#include "Containers/StringView.h"
#include "Delegates/Delegate.h"
#include "Dom/JsonObject.h"
#include "HAL/Platform.h"
#include "Misc/AssertionMacros.h"
#include "Templates/IsPointer.h"
#include "Templates/SharedPointer.h"
#include "Templates/UnrealTemplate.h"
#include "Templates/UnrealTypeTraits.h"

class FArrayProperty;
class FJsonObject;
class FJsonValue;
class FMapProperty;
class FProperty;
class FSetProperty;
class FString;
class UClass;
class UObject;
class UStruct;

class IVCOMMON_API FIVConfig
{
public:


	FIVConfig();

	void SetParent(TSharedPtr<FIVConfig> InConfig);
	
	bool LoadFromString(FStringView Content);
	bool LoadFromFile(FStringView FilePath);
	bool SaveToString(FString& OutResult) const;
	
	bool IsValid() const { return JsonConfig.IsValid() && JsonConfig->IsValid(); }

	// UStruct & UObject
	template <typename T>
	bool TryGetStruct(FStringView Key, T& OutValue = true) const;
	template <typename T>
	bool TryGetUObject(FStringView Key, T& OutValue = true) const;

	template <typename T>
	bool TryGetRootStruct(T& OutValue = true) const;
	template <typename T>
	bool TryGetRootUObject(T& OutValue = true) const;
	template <typename T>
	bool TryGetRootUObject(T* OutValue = true) const;

	bool TryGetRootStruct(const UStruct* Class, void* OutValue) const;
	bool TryGetRootUObject(const UClass* Class, UObject* OutValue ) const;

	template <typename T>
	void SetStruct(FStringView Key, const T& InValue = true);
	template <typename T>
	void SetUObject(FStringView Key, const T& InValue = true);

	template <typename T>
	void SetRootStruct(const T& InValue = true);
	template <typename T>
	void SetRootUObject(const T& InValue = true);

	void SetRootStruct(const UStruct* Class, const void* Instance);
	void SetRootUObject(const UClass* Class, const UObject* Instance);

	bool HasOverride(FStringView Key) const;

	void OnSaved();

	DECLARE_EVENT_OneParam(FIVConfig, FOnIVConfigDirtied, const FIVConfig&);
	FOnIVConfigDirtied& OnEditorConfigDirtied() { return IVConfigDirtiedEvent; }


public:
	
	static TSharedPtr<FJsonObject> SerializeFunction(const UClass* Class, const UObject* Instance,TArray<FString>& NativeFunction);
	static TSharedPtr<FJsonObject> ReadFuction(const TSharedPtr<FJsonObject>& JsonObject,const UFunction* Function,const UObject* Owner);

	static FString GetShortName(const FProperty* Property);

	
	static void ReadUObject(const TSharedPtr<FJsonObject>& JsonObject, const UClass* Class, UObject* Instance);
	static void ReadStruct(const TSharedPtr<FJsonObject>& JsonObject, const UStruct* Struct, void* Instance, UObject* Owner);
	static void ReadValue(const TSharedPtr<FJsonValue>& JsonValue, const FProperty* Property, void* DataPtr, UObject* Owner);
	
	
	static TSharedPtr<FJsonObject> WriteFunction( UFunction* Function,const void* Instance, const void* Defaults);
	static TSharedPtr<FJsonObject> WriteStruct(const UStruct* Struct, const void* Instance, const void* Defaults);
	static TSharedPtr<FJsonObject> WriteUObject(const UClass* Class, const UObject* Instance);
	static TSharedPtr<FJsonValue> WriteArray(const FArrayProperty* ArrayProperty, const void* DataPtr);
	static TSharedPtr<FJsonValue> WriteSet(const FSetProperty* Property, const void* DataPtr);
	static TSharedPtr<FJsonValue> WriteMap(const FMapProperty* Property, const void* DataPtr);
	static TSharedPtr<FJsonValue> WriteValue(const FProperty* Property, const void* DataPtr, const void* Defaults );
	
	

	bool SaveToFile(FStringView FilePath) const;

	void SetDirty();

	

private:
	TSharedPtr<IV::FIVJsonConfig> JsonConfig;
	TSharedPtr<FIVConfig> ParentConfig;
		
	FOnIVConfigDirtied IVConfigDirtiedEvent;
	bool Dirty { false };
};



template <typename T>
bool FIVConfig::TryGetStruct(FStringView Key, T& OutValue) const
{
	if (!IsValid())
	{
		return false;
	}

	TSharedPtr<FJsonObject> StructData;
	IV::FIVJsonPath Path(Key);
	if (!JsonConfig->TryGetJsonObject(Path, StructData))
	{
		return false;
	}

	if (!StructData.IsValid())
	{
		return false;
	}

	const UStruct* Struct = T::StaticStruct();
	ReadStruct(StructData, Struct, &OutValue, nullptr);

	return true;
}

template <typename T>
bool FIVConfig::TryGetUObject(FStringView Key, T& OutValue) const
{
	static_assert(TIsDerivedFrom<T, UObject>::Value, "Type is not derived from UObject.");

	if (!IsValid())
	{
		return false;
	}

	TSharedPtr<FJsonObject> UObjectData;
	IV::FIVJsonPath Path(Key);
	if (!JsonConfig->TryGetJsonObject(Path, UObjectData))
	{
		return false;
	}

	if (!UObjectData.IsValid())
	{
		return false;
	}

	const UClass* Class = T::StaticClass();
	ReadUObject(UObjectData, Class, &OutValue);

	return true;
}

template <typename T>
bool FIVConfig::TryGetRootStruct(T& OutValue) const
{
	return TryGetRootStruct(T::StaticStruct(), &OutValue);
}

template <typename T>
bool FIVConfig::TryGetRootUObject(T& OutValue) const
{
	static_assert(TIsDerivedFrom<T, UObject>::Value, "Type is not derived from UObject.");

	return TryGetRootUObject(T::StaticClass(), &OutValue);
}

template <typename T>
bool FIVConfig::TryGetRootUObject(T* OutValue) const
{
	static_assert(TIsDerivedFrom<T, UObject>::Value, "Type is not derived from UObject.");
	checkf(OutValue != nullptr, TEXT("Output value was null."));

	return TryGetRootUObject(T::StaticClass(), OutValue);
}

template <typename T>
void FIVConfig::SetStruct(FStringView Key, const T& InValue)
{
	if (!IsValid())
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject = WriteStruct(T::StaticStruct(), &InValue, nullptr);
	JsonConfig->SetJsonObject(IV::FIVJsonPath(Key), JsonObject);

	SetDirty();
}

template <typename T>
void FIVConfig::SetUObject(FStringView Key, const T& InValue)
{
	static_assert(TIsDerivedFrom<T, UObject>::Value, "Type is not derived from UObject.");

	if (!IsValid())
	{
		return;
	}
	
	TSharedPtr<FJsonObject> JsonObject = WriteUObject(T::StaticClass(), &InValue);
	JsonConfig->SetJsonObject(IV::FIVJsonPath(Key), JsonObject);
		
	SetDirty();
}

template <typename T>
void FIVConfig::SetRootStruct(const T& InValue )
{
	SetRootStruct(T::StaticStruct(), &InValue);
}

template <typename T>
void FIVConfig::SetRootUObject(const T& InValue)
{
	if constexpr (TIsPointer<T>::Value)
	{
		static_assert(TIsDerivedFrom<typename TRemovePointer<T>::Type, UObject>::Value, "Type is not derived from UObject.");
		checkf(InValue != nullptr, TEXT("Object value was null."));

		SetRootUObject(TRemovePointer<T>::Type::StaticClass(), InValue);
	}
	else
	{
		static_assert(TIsDerivedFrom<typename TRemovePointer<T>::Type, UObject>::Value, "Type is not derived from UObject.");

		SetRootUObject(T::StaticClass(), &InValue);
	}
}
