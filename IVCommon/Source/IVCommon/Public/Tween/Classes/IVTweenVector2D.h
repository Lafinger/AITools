// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tween/IVTweenBase.h"
#include "IVTweenVector2D.generated.h"

DECLARE_DELEGATE_RetVal(FVector2D, FIVTweenVector2DGetterdDelegate);
DECLARE_DELEGATE_OneParam(FIVTweenVector2DSetterdDelegate, FVector2D);
/**
 * 
 */
UCLASS(NotBlueprintType)
class IVCOMMON_API UIVTweenVector2D : public UIVTweenBase
{
	GENERATED_UCLASS_BODY()
public:
	float StartKey;
	float ChangeKey;
	FVector2D StartValue;
	FVector2D EndValue;
	
	FVector2D OriginStartKey;

	FIVTweenVector2DGetterdDelegate Getter;
	FIVTweenVector2DSetterdDelegate Setter;

public:
	auto SetInitialKey(const FIVTweenVector2DGetterdDelegate IntGetter, const FIVTweenVector2DSetterdDelegate InSetter,
					   const FVector2D InEndKey, float InDuration) -> void;

protected:
	/* Begin IVTweenBase Interface. */
	virtual void OnStartGetValue() override;
	virtual void TweenAndApplyValue(float CurrentTime) override;
	virtual void SetValueForIncremental() override;
	virtual void SetOriginValueForReverse() override;
	/* End IVTweenBase Interface. */
	
};
