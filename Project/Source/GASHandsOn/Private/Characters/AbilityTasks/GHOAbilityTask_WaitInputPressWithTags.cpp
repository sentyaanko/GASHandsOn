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

#if 1
		//解説
		//	UAbilityTask_WaitInputPress との差異
		//	行っていることは同じですが、拡張の際に同じコードが必要となったので関数化しています。
		SetDelegate();
#endif
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

#if 1
	//解説
	//	GASShooter で機能拡張している部分です。

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
#endif

#if 1
	//解説
	//	UAbilityTask_WaitInputPress との差異
	//	行っていることは同じですが、拡張の際に同じコードが必要となったので関数化しています。
	RemoveDelegate();
#endif

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, IsPredictingClient());

	if (IsPredictingClient())
	{
		//by Epic
		//	Tell the server about this
		//解説
		//	サーバーにこれを伝える
		AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
	}
	else
	{
		AbilitySystemComponent->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	//by Epic
	//	We are done. Kill us so we don't keep getting broadcast messages
	//解説
	//	終わりました。キルすればブロードキャストメッセージを受け取り続けることはなくなります。
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnPress.Broadcast(ElapsedTime);
	}

	EndTask();
}

//解説
//	以下は GASShooter で追加している関数です。

void UGHOAbilityTask_WaitInputPressWithTags::OnDestroy(bool AbilityEnded)
{
	RemoveDelegate();

	//解説
	//	>	//by Eipc
	//	>	//	Called when the ability task is waiting on remote player data. 
	//	>	//	IF the remote player ends the ability prematurely, and a task with this set is still running, the ability is killed.
	//	>	//和訳
	//	>	//	アビリティタスクがリモートプレイヤーのデータを待っている時に呼び出されます。
	//	>	//	リモートプレイヤーがアビリティを早期に終了させ、これが設定されたタスクがまだ実行されている場合、そのアビリティはキルされます。
	//	>	void SetWaitingOnRemotePlayerData();
	//	>	void ClearWaitingOnRemotePlayerData();
	//	>	virtual bool IsWaitingOnRemotePlayerdata() const override;
	//	呼び出している関数は Clear なので、 リモートプレイヤーがアビリティを早期に終了させた際にキルされないようにしています。
	//	同様なことを行っているアビリティタスクが UE4 のソース内にもないため、行っている理由は不明です。
	//		TODO:理解するためには、前提としてネットワークを通じて GameplayAbility がどのように実行されているかを理解していることが必要なようなので、確認は一旦保留します。
	//	SetDelegate() では SetWaitingOnRemotePlayerData() を呼び出しています。
	//	SetWaitingOnRemotePlayerData() の件については（ UAbilityTask_WaitInputRelease のケースではありますが） GA_InteractActive のコメントにも情報があります。
	ClearWaitingOnRemotePlayerData();

	Super::OnDestroy(AbilityEnded);
}

//解説
//	以下は共通部分をくくりだして関数化したものたちです。

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
