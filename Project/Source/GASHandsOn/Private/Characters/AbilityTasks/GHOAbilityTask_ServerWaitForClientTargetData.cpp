// (C) Sentya Anko 2021

#include "Characters/AbilityTasks/GHOAbilityTask_ServerWaitForClientTargetData.h"
#include "AbilitySystemComponent.h"

UGHOAbilityTask_ServerWaitForClientTargetData* UGHOAbilityTask_ServerWaitForClientTargetData::ServerWaitForClientTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, bool TriggerOnce)
{
	UGHOAbilityTask_ServerWaitForClientTargetData* MyObj = NewAbilityTask<UGHOAbilityTask_ServerWaitForClientTargetData>(OwningAbility, TaskInstanceName);
	MyObj->Initialize(TriggerOnce);
	return MyObj;
}

void UGHOAbilityTask_ServerWaitForClientTargetData::Initialize(bool TriggerOnce)
{
	bTriggerOnce = TriggerOnce;
}

void UGHOAbilityTask_ServerWaitForClientTargetData::Activate()
{
	if (!Ability || !Ability->GetCurrentActorInfo()->IsNetAuthority())
	{
		return;
	}

	FGameplayAbilitySpecHandle	SpecHandle = GetAbilitySpecHandle();
	FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
	AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UGHOAbilityTask_ServerWaitForClientTargetData::OnTargetDataReplicatedCallback);
}

void UGHOAbilityTask_ServerWaitForClientTargetData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	FGameplayAbilityTargetDataHandle MutableData = Data;
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(MutableData);
	}

	if (bTriggerOnce)
	{
		EndTask();
	}
}

void UGHOAbilityTask_ServerWaitForClientTargetData::OnDestroy(bool AbilityEnded)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpecHandle	SpecHandle = GetAbilitySpecHandle();
		FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).RemoveAll(this);
	}

	Super::OnDestroy(AbilityEnded);
}
