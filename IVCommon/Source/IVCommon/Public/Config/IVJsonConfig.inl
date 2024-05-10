
// Copyright 2023 ARIT All Rights Reserved.

#pragma once

namespace IV
{
	template <typename T>
	bool FIVJsonConfig::TryGetNumber(const FIVJsonPath& Path, T& OutValue) const
	{
		TSharedPtr<FJsonValue> JsonValue;
		if (!TryGetJsonValue(Path, JsonValue))
		{
			return false;
		}

		return JsonValue->TryGetNumber(OutValue);
	}

	template <typename T>
	bool FIVJsonConfig::SetNumber(const FIVJsonPath& Path, T Value)
	{
		static_assert(TIsArithmetic<T>::Value, "Value type must be a number.");

		TSharedPtr<FJsonValue> JsonValue = MakeShared<FJsonValueNumber>((double) Value);
		return SetJsonValue(Path, JsonValue);
	}

	template <typename T, typename TGetter>
	bool FIVJsonConfig::TryGetArrayHelper(const FIVJsonPath& Path, TArray<T>& OutArray, TGetter Getter) const
	{
		if (!IsValid())
		{
			return false;
		}
		
		TArray<TSharedPtr<FJsonValue>> JsonValueArray;
		if (!TryGetArray(Path, JsonValueArray))
		{
			return false;
		}

		OutArray.Reset();

		for (const TSharedPtr<FJsonValue>& JsonValue : JsonValueArray)
		{
			T Value;
			if (!Getter(JsonValue, Value))
			{
				OutArray.Reset();
				return false;
			}

			OutArray.Add(Value);
		}

		return true;
	}

	template <typename T>
	bool FIVJsonConfig::TryGetNumericArrayHelper(const FIVJsonPath& Path, TArray<T>& OutArray) const
	{
		return TryGetArrayHelper(Path, OutArray, 
	[](const TSharedPtr<FJsonValue>& JsonValue, T& OutNumber)
		{
		return JsonValue->TryGetNumber(OutNumber);
		});
	}
}
