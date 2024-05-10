// Copyright 2023 AIRT,  All Rights Reserved.


#include "Tween/IVTweenSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Tween/IVTweenBase.h"
DECLARE_CYCLE_STAT(TEXT("IVTween Update"), STAT_Update, STATGROUP_IVTween);

void UIVTweenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UIVTweenSubsystem::Deinitialize()
{
	Super::Deinitialize();
	TweenerList.Empty();
}

bool UIVTweenSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UIVTweenSubsystem::Tick(float DeltaTime)
{
	if (TickPaused == false)
	{
		OnTick(DeltaTime);
	}
}

ETickableTickType UIVTweenSubsystem::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool UIVTweenSubsystem::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject);
}

TStatId UIVTweenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UIVTweenSubsystem, STATGROUP_Tickables);
}

UWorld* UIVTweenSubsystem::GetTickableGameObjectWorld() const
{
	return FTickableGameObject::GetTickableGameObjectWorld();
}

UIVTweenSubsystem* UIVTweenSubsystem::GetTweenSubsystee(UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance->GetSubsystem<UIVTweenSubsystem>();
}

void UIVTweenSubsystem::CustomTick(float DeltaTime)
{
	OnTick(DeltaTime);
}

void UIVTweenSubsystem::DisableTick()
{
	TickPaused = true;
}

void UIVTweenSubsystem::EnableTick()
{
	TickPaused = false;
}

void UIVTweenSubsystem::KillAllTweens()
{
	for(UIVTweenBase *Item:TweenerList)
	{
		if(IsValid(Item))
		{
			Item->Kill();
		}
	}
}

UIVTweenBase* UIVTweenSubsystem::To(const FIVTweenFloatGetterdDelegate& Getter,
	const FIVTweenFloatSetterdDelegate& Setter, float EndValue, float Duration)
{
	UIVTweenFloat* TweenFloat=NewObject<UIVTweenFloat>();
	TweenFloat->SetInitialKey(Getter,Setter,EndValue,Duration);
	TweenerList.Add(TweenFloat);

	return TweenFloat;
}

UIVTweenBase* UIVTweenSubsystem::To(const FIVTweenVectorGetterdDelegate& Getter,
	const FIVTweenVectorSetterdDelegate& Setter, FVector EndValue, float Duration)
{
	UIVTweenVector* TweenVector=NewObject<UIVTweenVector>();
	TweenVector->SetInitialKey(Getter,Setter,EndValue,Duration);
	TweenerList.Add(TweenVector);

	return TweenVector;
}

UIVTweenBase* UIVTweenSubsystem::To(const FIVTweenVector2DGetterdDelegate& Getter,
	const FIVTweenVector2DSetterdDelegate& Setter, FVector2D EndValue, float Duration)
{
	UIVTweenVector2D* TweenVector=NewObject<UIVTweenVector2D>();
	TweenVector->SetInitialKey(Getter,Setter,EndValue,Duration);
	TweenerList.Add(TweenVector);

	return TweenVector;
}

UIVTweenBase* UIVTweenSubsystem::To(const FIVTweenRotatorGetterdDelegate& Getter,
	const FIVTweenRotatorSetterdDelegate& Setter, FRotator EndValue, float Duration)
{
	UIVTweenRotator* TweenRotato=NewObject<UIVTweenRotator>();
	TweenRotato->SetInitialKey(Getter,Setter,EndValue,Duration);
	TweenerList.Add(TweenRotato);

	return TweenRotato;
}

UIVTweenBase* UIVTweenSubsystem::To(const FIVTweenTransformGetterdDelegate& Getter,
	const FIVTweenTransformSetterdDelegate& Setter, FTransform EndValue, float Duration)
{
	UIVTweenTransform* TweenTransform=NewObject<UIVTweenTransform>();
	TweenTransform->SetInitialKey(Getter,Setter,EndValue,Duration);
	TweenerList.Add(TweenTransform);

	return TweenTransform;
}

UIVTweenBase* UIVTweenSubsystem::To(const FIVTweenIntGetterdDelegate& Getter, const FIVTweenIntSetterdDelegate& Setter,
                                    int32 EndValue, float Duration)
{
	UIVTweenInt* TweenInt=NewObject<UIVTweenInt>();
	TweenInt->SetInitialKey(Getter,Setter,EndValue,Duration);
	TweenerList.Add(TweenInt);

	return TweenInt;
}

UIVTweenBase* UIVTweenSubsystem::To(const FIVTweenMaterialScalarGetterDelegate& Getter,
                                    const FIVTweenMaterialScalarSetterDelegate& Setter, float EndValue, float Duration, int32 ParameterIndex)
{
	UIVTweenMaterialScalar* TweenMaterialScalar=NewObject<UIVTweenMaterialScalar>();
	TweenMaterialScalar->SetInitialValue(Getter,Setter,EndValue,Duration,ParameterIndex);
	TweenerList.Add(TweenMaterialScalar);

	return TweenMaterialScalar;
}

void UIVTweenSubsystem::OnTick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_Update);
	
	auto Count = TweenerList.Num();
	for (int32 i = 0; i < Count; i++)
	{
		UIVTweenBase* Tweener = TweenerList[i];
		if (!IsValid(Tweener))
		{
			TweenerList.RemoveAt(i);
			i--;
			Count--;
		}
		else
		{
			if (Tweener->ToNext(DeltaTime) == false)
			{
				TweenerList.RemoveAt(i);
				Tweener->ConditionalBeginDestroy();
				i--;
				Count--;
			}
		}
	}
	if (OnMulticastUpdateEvent.IsBound())
	{
		OnMulticastUpdateEvent.Broadcast(DeltaTime);
	}
	
}
