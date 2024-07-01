// Copyright 2023 AIRT All Rights Reserved.


#include "RunTimeDataTable.h"

#include "IVRunTimeDataTableJson.h"
#include "IVRuntimeDataTableModule.h"
#include "IVRunTimeDataTableCSV.h"
#include "IVExcelWorkBook.h"


URunTimeTable::URunTimeTable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	,bIgnoreExtraFields(true)
	,bIgnoreMissingFields(true)
{
	
}
TArray<FProperty*> URunTimeTable::GetTablePropertyArray(const TArray<const TCHAR*>& Cells, UStruct* InRowStruct,
                                                        int32 KeyColumn)
{
	TArray<FProperty*> ColumnProps;

	// 通过staruct获取参数名
	TArray<FName> ExpectedPropNames = DataTableUtils::GetStructPropertyNames(InRowStruct);

	// 必须大于2，因为有一个是rowname
	if(Cells.Num() > 1)
	{
		ColumnProps.AddZeroed( Cells.Num() );
		
		TArray<FString> TempPropertyImportNames;
		for (int32 ColIdx = 0; ColIdx < Cells.Num(); ++ColIdx)
		{
			if (ColIdx == KeyColumn)
			{
				continue;
			}

			const TCHAR* ColumnValue = Cells[ColIdx];

			FName PropName = DataTableUtils::MakeValidName(ColumnValue);
			if(PropName == NAME_None)
			{
				UE_LOG(LogIVDataTable,Warning,TEXT("Missing name for column %d."), ColIdx);
			}
			else
			{
				FProperty* ColumnProp = FindFProperty<FProperty>(InRowStruct, PropName);

				for (TFieldIterator<FProperty> It(InRowStruct); It && !ColumnProp; ++It)
				{
					DataTableUtils::GetPropertyImportNames(*It, TempPropertyImportNames);
					ColumnProp = TempPropertyImportNames.Contains(ColumnValue) ? *It : nullptr;
				}

				// 若没找到此参数名
				if(ColumnProp == nullptr)
				{
					if (!bIgnoreExtraFields)
					{
						UE_LOG(LogIVDataTable,Warning,TEXT("Cannot find Property for column '%s' in struct '%s'."), *PropName.ToString(), *InRowStruct->GetName());
					}
				}
				//若找到!
				else
				{
					// 检查是否已经检测到此数据
					if(ColumnProps.Contains(ColumnProp))
					{
						UE_LOG(LogIVDataTable,Warning,TEXT("Duplicate column '%s'."), *ColumnProp->GetName());
					}
					//检测是否支持此数据
					else if( !DataTableUtils::IsSupportedTableProperty(ColumnProp) )
					{
						UE_LOG(LogIVDataTable,Warning,TEXT("Unsupported Property type for struct member '%s'."), *ColumnProp->GetName());
					}
					else
					{
						ColumnProps[ColIdx] = ColumnProp;
					}
					ExpectedPropNames.Remove(ColumnProp->GetFName());
				}
			}
		}
	}
	if (!bIgnoreMissingFields)
	{
		for (int32 PropIdx = 0; PropIdx < ExpectedPropNames.Num(); PropIdx++)
		{
			const FProperty* const ColumnProp = FindFProperty<FProperty>(InRowStruct, ExpectedPropNames[PropIdx]);

			const FString DisplayName = DataTableUtils::GetPropertyExportName(ColumnProp);
			UE_LOG(LogIVDataTable,Warning,TEXT("Expected column '%s' not found in input."), *DisplayName);
		}

	}
	return ColumnProps;
}

