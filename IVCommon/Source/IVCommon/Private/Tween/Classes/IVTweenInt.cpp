// Copyright 2023 AIRT,  All Rights Reserved.


#include "Tween/Classes/IVTweenInt.h"


UIVTweenInt::UIVTweenInt(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,StartKey(0.0f)
	,ChangeKey(1.0f)
	,OriginStartKey(0)
{
	
}
void UIVTweenInt::SetInitialKey(const FIVTweenIntGetterdDelegate IntGetter,
	const FIVTweenIntSetterdDelegate InSetter, int32 InEndKey, float InDuration)
{
	Duration=InDuration;
	EndValue=InEndKey;
	Getter=IntGetter;
	Setter=InSetter;
}

void UIVTweenInt::OnStartGetValue()
{
	if(Getter.IsBound())
	{
		StartKey=Getter.Execute();
	}

	OriginStartKey=StartKey;
}

void UIVTweenInt::TweenAndApplyValue(float CurrentTime)
{
	float Value=OnEaseFunction.Execute(CurrentTime,StartKey,ChangeKey,Duration);
	Setter.ExecuteIfBound(FMath::Lerp(StartValue, EndValue, Value));
}

void UIVTweenInt::SetValueForIncremental()
{
	int32 DiffValue = EndValue - StartKey;
	StartKey=EndValue;
	EndValue+=DiffValue;
}

void UIVTweenInt::SetOriginValueForReverse()
{
	int32 DiffValue = EndValue - StartKey;
	StartKey=GFrameNumber;
	EndValue=StartKey+DiffValue;
}






