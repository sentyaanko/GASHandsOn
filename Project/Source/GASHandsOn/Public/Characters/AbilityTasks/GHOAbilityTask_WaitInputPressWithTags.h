// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GHOAbilityTask_WaitInputPressWithTags.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputPressWithTagsDelegate, float, TimeWaited);

/**
by GASShooter
	Waits until the input is pressed from activating an ability and the ASC has the required tags and not the ignored tags.
	This should be true immediately upon starting the ability, since the key was pressed to activate it. 
	We expect server to	execute this task in parallel and keep its own time.
和訳
	アビリティの起動により入力が押され、 ASC が必要なタグを持ち、無視するタグを持たない状態になるまで待ちます。
	これは、アビリティを起動するためにキーが押されたので、アビリティを起動した直後に当てはまるはずです。
	サーバーがこのタスクを並行して実行し、独自の時間を保つことを期待しています。
*/
UCLASS()
class GASHANDSON_API UGHOAbilityTask_WaitInputPressWithTags : public UAbilityTask
{
	GENERATED_BODY()

public:
	UGHOAbilityTask_WaitInputPressWithTags();
	/**
	by GASShooter
		Wait until the user presses the input button for this ability's activation. 
		Returns time this node spent waiting for the press. 
		Will return 0 if input was already down.
		This is hardcoded for GA_InteractPassive to not fire when State.Interacting TagCount is > State.InteractingRemoval TagCount.
		//TODO Ideally the RequiredTags, IgnoredTags, and State.Interacting TagCount would get moved into a subclass of FGameplayTagQuery and then we'd only expose that as one parameter and rename the task to WaitInputPress_Query.
		
		@param RequiredTags Ability Owner must have all of these tags otherwise the input is ignored.
		@param IgnoredTags Ability Owner cannot have any of these tags otherwise the input is ignored.
	和訳
		このアビリティの起動のために、ユーザが入力ボタンを押すまで待ちます。
		このノードがボタンを押すのを待っていた時間を返します。
		入力がすでに押されていた場合は 0 を返します。
		これは、 GA_InteractPassive が、タグカウントが State.Interacting TagCount is > State.InteractingRemoval の時に発動しないようにハードコードされています。
		//TODO: 理想的には、 RequiredTags 、 IgnoredTags 、 State.Interacting のタグカウントを FGameplayTagQuery のサブクラスに移動し、それを1つだけ公開し、タスクの名前を WaitInputPress_Query にすることです。

		@param RequiredTags アビリティオーナーがこれらのタグのすべてを持たなければならず、さもなくば入力は無視されます。
		@param IgnoredTags アビリティオーナーはこれらのタグのいずれかも持ってはならず、さもなくば入力は無視されます。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UGHOAbilityTask_WaitInputPressWithTags* WaitInputPressWithTags(UGameplayAbility* OwningAbility, FGameplayTagContainer RequiredTags, FGameplayTagContainer IgnoredTags, bool bTestAlreadyPressed = false);

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
	
protected:
	/**
	by GASShooter
		We can only listen for one input pressed event.
		I think it's because `UAbilitySystemComponent::InvokeReplicatedEvent` sets `ReplicatedData->GenericEvents[(uint8)EventType].bTriggered = true;`
		So if we want to keep listening for more input events, we just clear the delegate handle and bind again.
	解説
		InputPress イベントは1つしかリッスンできません。
		これは、 `UAbilitySystemComponent::InvokeReplicatedEvent` が `ReplicatedData->GenericEvents[(uint8)EventType].bTriggered = true;` を設定しているからだと思われます。
		そのため、更に入力イベントをリッスンし続けたい場合は、デリゲートハンドルをクリアして、再度バインドすればよいのです。
		So if we want to keep listening for more input events, we just clear the delegate handle and bind again.
	*/
	virtual void Reset();

private:
	void Initialize(UGameplayAbility* InOwningAbility, FGameplayTagContainer InRequiredTags, FGameplayTagContainer InIgnoredTags, bool bInTestAlreadyPressed);

	UFUNCTION()
	void OnPressCallback();

	void SetDelegate();
	void RemoveDelegate();

public:
	UPROPERTY(BlueprintAssignable)
	FInputPressWithTagsDelegate OnPress;

protected:
	float StartTime;

	bool bTestInitialState;

	FDelegateHandle DelegateHandle;

	FGameplayTagContainer RequiredTags;
	FGameplayTagContainer IgnoredTags;
};
