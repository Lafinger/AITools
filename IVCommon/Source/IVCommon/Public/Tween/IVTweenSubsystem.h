// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/IVTweenFloat.h"
#include "Classes/IVTweenInt.h"
#include "Classes/IVTweenMaterialScalar.h"
#include "Classes/IVTweenRotator.h"
#include "Classes/IVTweenTransform.h"
#include "Classes/IVTweenVector.h"
#include "Classes/IVTweenVector2D.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "IVTweenSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FIVTweenUpdateMulticastSignature, float);

DECLARE_STATS_GROUP(TEXT("IVTween"), STATGROUP_IVTween, STATCAT_Advanced);
/**
 * 
 */
UCLASS()
class IVCOMMON_API UIVTweenSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
	//~USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~End of USubsystem interface

	//~FTickableObjectBase interface
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	//~End of FTickableObjectBase interface

public:
	
	UFUNCTION(BlueprintPure, Category = LTween, meta = (WorldContext = "WorldContextObject"))
	static UIVTweenSubsystem* GetTweenSubsystee(UObject* WorldContextObject);
	
	/** 使用“CustomTick”而不是 UE4 的默认 Tick 来控制补间动画。 调用“DisableTick”函数禁用UE4默认的Tick函数，再调用这个CustomTick函数*/
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween")
	void CustomTick(float DeltaTime);
	
	/**
	* 禁用默认的 Tick 功能，这样你就可以暂停所有补间或使用 CustomTick 来做你自己的 tick 并使用你自己的 DeltaTime。
	* 这只会暂停当前 UIVTweenSubsystem 实例的 tick，因此在加载新关卡后，默认 Tick 将再次工作，如果要禁用 tick，则需要再次调用 DisableTick。
	*/
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween")
	void DisableTick();
	
	/** 启用默认勾选（如果禁用）*/
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween")
	void EnableTick();

	/** 杀死 All Tweens */
	UFUNCTION(BlueprintCallable, Category = "Visual|Tween")
	void KillAllTweens();

public:
	UIVTweenBase* To(const FIVTweenIntGetterdDelegate& Getter, const FIVTweenIntSetterdDelegate& Setter, int32 EndValue, float Duration);
	UIVTweenBase* To(const FIVTweenFloatGetterdDelegate& Getter, const FIVTweenFloatSetterdDelegate& Setter, float EndValue, float Duration);
	UIVTweenBase* To(const FIVTweenVectorGetterdDelegate& Getter, const FIVTweenVectorSetterdDelegate& Setter, FVector EndValue, float Duration);
	UIVTweenBase* To(const FIVTweenVector2DGetterdDelegate& Getter, const FIVTweenVector2DSetterdDelegate& Setter, FVector2D EndValue, float Duration);
	UIVTweenBase* To(const FIVTweenRotatorGetterdDelegate& Getter, const FIVTweenRotatorSetterdDelegate& Setter, FRotator EndValue, float Duration);
	UIVTweenBase* To(const FIVTweenTransformGetterdDelegate& Getter, const FIVTweenTransformSetterdDelegate& Setter, FTransform EndValue, float Duration);
	UIVTweenBase* To(const FIVTweenMaterialScalarGetterDelegate& Getter, const FIVTweenMaterialScalarSetterDelegate& Setter,float EndValue, float Duration, int32 ParameterIndex);
	
private:
	UPROPERTY(VisibleAnywhere, Category="Visual|Tween")
	TArray<UIVTweenBase*> TweenerList;

private:
	FIVTweenUpdateMulticastSignature OnMulticastUpdateEvent;
	
	bool TickPaused;
	
private:
	void OnTick(float DeltaTime);
	
};
