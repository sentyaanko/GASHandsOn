// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GHOAbilityTask_WaitChangeFOV.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangeFOVDelegate);

/**
 */
UCLASS()
class GASHANDSON_API UGHOAbilityTask_WaitChangeFOV : public UAbilityTask
{
	GENERATED_BODY()

public:
	UGHOAbilityTask_WaitChangeFOV();

	/*
	by GASSHooter
		Change the FOV to the specified value, using the float curve (range 0 - 1) or fallback to linear interpolation
	和訳
		FOV を指定されった値に変更します。 float curve (range 0 - 1) または linear interpolation にフォールバックします。
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UGHOAbilityTask_WaitChangeFOV* WaitChangeFOV(UGameplayAbility* OwningAbility, FName TaskInstanceName, class UCameraComponent* CameraComponent, float TargetFOV, float Duration, UCurveFloat* OptionalInterpolationCurve);

	// UGameplayTask interface
public:
	/*
	by Epic
		Tick function for this task, if bTickingTask == true
	和訳
		このタスクの Tick 関数。 bTickingTask == true の場合。
	*/
	virtual void TickTask(float DeltaTime) override;

protected:
	/*
	by Epic
		Called to trigger the actual task once the delegates have been set up
		Note that the default implementation does nothing and you don't have to call it
	和訳
		デリゲートが設定された後、実際のタスクの起動をするために呼び出される。
		なお、デフォルトの実装では何もしないので、これを呼び出す必要はありません。
	*/
	virtual void Activate() override;

	/*
	by Epic
		End and CleanUp the task - may be called by the task itself or by the task owner if the owner is ending.
		IMPORTANT! Do NOT call directly! Call EndTask() or TaskOwnerEnded()
		IMPORTANT! When overriding this function make sure to call Super::OnDestroy(bOwnerFinished) as the last thing,
			since the function internally marks the task as "Pending Kill", and this may interfere with internal BP mechanics
	和訳
		タスクの終了とクリーンアップ - タスク自身が呼び出すこともコーナーが終了する場合はタスクオーナーが呼び出すことも出来ます。
		重要！直接呼び出さないでください！ EndTask() 又は TaskOwnerEnded() を呼び出してください。
		重要！この関数をオーバーライドする際には、最後に Super::OnDestroy(bInOwnerFinished) を呼び出すようにしてください。
			なぜなら、この関数には内部的にタスクを「 Pending Kill 」とマークしており、 BP の内部メカニクスに支障をきたす可能性があるからです。
	解説
		直接の基底のアクセス指定子がおかしい。このクラスでは根っこに合わせます。
		UGameplayTask(protected) > UAbilityTask(public) > UGHOAbilityTask_WaitReceiveDamage(protected)
	*/
	virtual void OnDestroy(bool AbilityEnded) override;
	// End of UGameplayTask interface

private:
	void Initialize(class UCameraComponent* InCameraComponent, float InTargetFOV, float InDuration, UCurveFloat* InOptionalInterpolationCurve);

public:
	UPROPERTY(BlueprintAssignable)
	FChangeFOVDelegate OnTargetFOVReached;

protected:
	bool bIsFinished;

	float StartFOV;

	float TargetFOV;

	float Duration;

	float TimeChangeStarted;

	float TimeChangeWillEnd;

	class UCameraComponent* CameraComponent;

	UCurveFloat* LerpCurve;
};
