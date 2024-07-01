// Copyright 2023 AIRT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RunTimeDataTable.h"
#include "UObject/NoExportTypes.h"

THIRD_PARTY_INCLUDES_START
#include "xlnt/xlnt.hpp"
THIRD_PARTY_INCLUDES_END

#include "IVExcelWorkBook.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class IVRUNTIMEDATATABLE_API UIVExcelWorkBook : public UObject
{
	GENERATED_BODY()

public:
	UIVExcelWorkBook(const FObjectInitializer& ObjectInitializer);
	virtual ~UIVExcelWorkBook();
	
public:
	UFUNCTION(BlueprintCallable, Category = "IVData|WorkBook")
	static UPARAM(DisplayName = "ExcelWorkBook") UIVExcelWorkBook* LoadWorkBookFromFile(FString FilePath);

	/**获取Sheet数量*/
	UFUNCTION(BlueprintCallable, Category = "IVData|WorkBook")
	int32 GetSheetCount() const;

	/**通过序列获取Sheet*/
	UFUNCTION(BlueprintCallable, Category = "IVData|WorkBook")
	bool GetSheetWithIndex(UPARAM(ref)URunTimeTable*& Sheet,int32 Index);

	/**通过Sheet名字获取*/
	UFUNCTION(BlueprintCallable, Category = "IVData|WorkBook")
	bool GetSheetWithTitle(UPARAM(ref)URunTimeTable*& Sheet,FString Title);
	
private:
	void InitSheets();

private:
	TArray<xlnt::worksheet> Sheets;
	
	xlnt::workbook* WorkBook = nullptr;
};
