// Copyright 2023 AIRT All Rights Reserved.


#include "Tween/Classes/IVTweenTransform.h"

UIVTweenTransform::UIVTweenTransform(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,StartKey(0.0f)
	,ChangeKey(1.0f)
	,OriginStartKey(FTransform())
{
	
}

auto UIVTweenTransform::SetInitialKey(const FIVTweenTransformGetterdDelegate IntGetter,
	const FIVTweenTransformSetterdDelegate InSetter, const FTransform InEndKey, float InDuration) -> void
{
	Duration=InDuration;
	Getter=IntGetter;
	Setter=InSetter;
	EndValue=InEndKey;
}

void UIVTweenTransform::OnStartGetValue()
{
	if(Getter.IsBound())
	{
		StartValue=Getter.Execute();
	}
	OriginStartKey=StartValue;
}

void UIVTweenTransform::TweenAndApplyValue(float CurrentTime)
{
	float Alpha=OnEaseFunction.Execute(CurrentTime,StartKey,ChangeKey,Duration);
	
	FRotator Rotator= FMath::Lerp(StartValue.Rotator(), EndValue.Rotator(), Alpha);
	FVector Vector= FMath::Lerp(StartValue.GetLocation(), EndValue.GetLocation(), Alpha);
	FVector Scal= FMath::Lerp(StartValue.GetScale3D(), EndValue.GetScale3D(), Alpha);
	FTransform Value =FTransform(Rotator,Vector,Scal);
	Setter.ExecuteIfBound(Value);
}

void UIVTweenTransform::SetValueForIncremental()
{
	FTransform DiffValue = FTransform(EndValue.Rotator().Quaternion()-StartValue.Rotator().Quaternion(),EndValue.GetLocation()-StartValue.GetLocation(),EndValue.GetScale3D()-StartValue.GetScale3D());
	StartValue = EndValue;
	EndValue += DiffValue;
}

void UIVTweenTransform::SetOriginValueForReverse()
{
	FTransform DiffValue =FTransform(EndValue.Rotator().Quaternion()-StartValue.Rotator().Quaternion(),EndValue.GetLocation()-StartValue.GetLocation(),EndValue.GetScale3D()-StartValue.GetScale3D());
	StartValue = OriginStartKey;
	EndValue =	FTransform(DiffValue.Rotator().Quaternion()+OriginStartKey.Rotator().Quaternion(),DiffValue.GetLocation()+OriginStartKey.GetLocation(),DiffValue.GetScale3D()+OriginStartKey.GetScale3D());;

}
