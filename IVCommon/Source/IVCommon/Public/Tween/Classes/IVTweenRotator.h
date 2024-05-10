// Copyright 2023 AIRT All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tween/IVTweenBase.h"
#include "IVTweenRotator.generated.h"

DECLARE_DELEGATE_RetVal(FRotator, FIVTweenRotatorGetterdDelegate);
DECLARE_DELEGATE_OneParam(FIVTweenRotatorSetterdDelegate, FRotator);

UCLASS(NotBlueprintType)
class IVCOMMON_API UIVTweenRotator : public UIVTweenBase
{
	GENERATED_UCLASS_BODY()
	
public:
	float StartKey;
	float ChangeKey;
	FRotator StartValue;
	FRotator EndValue;

	FRotator OriginStartKey;

	FIVTweenRotatorGetterdDelegate Getter;
	FIVTweenRotatorSetterdDelegate Setter;

public:
	auto SetInitialKey(const FIVTweenRotatorGetterdDelegate IntGetter, const FIVTweenRotatorSetterdDelegate InSetter,
					   const FRotator InEndKey, float InDuration) -> void;

protected:
	/* Begin IVTweenBase Interface. */
	virtual void OnStartGetValue() override;
	virtual void TweenAndApplyValue(float CurrentTime) override;
	virtual void SetValueForIncremental() override;
	virtual void SetOriginValueForReverse() override;
	/* End IVTweenBase Interface. */
};
