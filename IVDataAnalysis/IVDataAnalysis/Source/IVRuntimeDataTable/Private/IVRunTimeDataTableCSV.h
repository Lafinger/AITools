// Copyright 2023 AIRT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RunTimeDataTable.h"

/**
 * .csv导入项
 */
class FIVDataTableImporterCSV
{
public:
	FIVDataTableImporterCSV(URunTimeTable& InDataTable, FString InCSVData);

	~FIVDataTableImporterCSV();

	bool ReadTable(bool bClearData);
	
public:
	URunTimeTable* DataTable;
	FString CSVData;
};
