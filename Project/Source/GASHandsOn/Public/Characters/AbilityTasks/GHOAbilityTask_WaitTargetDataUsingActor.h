// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameplayTagContainer.h"
#include "GHOAbilityTask_WaitTargetDataUsingActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitTargetDataUsingActorDelegate, const FGameplayAbilityTargetDataHandle&, Data);

/**
by GASShooter
	Waits for TargetData from an already spawned TargetActor and does *NOT* destroy it when it receives data.

	The original WaitTargetData's comments expects us to subclass it heavily, but the majority of its functions are not virtual. 
	Therefore this is a total rewrite of it to add bCreateKeyIfNotValidForMorePredicting functionality.
和訳
	既にスポーンされた TargetActor からの TargetData を待ち、データを受け取っても破棄 *しません*。

	オリジナルの WaitTargetData のコメントでは、これを大幅にサブクラス化することが期待されていますが、その大部分は仮想関数ではありません。
	そのため、これは bCreateKeyIfNotValidForMorePredicting の機能を追加するための完全な書き換えです。
解説
	UAbilityTask_WaitTargetData のことです。
	コードのつくりが似ている箇所が多々あります。
 */
UCLASS()
class GASHANDSON_API UGHOAbilityTask_WaitTargetDataUsingActor : public UAbilityTask
{
	GENERATED_BODY()

public:
	/*
	by Epic
		Uses specified spawned TargetActor and waits for it to return valid data or to be canceled. 
		The TargetActor is *NOT* destroyed.
		
		@param bCreateKeyIfNotValidForMorePredicting
			Will create a new scoped prediction key if the current scoped prediction key is not valid for more predicting.
			If false, it will always create a new scoped prediction key. 
			We would want to set this to true if we want to use a potentially existing valid scoped prediction key like the ability's activation key in a batched ability.
	和訳
		指定されたスポーン済みの TargetActor を使用し、有効なデータを返すかキャンセルされるのを待ちます。
		TargetActor は破棄*されません*。
		
		@param bCreateKeyIfNotValidForMorePredicting
			現在の scoped prediction key （スコープ付き予測キー）がより多くの predicting （予測）に有効でない場合、	新しい scoped prediction key （スコープ付き予測キー）を作成します。
			false の場合、常に新しい scoped prediction key （スコープ付き予測キー）を作成します。
			バッチ処理されたアビリティのアビリティのアクティベーションキーのように、有効な scoped prediction key （スコープ付き予測キー）が存在する可能性がある場合にはこれを true に設定します。
	*/
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
	static UGHOAbilityTask_WaitTargetDataUsingActor* WaitTargetDataWithReusableActor(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType,
		AGameplayAbilityTargetActor* InTargetActor,
		bool bCreateKeyIfNotValidForMorePredicting = false
	);

	// UGameplayTask interface
public:
	/*
	by Epic
		Called when the ability is asked to confirm from an outside node. 
		What this means depends on the individual task. 
		By default, this does nothing other than ending if bEndTask is true.
	和訳
		アビリティが外部のノードから確認を求められた時に呼び出されます。
		これが何を意味するかは個々のタスクによります。
		デフォルトでは、 bEndTask が true であれば終了する以外に何もしません。
	*/
	virtual void ExternalConfirm(bool bEndTask) override;

	/*
	by Epic
		Called when the ability is asked to cancel from an outside node. 
		What this means depends on the individual task. 
		By default, this does nothing other than ending the task.
	和訳
		アビリティが外部のノードからキャンセルを求められた時に呼び出されます。
		これが何を意味するかは個々のタスクによります。
		デフォルトでは、 タスクを終了する以外に何もしません。
	*/
	virtual void ExternalCancel() override;

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
	UFUNCTION()
	virtual void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

	UFUNCTION()
	virtual void OnTargetDataReplicatedCancelledCallback();

	UFUNCTION()
	virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	virtual void OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data);

private:
	void Initialize(AGameplayAbilityTargetActor* InTargetActor, TEnumAsByte<EGameplayTargetingConfirmation::Type> InConfirmationType, bool bInCreateKeyIfNotValidForMorePredicting = false );

	virtual void InitializeTargetActor() const;
	virtual void FinalizeTargetActor() const;

	virtual void RegisterTargetDataCallbacks();

	virtual bool ShouldReplicateDataToServer() const;

public:
	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataUsingActorDelegate ValidData;

	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataUsingActorDelegate Cancelled;

protected:
	UPROPERTY()
	AGameplayAbilityTargetActor* TargetActor;

	bool bCreateKeyIfNotValidForMorePredicting;

	TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType;

	FDelegateHandle OnTargetDataReplicatedCallbackDelegateHandle;

};
