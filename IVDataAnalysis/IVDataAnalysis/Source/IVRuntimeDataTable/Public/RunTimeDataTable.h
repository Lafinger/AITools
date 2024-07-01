// Copyright 2023 AIRT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

THIRD_PARTY_INCLUDES_START
#include "xlnt/xlnt.hpp"
THIRD_PARTY_INCLUDES_END

#include "RunTimeDataTable.generated.h"

class UIVExcelWorkBook;


UCLASS( BlueprintType)
class IVRUNTIMEDATATABLE_API URunTimeTable : public UObject
{
	GENERATED_UCLASS_BODY()
	
	virtual ~URunTimeTable() {};
	//friend FIVDataTableImporterCSV;
	
	/** 表的每一行使用的结构*/
	UPROPERTY(VisibleAnywhere, Category=DataTable, meta=(DisplayThumbnail="false"))
	TObjectPtr<UScriptStruct>		RowStruct;

	/** 导入数据中用作键的显式字段。 如果为空，则使用 JSON 的名称和 CSV 找到的第一个字段 */
	UPROPERTY(EditAnywhere, Category=ImportOptions)
	FString ImportKeyField;

	/** 设置为 true 以忽略导入数据中的额外字段，如果为 false 则会发出警告 */
	UPROPERTY(EditAnywhere, Category=ImportOptions)
	bool bIgnoreExtraFields ;
	
	/** 设置为 true 以忽略任何预期但缺失的字段，如果为 false 则会警告它们 */
	UPROPERTY(EditAnywhere, Category = ImportOptions)
	bool bIgnoreMissingFields;

public:
	TMap<FName, uint8*>	RowMap;
	
public:
	
	void Init(UScriptStruct* InRowStruct,FString InImportKeyField,bool bInIgnoreExtraFields,bool bInIgnoreMissingFields)
	{
		RowStruct=InRowStruct;
		ImportKeyField=InImportKeyField;
		bIgnoreExtraFields=bInIgnoreExtraFields;
		bIgnoreMissingFields=bInIgnoreMissingFields;
	}
	
	bool IsValid()
	{
		if(RowStruct.Get())
		{
			return true;
		}
		return false;
	}
public:
	
	/**获取表中的Property数组*/
	TArray<FProperty*> GetTablePropertyArray(const TArray<const TCHAR*>& Cells, UStruct* RowStruct, int32 KeyColumn = 0);

	TArray<FProperty*> GetTablePropertyArray(const TArray< FString>& Cells, UStruct* RowStruct, int32 KeyColumn = 0);
	
	TArray<FName> GetRowNames() const;
	
	/**清空数据*/
	void EmptyTable();

	bool CreateTableFromCSVString(const FString& InString,bool bClearData = true);

	bool CreateTableFromJSONString(const FString& InString,bool bClearData = true);

	void AddRowInternal(FName RowName, uint8* RowDataPtr);
	

	uint8* FindRowUnchecked(FName RowName) const;
	
	const TMap<FName, uint8*>& GetRowMap() const ;
	
	const UScriptStruct* GetRowStruct() const ;
	
protected:
	UScriptStruct& GetEmptyUsingStruct() const;


//以下都是做excel的函数
public:

	xlnt::worksheet WorkSheet;
	
	void InitializeExecel(UIVExcelWorkBook* InWorkBook, xlnt::worksheet InWorkSheet);

public:

	/**获取row数量，只在做解析xlsl使用*/
	UFUNCTION(BlueprintPure, Category = "IVData|DataTable|Worksheet")
	int32 GetRowCount(bool bSkipNull = true)const;

	/**获取row第一行位置，只在做解析xlsl使用*/
	UFUNCTION(BlueprintPure,Category = "IVData|DataTable|Worksheet")
	int32 GetLowestRow() const;

	/**获取row最后一行位置，只在做解析xlsl使用*/
	UFUNCTION(BlueprintPure, Category = "IVData|DataTable|Worksheet")
	int32 GetHighestRow() const;

	/**以FString形式返回单个数据，只在做解析xlsl使用*/
	UFUNCTION(BlueprintPure, Category = "IVData|DataTable|Worksheet")
	FString GetStringCellValue(int32 Column = 1, int32 Row = 1) const;

	/**获取Column数量，只在做解析xlsl使用*/
	UFUNCTION(BlueprintPure, Category = "IVData|DataTable|Worksheet")
	int32 GetColumnCount(bool bSkipNull = true)const;

	UFUNCTION(BlueprintPure, Category = "IVData|DataTable|Worksheet")
	int32 GetHighesColumn() const;
public:
	
	std::string GetCellValue(int32 Column = 1, int32 Row = 1)const;

private:

	TWeakObjectPtr<UIVExcelWorkBook> WorkBook;
	
};


