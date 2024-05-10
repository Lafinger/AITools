// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tween/IVTweenBase.h"
#include "IVTweenMaterialScalar.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_RetVal_OneParam(bool, FIVTweenMaterialScalarGetterDelegate, float&);
DECLARE_DELEGATE_RetVal_TwoParams(bool, FIVTweenMaterialScalarSetterDelegate, int32, float);

UCLASS(NotBlueprintType)
class IVCOMMON_API UIVTweenMaterialScalar : public UIVTweenBase
{
	GENERATED_UCLASS_BODY()

public:
	float StartKey;
	float EndKey;
	float ChangeKey;
	float OriginStartKey;
	int32 ParameterIndex;
	
	FIVTweenMaterialScalarGetterDelegate Getter;
	FIVTweenMaterialScalarSetterDelegate Setter;
public:
	void SetInitialValue(const FIVTweenMaterialScalarGetterDelegate& InGetter, const FIVTweenMaterialScalarSetterDelegate& InSetter, float InEndKey, float InDuration, int32 InParameterIndex);

protected:
	/* Begin IVTweenBase Interface. */
	virtual void OnStartGetValue() override;
	virtual void TweenAndApplyValue(float CurrentTime) override;
	virtual void SetValueForIncremental() override;
	virtual void SetOriginValueForReverse() override;
	/* End IVTweenBase Interface. */
};
