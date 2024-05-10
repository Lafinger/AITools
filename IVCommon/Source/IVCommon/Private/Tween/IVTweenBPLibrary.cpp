// Copyright 2023 AIRT All Rights Reserved.


#include "Tween/IVTweenBPLibrary.h"

#include "IVCommon.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Tween/IVTweenSubsystem.h"


UIVTweenBase* UIVTweenBPLibrary::FloatTo(UObject* WorldContextObject, FIVTweenFloatSetterDynamic Setter, float Start,
                                         float End, float Duration, float Delay, EEasingCategory EasingCategory, EEasingType EasingType)
{
	 UIVTweenBase* Tween=UIVTweenSubsystem::GetTweenSubsystee(WorldContextObject)->To(FIVTweenFloatGetterdDelegate::CreateLambda([Start]
	{
		return Start;
	 	
	}),FIVTweenFloatSetterdDelegate::CreateLambda([Setter](float Value)
	{
		if (Setter.IsBound())
		{
			Setter.Execute(Value);
		}
	}),End,Duration);

	Tween->SetDelay(Delay);
	Tween->SetEase(EasingCategory,EasingType);

	return Tween;
}

UIVTweenBase* UIVTweenBPLibrary::IntTo(UObject* WorldContextObject, FIVTweenIntSetterDynamic Setter, int32 Start,
	int32 End, float Duration, float Delay, EEasingCategory EasingCategory, EEasingType EasingType)
{
	UIVTweenBase* Tween=UIVTweenSubsystem::GetTweenSubsystee(WorldContextObject)->To(FIVTweenIntGetterdDelegate::CreateLambda([Start]
	{
	 return Start;
	 	
	}),FIVTweenIntSetterdDelegate::CreateLambda([Setter](int32 Value)
	{
   if (Setter.IsBound())
   {
	   Setter.Execute(Value);
   }
	}),End,Duration);

	Tween->SetDelay(Delay);
	Tween->SetEase(EasingCategory,EasingType);

	return Tween;
}

UIVTweenBase* UIVTweenBPLibrary::VetcorTo(UObject* WorldContextObject, FIVTweenVectorSetterDynamic Setter, FVector Start ,
	FVector End ,float Duration, float Delay, EEasingCategory EasingCategory, EEasingType EasingType)
{
	UIVTweenBase* Tween=UIVTweenSubsystem::GetTweenSubsystee(WorldContextObject)->To(FIVTweenVectorGetterdDelegate::CreateLambda([Start]
	{
	 return Start;
	 	
	}),FIVTweenVectorSetterdDelegate::CreateLambda([Setter](FVector Value)
	{
   if (Setter.IsBound())
   {
	   Setter.Execute(Value);
   }
	}),End,Duration);

	Tween->SetDelay(Delay);
	Tween->SetEase(EasingCategory,EasingType);

	return Tween;
}

UIVTweenBase* UIVTweenBPLibrary::Vetcor2DTo(UObject* WorldContextObject, FIVTweenVector2DSetterDynamic Setter,
	FVector2D Start, FVector2D End, float Duration, float Delay, EEasingCategory EasingCategory, EEasingType EasingType)
{
	UIVTweenBase* Tween=UIVTweenSubsystem::GetTweenSubsystee(WorldContextObject)->To(FIVTweenVector2DGetterdDelegate::CreateLambda([Start]
	{
	 return Start;
	 	
	}),FIVTweenVector2DSetterdDelegate::CreateLambda([Setter](FVector2D Value)
	{
   if (Setter.IsBound())
   {
	   Setter.Execute(Value);
   }
	}),End,Duration);

	Tween->SetDelay(Delay);
	Tween->SetEase(EasingCategory,EasingType);

	return Tween;
}

UIVTweenBase* UIVTweenBPLibrary::RotatorTo(UObject* WorldContextObject, FIVTweenRotatorSetterDynamic Setter,
	FRotator Start, FRotator End, float Duration, float Delay, EEasingCategory EasingCategory, EEasingType EasingType)
{
	UIVTweenBase* Tween=UIVTweenSubsystem::GetTweenSubsystee(WorldContextObject)->To(FIVTweenRotatorGetterdDelegate::CreateLambda([Start]
	{
	 return Start;
	 	
	}),FIVTweenRotatorSetterdDelegate::CreateLambda([Setter](FRotator Value)
	{
   if (Setter.IsBound())
   {
	   Setter.Execute(Value);
   }
	}),End,Duration);

	Tween->SetDelay(Delay);
	Tween->SetEase(EasingCategory,EasingType);

	return Tween;
}

