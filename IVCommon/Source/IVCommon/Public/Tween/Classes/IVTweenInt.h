// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tween/IVTweenBase.h"
#include "IVTweenInt.generated.h"
DECLARE_DELEGATE_RetVal(float, FIVTweenIntGetterdDelegate);
DECLARE_DELEGATE_OneParam(FIVTweenIntSetterdDelegate, float);
/**
 * 
 */
UCLASS(NotBlueprintType)
class IVCOMMON_API UIVTweenInt : public UIVTweenBase
{
	GENERATED_UCLASS_BODY()

public:
	float StartKey;
	int32 StartValue;
	int32 EndValue;
	float ChangeKey;

	int32 OriginStartKey;

	FIVTweenIntGetterdDelegate Getter;
	FIVTweenIntSetterdDelegate Setter;

public:
	void SetInitialKey(const FIVTweenIntGetterdDelegate IntGetter,const FIVTweenIntSetterdDelegate InSetter,int32 InEndKey,float InDuration);

protected:
	/* Begin IVTweenBase Interface. */
	virtual void OnStartGetValue() override;
	virtual void TweenAndApplyValue(float CurrentTime) override;
	virtual void SetValueForIncremental() override;
	virtual void SetOriginValueForReverse() override;
	/* End IVTweenBase Interface. */
};
