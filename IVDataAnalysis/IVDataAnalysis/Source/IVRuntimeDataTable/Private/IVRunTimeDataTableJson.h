// Copyright 2023 AIRT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RunTimeDataTable.h"

/**
 * 。json导入项
 */
class FIVDataTableImporterJSON
{
public:
	FIVDataTableImporterJSON(URunTimeTable& InDataTable, FString InJsonData);

	~FIVDataTableImporterJSON();

	bool ReadTable(bool bClearData);

private:

	bool ReadRow(const TSharedRef<FJsonObject>& InParsedTableRowObject, const int32 InRowIdx);

	bool ReadStruct(const TSharedRef<FJsonObject>& InParsedObject, UScriptStruct* InStruct, const FName InRowName, void* InStructData);

	bool ReadStructEntry(const TSharedRef<FJsonValue>& InParsedPropertyValue, const FName InRowName, const FString& InColumnName, const void* InRowData, FProperty* InProperty, void* InPropertyData);

	bool ReadContainerEntry(const TSharedRef<FJsonValue>& InParsedPropertyValue, const FName InRowName, const FString& InColumnName, const int32 InArrayEntryIndex, FProperty* InProperty, void* InPropertyData);

	FString GetKeyFieldName();
public:
	URunTimeTable* DataTable;
	FString JSONData;
};
