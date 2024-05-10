// Copyright 2023 AIRT,  All Rights Reserved.


#include "Tween/Classes/IVTweenFloat.h"


UIVTweenFloat::UIVTweenFloat(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,ChangeKey(0.0f)
	,OriginStartKey(0.0f)
{
	
}
void UIVTweenFloat::SetInitialKey(const FIVTweenFloatGetterdDelegate IntGetter,
	const FIVTweenFloatSetterdDelegate InSetter, float InEndKey, float InDuration)
{
	Duration=InDuration;
	EndKey=InEndKey;
	Getter=IntGetter;
	Setter=InSetter;
}

void UIVTweenFloat::OnStartGetValue()
{
	if(Getter.IsBound())
	{
		StartKey=Getter.Execute();
	}

	OriginStartKey=StartKey;
	ChangeKey=EndKey-StartKey;
}

void UIVTweenFloat::TweenAndApplyValue(float CurrentTime)
{
	float Value=OnEaseFunction.Execute(CurrentTime,StartKey,ChangeKey,Duration);
	Setter.ExecuteIfBound(Value);
}

void UIVTweenFloat::SetValueForIncremental()
{
	StartKey=EndKey;
	EndKey+=ChangeKey;
}

void UIVTweenFloat::SetOriginValueForReverse()
{
	StartKey=OriginStartKey;
	EndKey=StartKey+ChangeKey;
}






