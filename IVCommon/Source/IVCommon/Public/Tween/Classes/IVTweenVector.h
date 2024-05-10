// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tween/IVTweenBase.h"
#include "IVTweenVector.generated.h"

DECLARE_DELEGATE_RetVal(FVector, FIVTweenVectorGetterdDelegate);
DECLARE_DELEGATE_OneParam(FIVTweenVectorSetterdDelegate, FVector);
/**
 * 
 */
UCLASS(NotBlueprintType)
class IVCOMMON_API UIVTweenVector : public UIVTweenBase
{
	GENERATED_UCLASS_BODY()

public:
	float StartKey;
	float ChangeKey;
	FVector StartValue;
	FVector EndValue;

	FVector OriginStartKey;

	FIVTweenVectorGetterdDelegate Getter;
	FIVTweenVectorSetterdDelegate Setter;

public:
	auto SetInitialKey(const FIVTweenVectorGetterdDelegate IntGetter, const FIVTweenVectorSetterdDelegate InSetter,
	                   const FVector InEndKey, float InDuration) -> void;

protected:
	/* Begin IVTweenBase Interface. */
	virtual void OnStartGetValue() override;
	virtual void TweenAndApplyValue(float CurrentTime) override;
	virtual void SetValueForIncremental() override;
	virtual void SetOriginValueForReverse() override;
	/* End IVTweenBase Interface. */
	
};
