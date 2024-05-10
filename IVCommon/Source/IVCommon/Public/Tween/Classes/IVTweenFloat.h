// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tween/IVTweenBase.h"
#include "IVTweenFloat.generated.h"


DECLARE_DELEGATE_RetVal(float, FIVTweenFloatGetterdDelegate);
DECLARE_DELEGATE_OneParam(FIVTweenFloatSetterdDelegate, float);

/**
 * 
 */
UCLASS(NotBlueprintType)
class IVCOMMON_API UIVTweenFloat : public UIVTweenBase
{
	GENERATED_UCLASS_BODY()

public:
	float StartKey;
	float ChangeKey;
	float EndKey;

	float OriginStartKey;

	FIVTweenFloatGetterdDelegate Getter;
	FIVTweenFloatSetterdDelegate Setter;

public:
	void SetInitialKey(const FIVTweenFloatGetterdDelegate IntGetter,const FIVTweenFloatSetterdDelegate InSetter,float InEndKey,float InDuration);

protected:
	/* Begin IVTweenBase Interface. */
	virtual void OnStartGetValue() override;
	virtual void TweenAndApplyValue(float CurrentTime) override;
	virtual void SetValueForIncremental() override;
	virtual void SetOriginValueForReverse() override;
	/* End IVTweenBase Interface. */
};


