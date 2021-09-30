// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GHOAbilityTask_ServerWaitForClientTargetData.generated.h"

/**
 *
 */
UCLASS()
class GASHANDSON_API UGHOAbilityTask_ServerWaitForClientTargetData : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
	static UGHOAbilityTask_ServerWaitForClientTargetData* ServerWaitForClientTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, bool TriggerOnce);

	// UGameplayTask interface
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
	void Initialize(bool TriggerOnce);

	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

protected:
	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataDelegate	ValidData;

private:
	bool bTriggerOnce;
};