TArray<FProperty*> URunTimeTable::GetTablePropertyArray(const TArray< FString>& Cells, UStruct* InRowStruct,
	int32 KeyColumn)
{
	TArray<FProperty*> ColumnProps;

	// 通过staruct获取参数名
	TArray<FName> ExpectedPropNames = DataTableUtils::GetStructPropertyNames(InRowStruct);

	// 必须大于2，因为有一个是rowname
	if(Cells.Num() > 1)
	{
		ColumnProps.AddZeroed( Cells.Num() );
		
		TArray<FString> TempPropertyImportNames;
		for (int32 ColIdx = 0; ColIdx < Cells.Num(); ++ColIdx)
		{
			if (ColIdx == KeyColumn)
			{
				continue;
			}

			const TCHAR* ColumnValue = *Cells[ColIdx];

			FName PropName = DataTableUtils::MakeValidName(ColumnValue);
			if(PropName == NAME_None)
			{
				UE_LOG(LogIVDataTable,Warning,TEXT("Missing name for column %d."), ColIdx);
			}
			else
			{
				FProperty* ColumnProp = FindFProperty<FProperty>(InRowStruct, PropName);

				for (TFieldIterator<FProperty> It(InRowStruct); It && !ColumnProp; ++It)
				{
					DataTableUtils::GetPropertyImportNames(*It, TempPropertyImportNames);
					ColumnProp = TempPropertyImportNames.Contains(ColumnValue) ? *It : nullptr;
				}

				// 若没找到此参数名
				if(ColumnProp == nullptr)
				{
					if (!bIgnoreExtraFields)
					{
						UE_LOG(LogIVDataTable,Warning,TEXT("Cannot find Property for column '%s' in struct '%s'."), *PropName.ToString(), *InRowStruct->GetName());
					}
				}
				//若找到!
				else
				{
					// 检查是否已经检测到此数据
					if(ColumnProps.Contains(ColumnProp))
					{
						UE_LOG(LogIVDataTable,Warning,TEXT("Duplicate column '%s'."), *ColumnProp->GetName());
					}
					//检测是否支持此数据
					else if( !DataTableUtils::IsSupportedTableProperty(ColumnProp) )
					{
						UE_LOG(LogIVDataTable,Warning,TEXT("Unsupported Property type for struct member '%s'."), *ColumnProp->GetName());
					}
					else
					{
						ColumnProps[ColIdx] = ColumnProp;
					}
					ExpectedPropNames.Remove(ColumnProp->GetFName());
				}
			}
		}
	}
	if (!bIgnoreMissingFields)
	{
		for (int32 PropIdx = 0; PropIdx < ExpectedPropNames.Num(); PropIdx++)
		{
			const FProperty* const ColumnProp = FindFProperty<FProperty>(InRowStruct, ExpectedPropNames[PropIdx]);

			const FString DisplayName = DataTableUtils::GetPropertyExportName(ColumnProp);
			UE_LOG(LogIVDataTable,Warning,TEXT("Expected column '%s' not found in input."), *DisplayName);
		}

	}
	return ColumnProps;
}

TArray<FName> URunTimeTable::GetRowNames() const
{
	TArray<FName> Keys;
	RowMap.GetKeys(Keys);
	return Keys;
}

void URunTimeTable::EmptyTable()
{
	UScriptStruct& EmptyUsingStruct = GetEmptyUsingStruct();
	
	for (auto RowIt = RowMap.CreateIterator(); RowIt; ++RowIt)
	{
		uint8* RowData = RowIt.Value();
		EmptyUsingStruct.DestroyStruct(RowData);
		FMemory::Free(RowData);
	}

	RowMap.Empty();
}

bool URunTimeTable::CreateTableFromCSVString(const FString& InString,bool bClearData /**= true*/)
{

	return FIVDataTableImporterCSV(*this,InString).ReadTable(bClearData);
}

bool URunTimeTable::CreateTableFromJSONString(const FString& InString, bool bClearData)
{
	return FIVDataTableImporterJSON(*this,InString).ReadTable(bClearData);
}

UScriptStruct& URunTimeTable::GetEmptyUsingStruct() const
{
	UScriptStruct* EmptyUsingStruct = RowStruct;
	if (!EmptyUsingStruct)
	{
		EmptyUsingStruct = FTableRowBase::StaticStruct();
	}

	return *EmptyUsingStruct;
}


void URunTimeTable::AddRowInternal(FName RowName, uint8* RowDataPtr)
{
	RowMap.Add(RowName, RowDataPtr);
}

uint8* URunTimeTable::FindRowUnchecked(FName RowName) const
{
	if(RowStruct == nullptr)
	{
		return nullptr;
	}
	
	uint8* const* RowDataPtr = GetRowMap().Find(RowName);

	if(RowDataPtr == nullptr)
	{
		return nullptr;
	}

	return *RowDataPtr;
}

const TMap<FName, uint8*>& URunTimeTable::GetRowMap() const
{
	 return RowMap; 
}

const UScriptStruct* URunTimeTable::GetRowStruct() const
{
	return RowStruct;
}

void URunTimeTable::InitializeExecel(UIVExcelWorkBook* InWorkBook, xlnt::worksheet InWorkSheet)
{
	WorkSheet=InWorkSheet;
	WorkBook=InWorkBook;
}

int32 URunTimeTable::GetRowCount(bool bSkipNull) const
{
	return (int32)WorkSheet.rows(bSkipNull).length();
}

int32 URunTimeTable::GetLowestRow() const
{
	return  WorkSheet.lowest_row();
}

int32 URunTimeTable::GetHighestRow() const
{
	return  WorkSheet.highest_row();
}

FString URunTimeTable::GetStringCellValue(int32 Column, int32 Row) const
{
	return UTF8_TO_TCHAR(GetCellValue(Column,Row).c_str());
}

int32 URunTimeTable::GetColumnCount(bool bSkipNull) const
{
	return (int32)WorkSheet.columns(bSkipNull).length();
}

int32 URunTimeTable::GetHighesColumn() const
{
	return WorkSheet.highest_column().index;
}

std::string URunTimeTable::GetCellValue(int32 Column, int32 Row) const
{
	xlnt::cell_reference Reference(Column,Row);


	xlnt::worksheet* mutableData = (xlnt::worksheet*)&WorkSheet;
	mutableData->reserve(Row);

	xlnt::cell c = mutableData->cell(Reference);
	
	return c.to_string();
}


