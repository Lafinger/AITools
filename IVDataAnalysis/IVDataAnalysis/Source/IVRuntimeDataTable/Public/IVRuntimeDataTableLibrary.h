// Copyright 2023 AIRT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RunTimeDataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IVRuntimeDataTableLibrary.generated.h"

/**
 * 
 */
UCLASS()
class IVRUNTIMEDATATABLE_API UIVRuntimeDataTableLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 创建一个新的runtime表
	 * @param BaseStruct 只引用结构体参数，其数据不做参考
	 * @return RunTimeTable
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, meta=(CustomStructureParam = "BaseStruct", Keywords = "New Runtime Data Table"), Category = "IVData|DataTable")
	static URunTimeTable* CreateRunTimeTable( UPARAM(ref) int32& BaseStruct,FString ImportKeyField,bool bIgnoreExtraFields,bool bIgnoreMissingFields);

	DECLARE_FUNCTION(execCreateRunTimeTable)
	{
		Stack.Step(Stack.Object, NULL);
		FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
		void* StructPtr = Stack.MostRecentPropertyAddress;
		UScriptStruct* ScriptStruct = StructProperty->Struct;
		P_GET_PROPERTY(FStrProperty,ImportKeyField);
		P_GET_UBOOL(bIgnoreExtraFields);
		P_GET_UBOOL(bIgnoreMissingFields);
		P_FINISH;
		
		P_NATIVE_BEGIN;
		*(URunTimeTable**)RESULT_PARAM = CreateRunTimeTable(ScriptStruct,ImportKeyField,bIgnoreExtraFields,bIgnoreMissingFields);
		P_NATIVE_END;
	}

	/**
	 * 从本地文件加载Table，文本格式为scv
	 * @param Table RunTimeTable
	 * @param FilePath 文件路径
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = "IVData|DataTable",DisplayName="Load Table From CSV File")
	static bool LoadTableFromCSVFile( UPARAM(ref) URunTimeTable*& Table, FString FilePath);

	/**
	* 从字符串加载数据
	* @param Table RunTimeTable
	* @param CSV 以字符串的数据输入
	*/
	UFUNCTION(BlueprintCallable, Category = "IVData|DataTable",DisplayName="Load Table From CSV String")
	static bool  LoadTableFromCSVString( UPARAM(ref) URunTimeTable*& Table, FString CSV);

	/**
	 * 从本地文件加载Table，文本格式为json
	 * @param Table RunTimeTable
	 * @param FilePath 文件路径
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, Category = "IVData|DataTable",DisplayName="Load Table From JSON File")
	static bool LoadTableFromJSONFile( UPARAM(ref) URunTimeTable*& Table, FString FilePath);

	/**
	* 从字符串加载数据
	* @param Table RunTimeTable
	* @param JSON 以字符串的数据输入
	*/
	UFUNCTION(BlueprintCallable, Category = "IVData|DataTable",DisplayName="Load Table From JSON String")
	static bool  LoadTableFromJSONString( UPARAM(ref) URunTimeTable*& Table, FString JSON);
	
	/**
	 * 做增量数据,若数据key值重复则做row值替换
	 * @param Table RunTimeTable
	 * @param CSV 以字符串的数据输入
	 */
	UFUNCTION(BlueprintCallable, Category = "IVData|DataTable",DisplayName="Append Rows From CSV String")
	static bool AppendRowsFromCSVString( UPARAM(ref) URunTimeTable*& Table, FString CSV);

	/**
	 * 做增量数据,若数据key值重复则做row值替换
	 * @param Table RunTimeTable
	 * @param JSON 以字符串的数据输入
	 */
	UFUNCTION(BlueprintCallable, Category = "IVData|DataTable",DisplayName="Append Rows From JSON String")
	static bool AppendRowsFromJSONString( UPARAM(ref) URunTimeTable*& Table, FString JSON);

	UFUNCTION(BlueprintCallable, Category = "IVData|DataTable")
	static  bool DoesDataTableRowExist(UPARAM(ref) URunTimeTable*& Table, FName RowName);
	
	/*
	 * 获取runtimetable的全部rowname
	 */
	UFUNCTION(BlueprintCallable, Category = "IVData|DataTable")
	static void  GetTableRowNames( UPARAM(ref) URunTimeTable*& Table,TArray<FName>& OutRowNames);

	/**
	 * 获取单个row的数据
	 * @param Table RunTimeTable
	 * @param RowName 需要获取数据的row名字
	 * @param Row 返回的数据
	 * @param Index 数据在数据集中序列
	 * @return 是否能获取
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, meta=(DisplayName = "Get Row From Table",CustomStructureParam = "Row"), Category = "IVData|DataTable")
	static bool GetRowFromTable( UPARAM(ref) URunTimeTable*& Table, FName RowName, int32& Row, int& Index);

	DECLARE_FUNCTION(execGetRowFromTable) {
		P_GET_OBJECT(URunTimeTable,Table);
		P_GET_PROPERTY(FNameProperty,RowName);
		Stack.Step(Stack.Object, NULL);
		FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
		void* ContainerPtr = Stack.MostRecentPropertyAddress;
		P_GET_PROPERTY_REF(FIntProperty,Index);
		P_FINISH;
		bool bSuccess = false;
		
		if (!Table->IsValid())
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("GetDataTableRow", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
		else if(StructProperty && ContainerPtr)
		{
			UScriptStruct* OutputType = StructProperty->Struct;
			const UScriptStruct* TableType  = Table->GetRowStruct();
		
			const bool bCompatible = (OutputType == TableType) || 
				(OutputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(OutputType, TableType));
			if (bCompatible)
			{
				P_NATIVE_BEGIN;
				bSuccess=GetRowFromTable(Table, RowName,Index, ContainerPtr);
				P_NATIVE_END;
			}
			else
			{
				FBlueprintExceptionInfo ExceptionInfo(
					EBlueprintExceptionType::AccessViolation,
					NSLOCTEXT("GetDataTableRow", "IncompatibleProperty", "Incompatible output parameter; the data table's type is not the same as the return type.")
					);
				FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
			}
		}
		else
		{
			FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("GetDataTableRow", "MissingOutputProperty", "Failed to resolve the output parameter for GetDataTableRow.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
		
		*(bool*)RESULT_PARAM = bSuccess;
	}

	/**添加row*/
	UFUNCTION(BlueprintCallable, CustomThunk, meta=(CustomStructureParam = "Row"), Category = "IVData|DataTable")
	static void AddRowToTable( UPARAM(ref) URunTimeTable*& Table, FName RowName, UPARAM(ref) const int32& Row);

	DECLARE_FUNCTION(execAddRowToTable)
	{
		P_GET_OBJECT(URunTimeTable,Table);
		P_GET_PROPERTY(FNameProperty,RowName);
		Stack.Step(Stack.Object, NULL);
		FStructProperty* StructProperty = CastField<FStructProperty>(Stack.MostRecentProperty);
		void* StructPtr = Stack.MostRecentPropertyAddress;
		UScriptStruct* ScriptStruct = StructProperty->Struct;
		uint8* StructInMap =  (uint8*)FMemory::Malloc(ScriptStruct->GetStructureSize());
		FMemory::Memcpy(StructInMap, StructPtr, ScriptStruct->GetStructureSize());

		Table->AddRowInternal(RowName,StructInMap);
		P_FINISH;
	}

	/**删除row*/
	UFUNCTION(BlueprintCallable, Category = "IVData|DataTable")
	static void RemoveTableRow( UPARAM(ref) URunTimeTable*& Table, FName RowName);

	/**删除所有的数据*/
	UFUNCTION(BlueprintCallable, Category = "IVData|DataTable")
	static void Clear( UPARAM(ref) URunTimeTable*& Table);
public:
	static URunTimeTable* CreateRunTimeTable( UScriptStruct* Struct,FString ImportKeyField,bool bIgnoreExtraFields,bool bIgnoreMissingFields);
	static  bool GetRowFromTable(URunTimeTable*& Table,FName RowName,int& Index, void* OutRowPtr);
};
