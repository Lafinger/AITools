// Copyright 2023 AIRT,  All Rights Reserved.


#include "Tween/IVTweenBase.h"

UIVTweenBase::UIVTweenBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,Duration(0.0f)
	,Delay(0.0f)
	,ElapseTime(0.0f)
	,NumLoopsToPlay(0)
	,CurrentLoops(0)
	,bMarkedToKill(false)
	,bMarkedPause(false)
	,bStart(false)
	,bReverse(false)
	,PlayMode(ETweenPlayMode::Type::Once)
{

	OnEaseFunction.BindStatic(&UIVEaseBPLibrary::CubicOut);
}

bool UIVTweenBase::IsActive()
{
	return bActive;
}

void UIVTweenBase::SetEase(EEasingCategory EasingCategory, EEasingType EasingType)
{
	switch (EasingCategory)
	{
	case EEasingCategory::Ease_Linear:
		OnEaseFunction.BindStatic(&UIVEaseBPLibrary::Linear);
		break;
	case EEasingCategory::Ease_Sine:
		switch (EasingType)
		{
			case EEasingType::EaseIn:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::SineIn);
				break;
			
			case EEasingType::EaseOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::SineOut);
				break;
			case EEasingType::EaseInOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::SineInOut);
				break;
		}
		break;
		
	case EEasingCategory::Ease_Cubic:
		switch (EasingType)
		{
			case EEasingType::EaseIn:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::CubicIn);
				break;
			
			case EEasingType::EaseOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::CubicOut);
				break;
			
			case EEasingType::EaseInOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::CubicInOut);
				break;
		}		
		break;
		
	case EEasingCategory::Ease_Quint:
		switch (EasingType)
		{
			case EEasingType::EaseIn:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::QuintIn);
				break;
			
			case EEasingType::EaseOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::QuintOut);
				break;
			
			case EEasingType::EaseInOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::QuintInOut);
				break;
		}		
		break;
	
	case EEasingCategory::Ease_Circle:
		switch (EasingType)
		{
			case EEasingType::EaseIn:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::CircleIn);
				break;
			
			case EEasingType::EaseOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::CircleOut);
				break;
			
			case EEasingType::EaseInOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::CircleInOut);
				break;
		}		
		break;
	
	case EEasingCategory::Ease_Elastic:
		switch (EasingType)
		{
			case EEasingType::EaseIn:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::ElasticIn);
				break;
			
			case EEasingType::EaseOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::ElasticOut);
				break;
			
			case EEasingType::EaseInOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::ElasticInOut);
				break;
		}		
		break;

	case EEasingCategory::Ease_Quad:
		switch (EasingType)
		{
			case EEasingType::EaseIn:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::QuintIn);
				break;
			
			case EEasingType::EaseOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::QuintOut);
				break;
			
			case EEasingType::EaseInOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::QuintInOut);
				break;
		}		
		break;
	
	case EEasingCategory::Ease_Quart:
		switch (EasingType)
		{
			case EEasingType::EaseIn:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::QuadIn);
				break;
			
			case EEasingType::EaseOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::QuadOut);
				break;
			
			case EEasingType::EaseInOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::QuadInOut);
				break;
		}		
		break;

	case EEasingCategory::Ease_Expo:
		switch (EasingType)
		{
			case EEasingType::EaseIn:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::ExpoIn);
				break;
			
			case EEasingType::EaseOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::ExpoOut);
				break;
			
			case EEasingType::EaseInOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::ExpoInOut);
				break;
		}		
		break;

	case EEasingCategory::Ease_Back:
		switch (EasingType)
		{
			case EEasingType::EaseIn:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::BackIn);
				break;
			
			case EEasingType::EaseOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::BackOut);
				break;
			
			case EEasingType::EaseInOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::BackInOut);
				break;
		}		
		break;

	case EEasingCategory::Ease_Bounce:
		switch (EasingType)
		{
			case EEasingType::EaseIn:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::BounceIn);
				break;
			
			case EEasingType::EaseOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::BounceOut);
				break;
			
			case EEasingType::EaseInOut:
				OnEaseFunction.BindStatic(&UIVEaseBPLibrary::BounceInOut);
			break;
		}		
		break;

	default:
		break;
	}
}

