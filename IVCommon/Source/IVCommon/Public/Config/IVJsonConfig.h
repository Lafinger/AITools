// Copyright 2023 ARIT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
namespace IV
{
	class IVCOMMON_API FIVJsonPath
	{
	public:
		struct FIVPart
		{
			FString Name;
			int32 Index = INDEX_NONE;
		};

		FIVJsonPath();
		FIVJsonPath(const TCHAR* Path);
		FIVJsonPath(FStringView Path);
		FIVJsonPath(const FIVJsonPath& Other);
		FIVJsonPath(FIVJsonPath&& Other);

		bool IsValid() const { return Length() > 0; }
		const int32 Length() const { return PathParts.Num(); }

		void Append(FStringView Name);
		void SetArrayIndex(int32 Index);

		FIVJsonPath GetSubPath(int32 NumParts) const;

		FString ToString() const;

		const FIVPart& operator[](int32 Idx) const { return PathParts[Idx]; }
		const TArray<FIVPart>& GetAll() const { return PathParts; }

	private:
		void ParsePath(const FString& InPath);

	private:
		TArray<FIVPart> PathParts;
	};

	using FJsonValuePair = TPair<TSharedPtr<FJsonValue>, TSharedPtr<FJsonValue>>;

	class IVCOMMON_API FIVJsonConfig
	{
	public:
		FIVJsonConfig();

		void SetParent(const TSharedPtr<FIVJsonConfig>& Parent);

		bool LoadFromFile(FStringView FilePath);
		bool LoadFromString(FStringView Content);
	
		bool SaveToFile(FStringView FilePath) const;
		bool SaveToString(FString& OutResult) const;

		bool IsValid() const { return MergedObject.IsValid(); }

		const FIVJsonConfig* GetParentConfig() const;

		template <typename T>
		bool TryGetNumber(const FIVJsonPath& Path, T& OutValue) const;
		bool TryGetBool(const FIVJsonPath& Path, bool& OutValue) const;
		bool TryGetString(const FIVJsonPath& Path, FString& OutValue) const;
		bool TryGetString(const FIVJsonPath& Path, FName& OutValue) const;
		bool TryGetString(const FIVJsonPath& Path, FText& OutValue) const;
		bool TryGetJsonValue(const FIVJsonPath& Path, TSharedPtr<FJsonValue>& OutValue) const;
		bool TryGetJsonObject(const FIVJsonPath& Path, TSharedPtr<FJsonObject>& OutValue) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<TSharedPtr<FJsonValue>>& OutArray) const;

		TSharedPtr<FJsonObject> GetRootObject() const;


		bool TryGetArray(const FIVJsonPath& Path, TArray<bool>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<int8>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<int16>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<int32>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<int64>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<uint8>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<uint16>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<uint32>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<uint64>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<float>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<double>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<FString>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<FText>& OutArray) const;
		bool TryGetArray(const FIVJsonPath& Path, TArray<FName>& OutArray) const;

		bool TryGetMap(const FIVJsonPath& Path, TArray<FJsonValuePair>& OutMap) const;
		
		template <typename T>
		bool SetNumber(const FIVJsonPath& Path, T Value);
		bool SetBool(const FIVJsonPath& Path, bool Value);
		bool SetString(const FIVJsonPath& Path, FStringView Value);
		bool SetString(const FIVJsonPath& Path, const FText& Value);
		bool SetJsonValue(const FIVJsonPath& Path, const TSharedPtr<FJsonValue>& Value);
		bool SetJsonObject(const FIVJsonPath& Path, const TSharedPtr<FJsonObject>& Object);
		bool SetJsonArray(const FIVJsonPath& Path, const TArray<TSharedPtr<FJsonValue>>& Array);

		bool SetRootObject(const TSharedPtr<FJsonObject>& Object);

		bool HasOverride(const FIVJsonPath& Path) const;

	private:

		template <typename T, typename TGetter>
		bool TryGetArrayHelper(const FIVJsonPath& Path, TArray<T>& OutArray, TGetter Getter) const;

		template <typename T>
		bool TryGetNumericArrayHelper(const FIVJsonPath& Path, TArray<T>& OutArray) const;

		bool SetJsonValueInMerged(const FIVJsonPath& Path, const TSharedPtr<FJsonValue>& Value);
		bool SetJsonValueInOverride(const FIVJsonPath& Path, const TSharedPtr<FJsonValue>& NewValue, const TSharedPtr<FJsonValue>& PreviousValue, const TSharedPtr<FJsonValue>& ParentValue);

		//todo
		bool SetArrayValueInOverride(const TSharedPtr<FJsonValue>& CurrentValue, const TArray<TSharedPtr<FJsonValue>>& NewArray, const TSharedPtr<FJsonValue>& ParentValue);
		bool SetObjectValueInOverride(const TSharedPtr<FJsonObject>& CurrentObject, const TSharedPtr<FJsonObject>& NewObject, const TSharedPtr<FJsonValue>& ParentValue);

		bool RemoveJsonValueFromOverride(const FIVJsonPath& Path, const TSharedPtr<FJsonValue>& PreviousValue);

		bool MergeThisWithParent();
		void OnParentConfigChanged();

	private:

		TFuture<void> SaveFuture;

		FSimpleDelegate OnConfigChanged;

		TSharedPtr<FIVJsonConfig> ParentConfig;

		TSharedPtr<FJsonObject> OverrideObject;
		TSharedPtr<FJsonObject> MergedObject;
	};
}

#include "IVJsonConfig.inl" 