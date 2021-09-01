// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GHOAbilityTask_WaitDelayOneFrame.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitDelayOneFrameDelegate);

/**
by GASShooter
	Like WaitDelay but only delays one frame (tick).
和訳
	WaitDelay とにていますが、 1 フレーム（ティック）だけ遅らせます。
 */
UCLASS()
class GASHANDSON_API UGHOAbilityTask_WaitDelayOneFrame : public UAbilityTask
{
	GENERATED_BODY()

public:
	// Wait one frame.
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UGHOAbilityTask_WaitDelayOneFrame* WaitDelayOneFrame(UGameplayAbility* OwningAbility);

	// UGameplayTask interface
protected:
	/*
	by Epic
		Called to trigger the actual task once the delegates have been set up
		Note that the default implementation does nothing and you don't have to call it
	和訳
		デリゲートが設定された後、実際のタスクの起動をするために呼び出される。
		なお、デフォルトの実装では何もしないので、これを呼びdス必要はありません。
	*/
	virtual void Activate() override;

	// End of UGameplayTask interface

private:
	void OnDelayFinish();

public:
	UPROPERTY(BlueprintAssignable)
	FWaitDelayOneFrameDelegate OnFinish;
};
