// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IVEaseBPLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IVTweenBPLibrary.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FIVTweenFloatSetterDynamic, float, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FIVTweenIntSetterDynamic, int32, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FIVTweenVectorSetterDynamic,const FVector&, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FIVTweenVector2DSetterDynamic,const FVector2D&, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FIVTweenRotatorSetterDynamic,const FRotator&, Value);
DECLARE_DYNAMIC_DELEGATE_OneParam(FIVTweenTransformSetterDynamic,const FTransform&, Value);
/**
 * 
 */
UCLASS()
class IVCOMMON_API UIVTweenBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**基于float的Tween*/
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween", meta = (AdvancedDisplay = "Delay,EasingCategory,EasingType", WorldContext = "WorldContextObject"))
	static UIVTweenBase* FloatTo(UObject* WorldContextObject, FIVTweenFloatSetterDynamic Setter, float Start = 0.0f, float End = 1.0f, float Duration = 0.5f, float Delay = 0.0f, EEasingCategory EasingCategory=EEasingCategory::Ease_Cubic,EEasingType EasingType=EEasingType::EaseOut);

	/**基于Int的Tween，注意：返回值不是连续的int值*/
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween", meta = (AdvancedDisplay = "Delay,EasingCategory,EasingType", WorldContext = "WorldContextObject"))
	static UIVTweenBase* IntTo(UObject* WorldContextObject, FIVTweenIntSetterDynamic Setter, int32 Start, int32 End, float Duration = 0.5f, float Delay = 0.0f, EEasingCategory EasingCategory=EEasingCategory::Ease_Cubic,EEasingType EasingType=EEasingType::EaseOut);

	/**基于Vetcor的Tween*/
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween", meta = (AdvancedDisplay = "Delay,EasingCategory,EasingType", WorldContext = "WorldContextObject"))
	static UIVTweenBase* VetcorTo(UObject* WorldContextObject, FIVTweenVectorSetterDynamic Setter, FVector Start ,FVector End, float Duration = 0.5f, float Delay = 0.0f, EEasingCategory EasingCategory=EEasingCategory::Ease_Cubic,EEasingType EasingType=EEasingType::EaseOut);

	/**基于Vetcor2D的Tween*/
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween", meta = (AdvancedDisplay = "Delay,EasingCategory,EasingType", WorldContext = "WorldContextObject"))
	static UIVTweenBase* Vetcor2DTo(UObject* WorldContextObject, FIVTweenVector2DSetterDynamic Setter, FVector2D Start ,FVector2D End, float Duration = 0.5f, float Delay = 0.0f, EEasingCategory EasingCategory=EEasingCategory::Ease_Cubic,EEasingType EasingType=EEasingType::EaseOut);

	/**基于Rotator的Tween*/
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween", meta = (AdvancedDisplay = "Delay,EasingCategory,EasingType", WorldContext = "WorldContextObject"))
	static UIVTweenBase* RotatorTo(UObject* WorldContextObject, FIVTweenRotatorSetterDynamic Setter, FRotator Start ,FRotator End, float Duration = 0.5f, float Delay = 0.0f, EEasingCategory EasingCategory=EEasingCategory::Ease_Cubic,EEasingType EasingType=EEasingType::EaseOut);

	/**基于Rotator的Tween*/
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween", meta = (AdvancedDisplay = "Delay,EasingCategory,EasingType", WorldContext = "WorldContextObject"))
	static UIVTweenBase* TransformTo(UObject* WorldContextObject, FIVTweenTransformSetterDynamic Setter, FTransform Start ,FTransform End, float Duration = 0.5f, float Delay = 0.0f, EEasingCategory EasingCategory=EEasingCategory::Ease_Cubic,EEasingType EasingType=EEasingType::EaseOut);

	
	/**基于MaterialScalarParameter的Tween*/
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween", meta = (AdvancedDisplay = "Delay,EasingCategory,EasingType", WorldContext = "WorldContextObject"))
	static UIVTweenBase* MaterialScalarParameterTo(UObject* WorldContextObject,class UMaterialInstanceDynamic* Target, FName ParameterName,  float End = 1.0f, float Duration = 0.5f, float Delay = 0.0f, EEasingCategory EasingCategory=EEasingCategory::Ease_Cubic,EEasingType EasingType=EEasingType::EaseOut);

	/**基于MeshMaterialScalarParameter的Tween*/
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween", meta = (AdvancedDisplay = "Delay,EasingCategory,EasingType", WorldContext = "WorldContextObject"))
	static UIVTweenBase* MeshMaterialScalarParameterTo(class UPrimitiveComponent* Target, int32 MaterialIndex, FName ParameterName, float End, float Duration = 0.5f, float Delay = 0.0f,EEasingCategory EasingCategory=EEasingCategory::Ease_Cubic,EEasingType EasingType=EEasingType::EaseOut); 
};