void UIVTweenBase::SetDelay(float InDelay)
{
	if (ElapseTime > 0 || bStart)
	{
		return;
	} 
	Delay = InDelay;
	
	if (Delay < 0)
	{
		Delay = 0;
	}

}

void UIVTweenBase::SetLoop(ETweenPlayMode::Type InPlayMode, int32 NomberOfLoops/** = 1*/)
{
	if(ElapseTime >0 || bStart)
	{
		return;
	}

	PlayMode=InPlayMode;
	NumLoopsToPlay=NomberOfLoops;
	
}

void UIVTweenBase::Kill()
{
	OnKillEvent.ExecuteIfBound();
	bActive=false;
	bMarkedToKill = true;
}

void UIVTweenBase::ForceComplete()
{
	bActive=false;
	bMarkedToKill=true;
	ElapseTime=Delay+Duration;
	TweenAndApplyValue(Duration);
	OnUpdateEvent.ExecuteIfBound(1.0f);
	OnCompleteEvent.ExecuteIfBound();
}

void UIVTweenBase::Pause()
{
	bMarkedPause=true;
	bActive=false;
}

void UIVTweenBase::Resume()
{
	bMarkedPause=false;
	bActive=true;
}

void UIVTweenBase::Restart()
{
	if(ElapseTime == 0)
	{
		return;
	}
	bMarkedPause =false;
	bActive=true;
	CurrentLoops=0;
	bReverse=false;
	SetOriginValueForReverse();
	ToNextWithElapsedTime(0.0f);
}

void UIVTweenBase::Goto(float TimePoint)
{
	TimePoint=FMath::Clamp(TimePoint,0.0f,Duration);
	CurrentLoops=0;
	bReverse=false;
	ToNextWithElapsedTime(TimePoint);
}

int32 UIVTweenBase::GetLoopCurrentCount()
{
	return CurrentLoops;
}

void UIVTweenBase::OnComplete(const FSimpleDelegate& InSignature)
{
	OnCompleteEvent=InSignature;
}

void UIVTweenBase::OnComplete(const TFunction<void()>& InFunction)
{
	if(InFunction)
	{
		OnCompleteEvent.BindLambda(InFunction);
	}
}

void UIVTweenBase::OnComplete(const FIVTweenSimpleDynamicSignature& InDynamicSignature)
{
	OnCompleteEvent.BindLambda([InDynamicSignature]
	{
		InDynamicSignature.ExecuteIfBound();
	});
}

void UIVTweenBase::OnCycleComplete(const FSimpleDelegate& InSignature)
{
	OnCycleCompleteEvent=InSignature;
}

void UIVTweenBase::OnCycleComplete(const TFunction<void()>& InFunction)
{
	if(InFunction)
	{
		OnCycleCompleteEvent.BindLambda(InFunction);
	}
}

void UIVTweenBase::OnCycleComplete(const FIVTweenSimpleDynamicSignature& InDynamicSignature)
{
	OnCycleCompleteEvent.BindLambda([InDynamicSignature]
	{
		InDynamicSignature.ExecuteIfBound();
	});
}

void UIVTweenBase::OnCycleStart(const FSimpleDelegate& InSignature)
{
	OnCycleStartEvent=InSignature;
}

void UIVTweenBase::OnCycleStart(const TFunction<void()>& InFunction)
{
	if(InFunction)
	{
		OnCycleStartEvent.BindLambda(InFunction);
	}
}

