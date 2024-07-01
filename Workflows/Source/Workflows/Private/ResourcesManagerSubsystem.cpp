// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcesManagerSubsystem.h"

#include "IVExcelWorkBook.h"
#include "IVRuntimeDataTableLibrary.h"
#include "IVXunFeiNodes.h"

DEFINE_LOG_CATEGORY(LogResourcesManagerSubsystem);

void UResourcesManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UResourcesManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
}

void UResourcesManagerSubsystem::UpdateQAExcelBookManually(const FString& InFileName, const FString& InExcelBookTitle, URunTimeTable* InRunTimeTable)
{
	const FString ProjectDir = FPaths::ProjectDir();
	const FString ExcelDir = ProjectDir + FString(TEXT("Resources/Excel/"));
	const FString FilePath = ExcelDir + InFileName;

	UIVExcelWorkBook* ExcelBook = UIVExcelWorkBook::LoadWorkBookFromFile(FilePath);
	
	if(!ExcelBook->GetSheetWithTitle(InRunTimeTable, InExcelBookTitle))
	{
		UE_LOG(LogResourcesManagerSubsystem, Error, TEXT("ThreadID:%d, %s: Parse excel book error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
		return;
	}

	TArray<FName> RowNames = InRunTimeTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		const uint8* RowDataPtr = InRunTimeTable->FindRowUnchecked(RowName);
		
		// DataTableUtils::GetPropertyValueAsString(, RowDataPtr);
	}
	
}

bool UResourcesManagerSubsystem::UpdateOrAddLocalAudioFile_QA(const FString& InVoiceText, const FTTSParameters& InTTSParameters, const FString& InFilePath)
{

	FXunFeiTTSOptions XunFeiTTSOptions;
	XunFeiTTSOptions.vcn = InTTSParameters.Vcn;
	XunFeiTTSOptions.speed = InTTSParameters.Speed;
	XunFeiTTSOptions.volume = InTTSParameters.Pitch;
	XunFeiTTSOptions.pitch = InTTSParameters.Pitch;
	
	return true;
}
