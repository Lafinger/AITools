// Copyright 2023 AIRT,  All Rights Reserved.


#include "Tween/Classes/IVTweenMaterialScalar.h"

#include "IVCommon.h"

UIVTweenMaterialScalar::UIVTweenMaterialScalar(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,StartKey(0.0f)
	,EndKey(0.0f)
	,ChangeKey(0.0f)
	,OriginStartKey(0.0f)
	,ParameterIndex(0)
{
	
}

void UIVTweenMaterialScalar::SetInitialValue(const FIVTweenMaterialScalarGetterDelegate& InGetter,
                                             const FIVTweenMaterialScalarSetterDelegate& InSetter, float InEndKey, float InDuration, int32 InParameterIndex)
{
	Duration=InDuration;
	Getter=InGetter;
	Setter=InSetter;
	EndKey=InEndKey;
	ParameterIndex=InParameterIndex;
}

void UIVTweenMaterialScalar::OnStartGetValue()
{
	if(Getter.IsBound())
	{
		if(Getter.Execute(StartKey))
		{
			ChangeKey=EndKey-StartKey;
			OriginStartKey=ChangeKey;
		}
		else
		{
			UE_LOG(LogIVTween,Warning,TEXT("[UIVTweenMaterialScalar/OnStartGetValue]Get paramter value error!"));
		}
	}
}

void UIVTweenMaterialScalar::TweenAndApplyValue(float CurrentTime)
{
	float Value=OnEaseFunction.Execute(CurrentTime,StartKey,ChangeKey,Duration);
	if(Setter.Execute(ParameterIndex,Value)==false)
	{
		UE_LOG(LogIVTween,Warning,TEXT("[UIVTweenMaterialScalar/TweenAndApplyValue]Set paramter value error!"));
	}
}

void UIVTweenMaterialScalar::SetValueForIncremental()
{
	StartKey=EndKey;
	EndKey+=ChangeKey;
}

void UIVTweenMaterialScalar::SetOriginValueForReverse()
{
	StartKey=OriginStartKey;
	EndKey=OriginStartKey+ChangeKey;
}