void UIVTweenBase::OnCycleStart(const FIVTweenSimpleDynamicSignature& InDynamicSignature)
{
	OnCycleStartEvent.BindLambda([InDynamicSignature]
	{
		InDynamicSignature.ExecuteIfBound();
	});
}

void UIVTweenBase::OnUpdate(const FIVTweenUpdatSignature& InSignature)
{
	OnUpdateEvent=InSignature;
}

void UIVTweenBase::OnUpdate(const TFunction<void(float)>& InFunction)
{
	if(InFunction)
	{
		OnUpdateEvent.BindLambda(InFunction);
	}
}

void UIVTweenBase::OnUpdate(const FIVTweenProgressDynamicSignature& InDynamicSignature)
{
	OnUpdateEvent.BindLambda([InDynamicSignature](float Progress)
	{
		InDynamicSignature.ExecuteIfBound(Progress);
	});
}

void UIVTweenBase::OnStart(const FSimpleDelegate& InSignature)
{
	OnStartEvent=InSignature;
}

void UIVTweenBase::OnStart(const TFunction<void()>& InFunction)
{
	if(InFunction)
	{
		OnStartEvent.BindLambda(InFunction);
	}
}

bool UIVTweenBase::ToNext(float DeltaTime)
{
	if(bMarkedToKill)
	{
		return false;
	}
	if(bMarkedPause)
	{
		return true;
	}
	return  ToNextWithElapsedTime(ElapseTime+DeltaTime);
}

bool UIVTweenBase::ToNextWithElapsedTime(float InElapseTime)
{
	ElapseTime=InElapseTime;

	//如果总的时间大于延迟，执行动画
	if(ElapseTime>Delay)
	{
		bActive=true;
		
		if(!bStart)
		{
			bStart=true;
			//设置重置value
			OnStartGetValue();
			OnCycleStartEvent.ExecuteIfBound();
			OnStartEvent.ExecuteIfBound();
		}

		float ElapseTimeWithoutDelay=ElapseTime-Delay;
		float CurrentTime=ElapseTimeWithoutDelay-Duration*CurrentLoops;
		if(CurrentTime>=Duration)
		{
			bool ReturnValue=true;
			CurrentLoops++;
			
			TweenAndApplyValue(bReverse ? 0:Duration);
			OnUpdateEvent.ExecuteIfBound(1.0f);

			if(PlayMode == ETweenPlayMode::Type::Once)
			{
				OnCompleteEvent.ExecuteIfBound();
				ReturnValue =false;
			}
			//无限循环
			else if(NumLoopsToPlay<=-1)
			{
				OnCycleStartEvent.ExecuteIfBound();
				ReturnValue =true;
			}
			else
			{
				if(CurrentLoops>=NumLoopsToPlay)
				{
					OnCompleteEvent.ExecuteIfBound();
					bActive=false;
					ReturnValue=false;
				}
				else
				{
					OnCycleStartEvent.ExecuteIfBound();
					ReturnValue=true;
				}
			}

			switch (PlayMode)
			{
			case ETweenPlayMode::Type::Reverse:
				SetValueForReverse();
				break;
			case ETweenPlayMode::Type::PingPong:
				bReverse= !bReverse;
				SetValueForPingPong();
				break;
			case ETweenPlayMode::Type::Incremental:
				SetValueForIncremental();
				break;
			}
			return ReturnValue;
		}
		else
		{
			if(bReverse)
			{
				CurrentTime=Duration-CurrentTime;
			}
			TweenAndApplyValue(CurrentTime);
			OnUpdateEvent.ExecuteIfBound(CurrentTime/Duration);
			return true;
		}
	}
	else
	{
		//等待
		bActive=false;
		return true;
	}

	
}

void UIVTweenBase::OnStart(const FIVTweenSimpleDynamicSignature& InDynamicSignature)
{
	OnStartEvent.BindLambda([InDynamicSignature]
	{
		InDynamicSignature.ExecuteIfBound();
	});
}
