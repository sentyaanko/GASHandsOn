// (C) Sentya Anko 2021


#include "Characters/Abilities/AbilityTasks/GHOAbilityTask_WaitReceiveDamage.h"
#include "Characters/Abilities/GHOAbilitySystemComponent.h"

UGHOAbilityTask_WaitReceiveDamage::UGHOAbilityTask_WaitReceiveDamage()
{
	bTriggerOnce = false;
}

UGHOAbilityTask_WaitReceiveDamage* UGHOAbilityTask_WaitReceiveDamage::WaitReceiveDamage(UGameplayAbility* OwningAbility, bool bInTriggerOnce)
{
	UGHOAbilityTask_WaitReceiveDamage* MyObj = NewAbilityTask<UGHOAbilityTask_WaitReceiveDamage>(OwningAbility);
	MyObj->bTriggerOnce = bInTriggerOnce;
	return MyObj;
}

void UGHOAbilityTask_WaitReceiveDamage::Activate()
{
	if (UGHOAbilitySystemComponent* asc = Cast<UGHOAbilitySystemComponent>(AbilitySystemComponent))
	{
		asc->GetReceivedDamage().AddDynamic(this, &UGHOAbilityTask_WaitReceiveDamage::OnDamageReceived);
	}
}

void UGHOAbilityTask_WaitReceiveDamage::OnDestroy(bool AbilityIsEnding)
{
	if (UGHOAbilitySystemComponent* asc = Cast<UGHOAbilitySystemComponent>(AbilitySystemComponent))
	{
		asc->GetReceivedDamage().RemoveDynamic(this, &UGHOAbilityTask_WaitReceiveDamage::OnDamageReceived);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UGHOAbilityTask_WaitReceiveDamage::OnDamageReceived(UGHOAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}

	if (bTriggerOnce)
	{
		EndTask();
	}
}
