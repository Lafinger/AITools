// Copyright 2023 AIRT,  All Rights Reserved.


#include "Tween/Classes/IVTweenVector2D.h"

UIVTweenVector2D::UIVTweenVector2D(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,StartKey(0.0f)
	,ChangeKey(1.0f)
	,OriginStartKey(0.0f)
{
	
}

void UIVTweenVector2D::SetInitialKey(const FIVTweenVector2DGetterdDelegate IntGetter,
	const FIVTweenVector2DSetterdDelegate InSetter,  const FVector2D InEndKey, float InDuration)
{
	Duration=InDuration;
	Getter=IntGetter;
	Setter=InSetter;
	EndValue=InEndKey;
}

void UIVTweenVector2D::OnStartGetValue()
{
	if(Getter.IsBound())
	{
		StartValue=Getter.Execute();
	}
	OriginStartKey=StartValue;
}

void UIVTweenVector2D::TweenAndApplyValue(float CurrentTime)
{
	float Alpha=OnEaseFunction.Execute(CurrentTime,StartKey,ChangeKey,Duration);
	auto Value = FMath::Lerp(StartValue, EndValue, Alpha);
	Setter.ExecuteIfBound(Value);
}

void UIVTweenVector2D::SetValueForIncremental()
{
	FVector2D DiffValue = EndValue - StartValue;
	StartValue = EndValue;
	EndValue += DiffValue;
}

void UIVTweenVector2D::SetOriginValueForReverse()
{
	FVector2D DiffValue = EndValue - StartValue;
	StartValue = OriginStartKey;
	EndValue = DiffValue+OriginStartKey;
}
