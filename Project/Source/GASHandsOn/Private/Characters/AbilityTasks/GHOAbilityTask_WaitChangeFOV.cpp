// (C) Sentya Anko 2021

#include "Characters/AbilityTasks/GHOAbilityTask_WaitChangeFOV.h"
#include "Camera/CameraComponent.h"

UGHOAbilityTask_WaitChangeFOV::UGHOAbilityTask_WaitChangeFOV()
{
	bTickingTask = true;
}

UGHOAbilityTask_WaitChangeFOV* UGHOAbilityTask_WaitChangeFOV::WaitChangeFOV(UGameplayAbility* OwningAbility, FName TaskInstanceName, class UCameraComponent* CameraComponent, float TargetFOV, float Duration, UCurveFloat* OptionalInterpolationCurve)
{
	UGHOAbilityTask_WaitChangeFOV* MyObj = NewAbilityTask<UGHOAbilityTask_WaitChangeFOV>(OwningAbility, TaskInstanceName);
	MyObj->Initialize(CameraComponent, TargetFOV, Duration, OptionalInterpolationCurve);
	return MyObj;
}

void UGHOAbilityTask_WaitChangeFOV::Initialize(class UCameraComponent* InCameraComponent, float InTargetFOV, float InDuration, UCurveFloat* InOptionalInterpolationCurve)
{
	CameraComponent = InCameraComponent;
	if (InCameraComponent != nullptr)
	{
		StartFOV = CameraComponent->FieldOfView;
	}

	TargetFOV = InTargetFOV;
	Duration = FMath::Max(InDuration, 0.001f);		// Avoid negative or divide-by-zero cases
	TimeChangeStarted = GetWorld()->GetTimeSeconds();
	TimeChangeWillEnd = TimeChangeStarted + Duration;
	LerpCurve = InOptionalInterpolationCurve;
}

void UGHOAbilityTask_WaitChangeFOV::Activate()
{
}

void UGHOAbilityTask_WaitChangeFOV::TickTask(float DeltaTime)
{
	if (bIsFinished)
	{
		return;
	}

	Super::TickTask(DeltaTime);

	if (CameraComponent)
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();

		if (CurrentTime >= TimeChangeWillEnd)
		{
			bIsFinished = true;

			CameraComponent->SetFieldOfView(TargetFOV);

			if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnTargetFOVReached.Broadcast();
			}
			EndTask();
		}
		else
		{
			const float Elapsed = (CurrentTime - TimeChangeStarted) / Duration;
			const float MoveFraction = LerpCurve? LerpCurve->GetFloatValue(Elapsed): Elapsed;
			const float NewFOV = FMath::Lerp<float, float>(StartFOV, TargetFOV, MoveFraction);

			CameraComponent->SetFieldOfView(NewFOV);
		}
	}
	else
	{
		bIsFinished = true;
		EndTask();
	}
}

void UGHOAbilityTask_WaitChangeFOV::OnDestroy(bool AbilityIsEnding)
{
	Super::OnDestroy(AbilityIsEnding);
}
