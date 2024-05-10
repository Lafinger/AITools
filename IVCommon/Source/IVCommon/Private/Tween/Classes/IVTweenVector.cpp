// Copyright 2023 AIRT,  All Rights Reserved.


#include "Tween/Classes/IVTweenVector.h"

UIVTweenVector::UIVTweenVector(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,StartKey(0.0f)
	,ChangeKey(1.0f)
	,OriginStartKey(0.0f)
{
	
}

void UIVTweenVector::SetInitialKey(const FIVTweenVectorGetterdDelegate IntGetter,
	const FIVTweenVectorSetterdDelegate InSetter,  const FVector InEndKey, float InDuration)
{
	Duration=InDuration;
	Getter=IntGetter;
	Setter=InSetter;
	EndValue=InEndKey;
}

void UIVTweenVector::OnStartGetValue()
{
	if(Getter.IsBound())
	{
		StartValue=Getter.Execute();
	}
	OriginStartKey=StartValue;
}

void UIVTweenVector::TweenAndApplyValue(float CurrentTime)
{
	float Alpha=OnEaseFunction.Execute(CurrentTime,StartKey,ChangeKey,Duration);
	FVector Value = FMath::Lerp(StartValue, EndValue, Alpha);
	Setter.ExecuteIfBound(Value);
}

void UIVTweenVector::SetValueForIncremental()
{
	FVector DiffValue = EndValue - StartValue;
	StartValue = EndValue;
	EndValue += DiffValue;
}

void UIVTweenVector::SetOriginValueForReverse()
{
	FVector DiffValue = EndValue - StartValue;
	StartValue = OriginStartKey;
	EndValue = DiffValue+OriginStartKey;
}
