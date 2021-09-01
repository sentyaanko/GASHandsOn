// (C) Sentya Anko 2021

#include "Characters/AbilityTasks/GHOAbilityTask_WaitDelayOneFrame.h"

void UGHOAbilityTask_WaitDelayOneFrame::Activate()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UGHOAbilityTask_WaitDelayOneFrame::OnDelayFinish);
}

UGHOAbilityTask_WaitDelayOneFrame* UGHOAbilityTask_WaitDelayOneFrame::WaitDelayOneFrame(UGameplayAbility* OwningAbility)
{
	UGHOAbilityTask_WaitDelayOneFrame* MyObj = NewAbilityTask<UGHOAbilityTask_WaitDelayOneFrame>(OwningAbility);
	return MyObj;
}

void UGHOAbilityTask_WaitDelayOneFrame::OnDelayFinish()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnFinish.Broadcast();
	}
	EndTask();
}