UIVTweenBase* UIVTweenBPLibrary::TransformTo(UObject* WorldContextObject, FIVTweenTransformSetterDynamic Setter,
	FTransform Start, FTransform End, float Duration, float Delay, EEasingCategory EasingCategory, EEasingType EasingType)
{
	UIVTweenBase* Tween=UIVTweenSubsystem::GetTweenSubsystee(WorldContextObject)->To(FIVTweenTransformGetterdDelegate::CreateLambda([Start]
	{
		return Start;
	 	
	}),FIVTweenTransformSetterdDelegate::CreateLambda([Setter](FTransform Value)
	{
	if (Setter.IsBound())
	{
		Setter.Execute(Value);
	}
	}),End,Duration);

	Tween->SetDelay(Delay);
	Tween->SetEase(EasingCategory,EasingType);

	return Tween;
}

UIVTweenBase* UIVTweenBPLibrary::MaterialScalarParameterTo(UObject* WorldContextObject,
                                                           UMaterialInstanceDynamic* Target, FName ParameterName, float End, float Duration, float Delay,
                                                           EEasingCategory EasingCategory, EEasingType EasingType)
{
	if (!IsValid(Target))
	{
		UE_LOG(LogIVTween, Error, TEXT("[UIVTweenBPLibrary::MaterialScalarParameterTo] target is not valid:%s"), *(Target->GetPathName()));
		return nullptr;
	}
	
	float StartValue = 0.0f;
	int32 ParameterIndex = 0;
	if (Target->GetScalarParameterValue(ParameterName, StartValue))
	{
		Target->InitializeScalarParameterAndGetIndex(ParameterName, StartValue, ParameterIndex);
	}
	else
	{
		UE_LOG(LogIVTween, Error, TEXT("[UIVTweenBPLibrary::MaterialScalarParameterTo]GetScalarParameterValue:%s error!"), *(ParameterName.ToString()));
		return nullptr;
	}
	UIVTweenBase* Tween=UIVTweenSubsystem::GetTweenSubsystee(WorldContextObject)->To(FIVTweenMaterialScalarGetterDelegate::CreateWeakLambda(Target,[Target,ParameterName](float&Result)
	{
		return Target->GetScalarParameterValue(ParameterName,Result);
	}),FIVTweenMaterialScalarSetterDelegate::CreateUObject(Target,&UMaterialInstanceDynamic::SetScalarParameterByIndex),End,Duration,ParameterIndex);

	Tween->SetDelay(Delay);
	Tween->SetEase(EasingCategory,EasingType);

	return Tween;
}

UIVTweenBase* UIVTweenBPLibrary::MeshMaterialScalarParameterTo(UPrimitiveComponent* Target, int32 MaterialIndex,
	FName ParameterName, float End, float Duration, float Delay, EEasingCategory EasingCategory,
	EEasingType EasingType)
{
	if (!IsValid(Target))
	{
		UE_LOG(LogIVTween, Error, TEXT("[UIVTweenBPLibrary::MaterialScalarParameterTo] target is not valid:%s"), *(Target->GetPathName()));
		return nullptr;
	}
	
	float StartValue = 0.0f;
	int32 ParameterIndex = 0;
	UMaterialInstanceDynamic* Material=Target->CreateAndSetMaterialInstanceDynamic(ParameterIndex);
	if (Material->GetScalarParameterValue(ParameterName, StartValue))
	{
		Material->InitializeScalarParameterAndGetIndex(ParameterName, StartValue, ParameterIndex);
	}
	else
	{
		UE_LOG(LogIVTween, Error, TEXT("[UIVTweenBPLibrary::MaterialScalarParameterTo]GetScalarParameterValue:%s error!"), *(ParameterName.ToString()));
		return nullptr;
	}

	UIVTweenBase* Tween=UIVTweenSubsystem::GetTweenSubsystee(Target)->To(FIVTweenMaterialScalarGetterDelegate::CreateWeakLambda(Material,[Material,ParameterName](float&Result)
	{
		return Material->GetScalarParameterValue(ParameterName,Result);
	}),FIVTweenMaterialScalarSetterDelegate::CreateUObject(Material,&UMaterialInstanceDynamic::SetScalarParameterByIndex),End,Duration,ParameterIndex);

	Tween->SetDelay(Delay);
	Tween->SetEase(EasingCategory,EasingType);

	return Tween;
	
}
