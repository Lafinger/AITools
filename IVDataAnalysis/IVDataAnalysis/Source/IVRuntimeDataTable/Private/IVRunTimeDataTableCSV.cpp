// Copyright 2023 AIRT All Rights Reserved.


#include "IVRunTimeDataTableCSV.h"

#include "IVRuntimeDataTableModule.h"
#include "RunTimeDataTable.h"
#include "Serialization/Csv/CsvParser.h"


FIVDataTableImporterCSV::FIVDataTableImporterCSV(URunTimeTable& InDataTable, FString InCSVData)
	: DataTable(&InDataTable)
	, CSVData(MoveTemp(InCSVData))
{
}

FIVDataTableImporterCSV::~FIVDataTableImporterCSV()
{
}

bool FIVDataTableImporterCSV::ReadTable(bool bClearData)
{
	if (CSVData.IsEmpty())
	{
		UE_LOG(LogIVDataTable,Error,TEXT("Input data is empty."))
		return false;
	}

	if (!DataTable->RowStruct)
	{
		UE_LOG(LogIVDataTable,Error,TEXT("No RowStruct specified."));
		return false;
	}

	const FCsvParser Parser(CSVData);
	const auto& Rows = Parser.GetRows();

	if(Rows.Num() < 1)
	{
		UE_LOG(LogIVDataTable,Error,TEXT("Too few rows (there must be at least a header row)."));
		return false;
	}

	
	// 找到Key
	int32 KeyColumn = 0;
	if (!DataTable->ImportKeyField.IsEmpty())
	{
		for (int32 ColIdx = 0; ColIdx < Rows[0].Num(); ++ColIdx)
		{
			const TCHAR* ColumnValue = Rows[0][ColIdx];
			if (DataTable->ImportKeyField == FString(ColumnValue))
			{
				KeyColumn = ColIdx;
				break;
			}
		}
	}

	TArray<FProperty*> ColumnProps = DataTable->GetTablePropertyArray(Rows[0], DataTable->RowStruct, KeyColumn);

	//清空数据
	if(bClearData)
	{
		DataTable->EmptyTable();
	}
	
	for(int32 RowIdx=1; RowIdx<Rows.Num(); RowIdx++)
	{
		const TArray<const TCHAR*>& Cells = Rows[RowIdx];

		//至少需要一个关键词
		if (Cells.Num() <= KeyColumn)
		{
			UE_LOG(LogIVDataTable,Warning,TEXT("Row '%d' has too few cells."), RowIdx);
			continue;
		}

		if( ColumnProps.Num() < Cells.Num() )
		{
			UE_LOG(LogIVDataTable,Warning,TEXT("Row '%d' has more cells than properties, is there a malformed string?"), RowIdx);
			continue;
		}

		// Get row name
		FName RowName = DataTableUtils::MakeValidName(Cells[KeyColumn]);

		// Check its not 'none'
		if(RowName == NAME_None)
		{
			if (DataTable->ImportKeyField.IsEmpty())
			{
				UE_LOG(LogIVDataTable,Warning,TEXT("Row '%d' missing key field '%s'."), RowIdx, *DataTable->ImportKeyField);
			}

			continue;
		}

		// 分配数据来存储信息，使用UScriptStruct知道它的大小
		uint8* RowData = (uint8*)FMemory::Malloc(DataTable->RowStruct->GetStructureSize());
		DataTable->RowStruct->InitializeStruct(RowData);
		// 并且一定要稍后调用 DestroyScriptStruct

		DataTable->AddRowInternal(RowName, RowData);
		// 现在迭代单元格（跳过第一个单元格，除非有明确的名称）
		for(int32 CellIdx = 0; CellIdx < Cells.Num(); CellIdx++)
		{
			if (CellIdx == KeyColumn)
			{
				continue;
			}

			// 尝试使用列属性将字符串分配给数据
			FProperty* ColumnProp = ColumnProps[CellIdx];
			const FString CellValue = Cells[CellIdx];
			FString Error = DataTableUtils::AssignStringToProperty(CellValue, ColumnProp, RowData);
			
			if(Error.Len() > 0)
			{
				FString ColumnName = (ColumnProp != nullptr) 
					? DataTableUtils::GetPropertyExportName(ColumnProp)
					: FString(TEXT("NONE"));
				UE_LOG(LogIVDataTable,Warning,TEXT("Problem assigning string '%s' to property '%s' on row '%s' : %s"), *CellValue, *ColumnName, *RowName.ToString(), *Error);
			}
		}
		if(Cells.Num() < ColumnProps.Num())
		{
			UE_LOG(LogIVDataTable,Warning,TEXT("Too few cells on row '%s'."), *RowName.ToString());			
		}
	}
	return true;
}
