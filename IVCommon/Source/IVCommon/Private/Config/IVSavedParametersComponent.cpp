// Copyright 2023 AIRT,  All Rights Reserved.


#include "Config/IVSavedParametersComponent.h"

#include "iVisualFileHeplerBPLibrary.h"
#include "Config/IVConfig.h"
#include "Misc/FileHelper.h"


UIVSavedParametersComponent::UIVSavedParametersComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

#if WITH_EDITOR
	FCoreUObjectDelegates::OnObjectPropertyChanged.AddUObject(this, &UIVSavedParametersComponent::OnActorEditChangeProperty);
#endif 
	if(GetOwner())
	{
		FilePath=UiVisualFileHeplerBPLibrary::GetIVActorJsonConfigDir()/GetOwner()->GetName()+".json";
	}
}

void UIVSavedParametersComponent::PostReinitProperties()
{

	Super::PostInitProperties();
}


#if WITH_EDITOR
void UIVSavedParametersComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UIVSavedParametersComponent::OnActorEditChangeProperty(UObject* Actor, FPropertyChangedEvent& PropertyChangedEvent)
{
	if(!GetOwner())
	{
		return;
	}

	if(Actor->GetName()==GetOwner()->GetName())
	{
		if(GetOwner()->GetName().StartsWith("Default__C")|| GetOwner()->GetName().EndsWith(TEXT("_C_0")))
		{
			return;
		}
			
		FIVConfig Config;
		FString Result;
		Config.SetRootUObject(GetOwner()->GetClass(),GetOwner());
		if (!Config.SaveToString(Result))
		{
			return ;
		}
		
		FFileHelper::SaveStringToFile(Result, *FilePath);
	}
}
#endif

void UIVSavedParametersComponent::BeginPlay()
{
	Super::BeginPlay();

	if(!GetOwner())
	{
		return;
	}
	
	if(HasAnyFlags(RF_ClassDefaultObject | RF_Transient))
	{
		return;
	}

	if(GetOwner()->GetName().StartsWith("Default__C")|| GetOwner()->GetName().EndsWith(TEXT("_C_0")))
	{
		return;
	}
	
	FIVConfig Config;
	FString Contents;
	if(!FFileHelper::LoadFileToString(Contents, *FilePath))
	{
		return;
	}
	
	if (!Config.LoadFromString(Contents))
	{
		return;
	}

	Config.TryGetRootUObject(GetOwner()->GetClass(),GetOwner());
}
