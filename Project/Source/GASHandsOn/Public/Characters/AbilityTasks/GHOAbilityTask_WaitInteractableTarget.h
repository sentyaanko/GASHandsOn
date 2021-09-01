// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Engine/CollisionProfile.h"
#include "GHOAbilityTask_WaitInteractableTarget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitInteractableTargetDelegate, const FGameplayAbilityTargetDataHandle&, Data);

/**
by GASShooter
	Performs a line trace on a timer, looking for an Actor that implements IGHOInteractable that is available for interaction.
	The StartLocations are hardcoded for GASShooter since we can be in first and third person so we have to check every time we trace. 
	If you only have one start location, you should make it more generic with a parameter on your AbilityTask node.
和訳
	タイマーでライントレースを実行し、インタラクト可能な IGHOInteractable を実装したアクターを検索します。
	StartLocations は GASShooter ではハードコードされていて、一人称と三人称があるので、トレースするたびにチェックしなければなりません。
	StartLocation が1つしかない場合は、 AbilityTask ノードにパラメータを指定して、より一般的なものにする必要があります。
*/
UCLASS()
class GASHANDSON_API UGHOAbilityTask_WaitInteractableTarget : public UAbilityTask
{
	GENERATED_BODY()

public:
	UGHOAbilityTask_WaitInteractableTarget();
	/**
	by GASShooter
		Traces a line on a timer looking for InteractableTargets.
		@param MaxRange How far to trace.
		@param TimerPeriod Period of trace timer.
		@param bShowDebug Draws debug lines for traces.
	和訳
		InteractableTargets を探すためにタイマーでライントレースを実行します。
		@param MaxRange どこまでトレースするか
		@param TimerPeriod トレースタイマーの周期
		@param bShowDebug トレースのデバッグラインを描画するか
	*/
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
	static UGHOAbilityTask_WaitInteractableTarget* WaitForInteractableTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, FCollisionProfileName TraceProfile, float MaxRange = 200.0f, float TimerPeriod = 0.1f, bool bShowDebug = true);

	// UGameplayTask interface
public:
	/*
	by Epic
		Called to trigger the actual task once the delegates have been set up
		Note that the default implementation does nothing and you don't have to call it
	和訳
		デリゲートが設定された後、実際のタスクの起動をするために呼び出される。
		なお、デフォルトの実装では何もしないので、これを呼びdス必要はありません。
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

protected:
	/** Traces as normal, but will manually filter all hit actors */
	void LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params, bool bLookForInteractableActor) const;

	void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, FVector& OutTraceEnd, bool bIgnorePitch = false) const;

	bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition) const;

	UFUNCTION()
	void PerformTrace();

	FGameplayAbilityTargetDataHandle MakeTargetData(const FHitResult& HitResult) const;

private:
	void Initialize(UGameplayAbility* InOwningAbility, FName InTaskInstanceName, FCollisionProfileName InTraceProfile, float InMaxRange, float InTimerPeriod, bool bInShowDebug);

public:
	UPROPERTY(BlueprintAssignable)
	FWaitInteractableTargetDelegate FoundNewInteractableTarget;

	UPROPERTY(BlueprintAssignable)
	FWaitInteractableTargetDelegate LostInteractableTarget;

protected:
	FGameplayAbilityTargetingLocationInfo StartLocation;
	FGameplayAbilityTargetingLocationInfo StartLocation1P;
	FGameplayAbilityTargetingLocationInfo StartLocation3P;

	float MaxRange;

	float TimerPeriod;

	bool bShowDebug;

	bool bTraceAffectsAimPitch;

	FCollisionProfileName TraceProfile;

	FGameplayAbilityTargetDataHandle TargetData;

	FTimerHandle TraceTimerHandle;
};
