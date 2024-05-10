// Copyright 2023 AIRT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tween/IVTweenBase.h"
#include "IVTweenTransform.generated.h"

DECLARE_DELEGATE_RetVal(FTransform, FIVTweenTransformGetterdDelegate);
DECLARE_DELEGATE_OneParam(FIVTweenTransformSetterdDelegate, FTransform);
/**
 * 
 */
UCLASS(NotBlueprintType)
class IVCOMMON_API UIVTweenTransform : public UIVTweenBase
{
	GENERATED_UCLASS_BODY()
	public:
	float StartKey;
	float ChangeKey;
	FTransform StartValue;
	FTransform EndValue;
	
	FTransform OriginStartKey;

	FIVTweenTransformGetterdDelegate Getter;
	FIVTweenTransformSetterdDelegate Setter;

public:
	auto SetInitialKey(const FIVTweenTransformGetterdDelegate IntGetter, const FIVTweenTransformSetterdDelegate InSetter,
					   const FTransform InEndKey, float InDuration) -> void;

protected:
	/* Begin IVTweenBase Interface. */
	virtual void OnStartGetValue() override;
	virtual void TweenAndApplyValue(float CurrentTime) override;
	virtual void SetValueForIncremental() override;
	virtual void SetOriginValueForReverse() override;
	/* End IVTweenBase Interface. */
	
};
