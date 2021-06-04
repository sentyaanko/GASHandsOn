// (C) Sentya Anko 2021


#include "Characters/AsyncTasks/GHOAsyncTaskCooldownChanged.h"

UGHOAsyncTaskCooldownChanged * UGHOAsyncTaskCooldownChanged::ListenForCooldownChange(UAbilitySystemComponent* InAbilitySystemComponent, FGameplayTagContainer InCooldownTags, bool bInUseServerCooldown)
{
	/*
	解説
		GASDocumentation では、まず NewObject() した後に引数チェックを行い、適切でない場合は EndTask() の呼び出しを行っています。
		そうすることに特に意味がないように読み取れたため、ここでは引数チェックを先んじて行うように変更しています。
		また、メンバの初期化に関しても Initialize() 関数で行うように変更しています。
	*/
	if (!IsValid(InAbilitySystemComponent) || InCooldownTags.Num() < 1)
	{
		return nullptr;
	}
	UGHOAsyncTaskCooldownChanged* ListenForCooldownChange = NewObject<UGHOAsyncTaskCooldownChanged>();
	ListenForCooldownChange->Initialize(InAbilitySystemComponent, InCooldownTags, bInUseServerCooldown);

	return ListenForCooldownChange;
}

void UGHOAsyncTaskCooldownChanged::Initialize(UAbilitySystemComponent* InAbilitySystemComponent, FGameplayTagContainer InCooldownTags, bool bInUseServerCooldown)
{
	AbilitySystemComponent = InAbilitySystemComponent;
	CooldownTags = InCooldownTags;
	bUseServerCooldown = bInUseServerCooldown;

	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UGHOAsyncTaskCooldownChanged::OnActiveGameplayEffectAddedCallback);

	TArray<FGameplayTag> CooldownTagArray;
	CooldownTags.GetGameplayTagArray(CooldownTagArray);

	for (FGameplayTag CooldownTag : CooldownTagArray)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGHOAsyncTaskCooldownChanged::CooldownTagChanged);
	}
}

void UGHOAsyncTaskCooldownChanged::EndTask()
{
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);

		TArray<FGameplayTag> CooldownTagArray;
		CooldownTags.GetGameplayTagArray(CooldownTagArray);

		for (FGameplayTag CooldownTag : CooldownTagArray)
		{
			AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		}
	}

	SetReadyToDestroy();
	MarkPendingKill();
}

void UGHOAsyncTaskCooldownChanged::OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	TArray<FGameplayTag> CooldownTagArray;
	CooldownTags.GetGameplayTagArray(CooldownTagArray);

	for (FGameplayTag CooldownTag : CooldownTagArray)
	{
		if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
		{
			float TimeRemaining = 0.0f;
			float Duration = 0.0f;
			/*
			by GASDocumentation
				Expecting cooldown tag to always be first tag
			和訳
				cooldown タグが常に最初のタグであることを期待する。
			*/
			FGameplayTagContainer CooldownTagContainer(GrantedTags.GetByIndex(0));
			GetCooldownRemainingForTag(CooldownTagContainer, TimeRemaining, Duration);

			if (AbilitySystemComponent->GetOwnerRole() == ROLE_Authority)
			{
				/*
				by GASDocumentation
					Player is Server
				和訳
					プレイヤーがサーバーである
				*/
				OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			}
			else if (!bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
			{
				/*
				by GASDocumentation
					Client using predicted cooldown
				和訳
					predicted （予測）された cooldown を使用するクライアント
				*/
				OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			}
			else if (bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated() == nullptr)
			{
				/*
				by GASDocumentation
					Client using Server's cooldown. 
					This is Server's corrective cooldown GE.
				和訳
					サーバー cooldown を使用するクライアント
					これはサーバーの修正用 cooldown GameplayEffect です。
				*/
				OnCooldownBegin.Broadcast(CooldownTag, TimeRemaining, Duration);
			}
			else if (bUseServerCooldown && SpecApplied.GetContext().GetAbilityInstance_NotReplicated())
			{
				/*
				by GASDocumentation
					Client using Server's cooldown but this is predicted cooldown GE.
					This can be useful to gray out abilities until Server's cooldown comes in.
				和訳
					サーバー cooldown を使用するクライアントだが、これは predicted （予測）された cooldown GameplayEffect です。
					これは、サーバーのクールダウンが来るまでアビリティをグレイアウトさせるのに便利です。
				*/
				OnCooldownBegin.Broadcast(CooldownTag, -1.0f, -1.0f);
			}
		}
	}
}

void UGHOAsyncTaskCooldownChanged::CooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		OnCooldownEnd.Broadcast(CooldownTag, -1.0f, -1.0f);
	}
}

bool UGHOAsyncTaskCooldownChanged::GetCooldownRemainingForTag(FGameplayTagContainer InCooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	if (IsValid(AbilitySystemComponent) && InCooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}

	return false;
}
