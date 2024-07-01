// Copyright 2023 AIRT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RunTimeDataTable.h"


class UIVExcelWorkBook;
struct FExcelWorkSheet;

/**
 * .xlsl导入项
 */
class  FIVDataTableImporterXLSL
{
public:
	FIVDataTableImporterXLSL(URunTimeTable* InDataTable, UIVExcelWorkBook* InWorkBook);
	~FIVDataTableImporterXLSL();

	bool ReadTable(bool bClearData);
private:
	URunTimeTable* DataTable;
	TWeakObjectPtr<UIVExcelWorkBook> WorkBook;
};
