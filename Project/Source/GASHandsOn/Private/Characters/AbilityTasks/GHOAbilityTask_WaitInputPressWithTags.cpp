// (C) Sentya Anko 2021


#include "Characters/AbilityTasks/GHOAbilityTask_WaitInputPressWithTags.h"
#include "AbilitySystemComponent.h"
#include "Settings/GHOGameplayTags.h"

//解説
//	このクラスは 
//		Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/Abilities/Tasks/AbilityTask_WaitInputPress.h
//	で宣言されている
//		UAbilityTask_WaitInputPress
//	を拡張したものです。

UGHOAbilityTask_WaitInputPressWithTags::UGHOAbilityTask_WaitInputPressWithTags()
{
	StartTime = 0.f;
	bTestInitialState = false;
}

UGHOAbilityTask_WaitInputPressWithTags* UGHOAbilityTask_WaitInputPressWithTags::WaitInputPressWithTags(UGameplayAbility* OwningAbility, FGameplayTagContainer RequiredTags, FGameplayTagContainer IgnoredTags, bool bTestAlreadyPressed)
{
	UGHOAbilityTask_WaitInputPressWithTags* Task = NewAbilityTask<UGHOAbilityTask_WaitInputPressWithTags>(OwningAbility);
	Task->Initialize(OwningAbility, RequiredTags, IgnoredTags, bTestAlreadyPressed);
	return Task;
}

void UGHOAbilityTask_WaitInputPressWithTags::Initialize(UGameplayAbility* InOwningAbility, FGameplayTagContainer InRequiredTags, FGameplayTagContainer InIgnoredTags, bool bInTestAlreadyPressed)
{
	bTestInitialState = bInTestAlreadyPressed;
	RequiredTags = InRequiredTags;
	IgnoredTags = InIgnoredTags;
}

void UGHOAbilityTask_WaitInputPressWithTags::Activate()
{
	StartTime = GetWorld()->GetTimeSeconds();
	if (Ability)
	{
		if (bTestInitialState && IsLocallyControlled())
		{
			if (FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec())
			{
				if (Spec->InputPressed)
				{
					OnPressCallback();
					return;
				}
			}
		}

		SetDelegate();
	}
}

void UGHOAbilityTask_WaitInputPressWithTags::OnPressCallback()
{
	const float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

	if (!Ability || !AbilitySystemComponent)
	{
		EndTask();
		return;
	}

	//by GASShooter
	//	TODO move this into a tag query
	//解説
	//	TODO これをタグクエリに移す。
	if (AbilitySystemComponent->HasAnyMatchingGameplayTags(IgnoredTags) || !AbilitySystemComponent->HasAllMatchingGameplayTags(RequiredTags))
	{
		Reset();
		return;
	}

	//by GASShooter
	//	TODO extend tag query to support this and move this into it
	//		Hardcoded for GA_InteractPassive to ignore input while already interacting
	//解説
	//	TODO タグクエリを拡張してこれをサポートし、これをそこに移す。
	//		GA_InteractPassive のためのハードコードで、すでにインタラクトしている場合は入力を無視する。
	if (AbilitySystemComponent->GetTagCount(FGameplayTag::RequestGameplayTag(TAG_State_Interacting))
		> AbilitySystemComponent->GetTagCount(FGameplayTag::RequestGameplayTag(TAG_State_Interacting_Removal)))
	{
		Reset();
		return;
	}

	RemoveDelegate();

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, IsPredictingClient());

	if (IsPredictingClient())
	{
		//by GASShooter
		//	Tell the server about this
		//解説
		//	サーバーにこれを伝える
		AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
	}
	else
	{
		AbilitySystemComponent->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	//by GASShooter
	//	We are done. Kill us so we don't keep getting broadcast messages
	//解説
	//	終わりました。キルすればブロードキャストメッセージを受け取り続けることはなくなります。
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnPress.Broadcast(ElapsedTime);
	}

	EndTask();
}

void UGHOAbilityTask_WaitInputPressWithTags::OnDestroy(bool AbilityEnded)
{
	RemoveDelegate();

	ClearWaitingOnRemotePlayerData();

	Super::OnDestroy(AbilityEnded);
}

void UGHOAbilityTask_WaitInputPressWithTags::Reset()
{
	RemoveDelegate();
	SetDelegate();
}

void UGHOAbilityTask_WaitInputPressWithTags::SetDelegate()
{
	DelegateHandle = AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &UGHOAbilityTask_WaitInputPressWithTags::OnPressCallback);
	if (IsForRemoteClient())
	{
		if (!AbilitySystemComponent->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey()))
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UGHOAbilityTask_WaitInputPressWithTags::RemoveDelegate()
{
	AbilitySystemComponent->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);
}
