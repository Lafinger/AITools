// Copyright 2023 AIRT All Rights Reserved.


#include "Tween/Classes/IVTweenRotator.h"


UIVTweenRotator::UIVTweenRotator(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,StartKey(0.0f)
	,ChangeKey(1.0f)
	,OriginStartKey(0.0f)
{
	
}

 auto UIVTweenRotator::SetInitialKey(const FIVTweenRotatorGetterdDelegate IntGetter,
	const FIVTweenRotatorSetterdDelegate InSetter, const FRotator InEndKey, float InDuration) -> void
{
	Duration=InDuration;
	Getter=IntGetter;
	Setter=InSetter;
	EndValue=InEndKey;
}

void UIVTweenRotator::OnStartGetValue()
{
	if(Getter.IsBound())
	{
		StartValue=Getter.Execute();
	}
	OriginStartKey=StartValue;
}

void UIVTweenRotator::TweenAndApplyValue(float CurrentTime)
{
	float Alpha=OnEaseFunction.Execute(CurrentTime,StartKey,ChangeKey,Duration);
	FRotator Value = FMath::Lerp(StartValue, EndValue, Alpha);
	Setter.ExecuteIfBound(Value);
}

void UIVTweenRotator::SetValueForIncremental()
{
	FRotator DiffValue = EndValue - StartValue;
	StartValue = EndValue;
	EndValue += DiffValue;
}

void UIVTweenRotator::SetOriginValueForReverse()
{
	FRotator DiffValue = EndValue - StartValue;
	StartValue = OriginStartKey;
	EndValue = DiffValue+OriginStartKey;
}
