// Copyright 2023 AIRT All Rights Reserved.


#include "IVExcelWorkBook.h"

#include "IVRuntimeDataTableModule.h"
#include "IVRunTimeDataTableXLSL.h"
#include "RunTimeDataTable.h"


UIVExcelWorkBook::UIVExcelWorkBook(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	WorkBook= new xlnt::workbook();
}

UIVExcelWorkBook::~UIVExcelWorkBook()
{
	delete WorkBook;
	WorkBook=nullptr;
}

UIVExcelWorkBook* UIVExcelWorkBook::LoadWorkBookFromFile(FString FilePath)
{
	
	UIVExcelWorkBook *WorkBook=NewObject<UIVExcelWorkBook>();
	
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		UE_LOG(LogIVExcel, Error, TEXT("Function: %s; Message: Invalid file path:%s"), *FString(__func__), *FilePath);
		return nullptr;
	}
	
	FString Extension = FPaths::GetExtension(FilePath);
	if(!Extension.Equals(TEXT("xlsx"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogIVExcel, Error, TEXT("Function: %s; Message: Extension %s is not supported."), *FString(__func__), *Extension);
		return nullptr;
	}
	
	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *FilePath, FILEREAD_AllowWrite))
	{
		return nullptr;
	}
    
	std::vector<std::uint8_t> CopyData;
	CopyData.reserve(FileData.Num());
	for (uint8 Val : FileData)
	{
		CopyData.push_back(Val);
	}
    
	WorkBook->WorkBook->load(CopyData);
	WorkBook->InitSheets();
	
	return  WorkBook;
}

int32 UIVExcelWorkBook::GetSheetCount() const
{
	return WorkBook->sheet_count();
}

bool UIVExcelWorkBook::GetSheetWithIndex(URunTimeTable* &InSheet,int32 Index)
{
	if (Sheets.IsValidIndex(Index))
	{
		InSheet->WorkSheet=Sheets[Index];
		InSheet->InitializeExecel(this,InSheet->WorkSheet);
		return FIVDataTableImporterXLSL(InSheet,this).ReadTable(true);
		 
	}
	
	return false;
}

bool UIVExcelWorkBook::GetSheetWithTitle(URunTimeTable*& InSheet, FString Title)
{
	std::string Str = TCHAR_TO_UTF8(*Title);

	for (xlnt::worksheet Sheet : Sheets)
	{
		if (Sheet.title() == Str)
		{
			InSheet->WorkSheet=Sheet;
			InSheet->InitializeExecel(this,InSheet->WorkSheet);
			return FIVDataTableImporterXLSL(InSheet,this).ReadTable(true);
		}
	}
	UE_LOG(LogIVExcel,Error,TEXT("Sheet is not valid %s"),*Title);
	return false;
}

void UIVExcelWorkBook::InitSheets()
{
	Sheets.Empty();

	size_t Count = WorkBook->sheet_count();
	for (size_t i = 0; i < Count; ++i)
	{
		xlnt::worksheet WS = WorkBook->sheet_by_index(i);
		Sheets.Add(WS);
	}
	
}
