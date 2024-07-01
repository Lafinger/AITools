// Copyright 2023 AIRT All Rights Reserved.


#include "IVRuntimeDataTableLibrary.h"

#include "IVRuntimeDataTableModule.h"


bool UIVRuntimeDataTableLibrary::LoadTableFromCSVFile(URunTimeTable*& Table, FString FilePath)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		UE_LOG(LogIVDataTable, Error, TEXT("Function: %s; Message: Invalid file path:%s"), *FString(__func__), *FilePath);
		return false;
	}
	
	FString Extension = FPaths::GetExtension(FilePath);
	if(!Extension.Equals(TEXT("csv"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogIVDataTable, Error, TEXT("Function: %s; Message: Extension %s is not supported."), *FString(__func__), *Extension);
		return false;
	}
	
	FString StrData;
	if(FFileHelper::LoadFileToString(StrData,*FilePath))
	{
		LoadTableFromCSVString(Table,StrData);
		return true;
	}

	return false;
}

bool UIVRuntimeDataTableLibrary::AppendRowsFromCSVString(URunTimeTable*& Table, FString CSV)
{
	return Table->CreateTableFromCSVString(CSV,false);
}

bool UIVRuntimeDataTableLibrary::AppendRowsFromJSONString(URunTimeTable*& Table, FString JSON)
{
	return Table->CreateTableFromJSONString(JSON,false);
}

bool UIVRuntimeDataTableLibrary::DoesDataTableRowExist(URunTimeTable*& Table, FName RowName)
{
	if(!Table->IsValid())
	{
		return false;
	}else if (Table->RowStruct == nullptr)
	{
		return false;
	}
	return Table->GetRowMap().Find(RowName) != nullptr;
}

void UIVRuntimeDataTableLibrary::GetTableRowNames(  URunTimeTable*& Table, TArray<FName>& OutRowNames)
{
	Table->RowMap.GetKeys(OutRowNames);
}

bool UIVRuntimeDataTableLibrary::LoadTableFromCSVString(URunTimeTable*& Table, FString CSV)
{
	return  Table->CreateTableFromCSVString(CSV,true);
}

bool UIVRuntimeDataTableLibrary::LoadTableFromJSONFile(URunTimeTable*& Table, FString FilePath)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		UE_LOG(LogIVDataTable, Error, TEXT("Function: %s; Message: Invalid file path:%s"), *FString(__func__), *FilePath);
		return false;
	}
	
	FString Extension = FPaths::GetExtension(FilePath);
	if(!Extension.Equals(TEXT("csv"), ESearchCase::IgnoreCase))
	{
		UE_LOG(LogIVDataTable, Error, TEXT("Function: %s; Message: Extension %s is not supported."), *FString(__func__), *Extension);
		return false;
	}
	
	FString StrData;
	FFileHelper::LoadFileToString(StrData,*FilePath);

	LoadTableFromJSONString(Table,StrData);
	return true;
}

bool UIVRuntimeDataTableLibrary::LoadTableFromJSONString(URunTimeTable*& Table, FString JSON)
{
	return  Table->CreateTableFromJSONString(JSON,true);
}

void UIVRuntimeDataTableLibrary::RemoveTableRow(URunTimeTable*& Table, FName RowName)
{
	Table->RowMap.Remove(RowName);
}

void UIVRuntimeDataTableLibrary::Clear(URunTimeTable*& Table)
{
	Table->RowMap.Empty();
}

URunTimeTable* UIVRuntimeDataTableLibrary::CreateRunTimeTable(UScriptStruct* Struct, FString ImportKeyField,
                                                             bool bIgnoreExtraFields, bool bIgnoreMissingFields)
{
	URunTimeTable* RunTimeTable= NewObject<URunTimeTable>();

	RunTimeTable->Init(Struct,ImportKeyField,bIgnoreExtraFields,bIgnoreMissingFields);

	return RunTimeTable;
}

bool UIVRuntimeDataTableLibrary::GetRowFromTable( URunTimeTable*& Table, FName RowName, int& Index, void* OutRowPtr)
{
	bool bFoundRow = false;
	
	if (OutRowPtr && Table->IsValid())
	{
		void* RowPtr = Table->FindRowUnchecked(RowName);

		if (RowPtr != nullptr)
		{
			const UScriptStruct* StructType = Table->GetRowStruct();

			if (StructType != nullptr)
			{
				StructType->CopyScriptStruct(OutRowPtr, RowPtr);
				TArray<FName> Keys;
				Index = Table->RowMap.GetKeys(Keys);
				Index=Keys.Find(RowName);
				bFoundRow = true;
			}
		}
	}

	return bFoundRow;
}


