// (C) Sentya Anko 2021


#include "Characters/Components/GHOAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Characters/GHOCharacterBase.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "Characters/AttributeSets/GHOAttributeSetBase.h"
#include "Settings/GHOGameplayTags.h"

UGHOAbilitySystemComponent::UGHOAbilitySystemComponent()
{
	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName(TAG_State_Dead));
	KnockedDownTag = FGameplayTag::RequestGameplayTag(TAG_State_KnockedDown);
	StunTag = FGameplayTag::RequestGameplayTag(FName(TAG_State_CrowdControl_Stun));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName(TAG_Effect_RemoveOnDeath));
}

void UGHOAbilitySystemComponent::InitializeAttributes(class AGHOCharacterBase* InSourceObject)
{
	UGHOAttributeSetBase* AttributeSet = InSourceObject->GetAttributeSet();

	const TSubclassOf<class UGameplayEffect>& DefaultAttributes = InSourceObject->GetDefaultAttributes();

	/*
	by GASDocumentation
		Can run on Server and Client
	和訳
		サーバーとクライアントの両方で動作可能
	*/
	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(InSourceObject);

	FGameplayEffectSpecHandle NewHandle = MakeOutgoingSpec(DefaultAttributes, AttributeSet->GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), this);
	}

	/*
	解説
		死亡状態を解除します。
		元々は AddCharacterAbilities() より後のタイミングで呼び出されていました。
		ActivateAbilityOnGranted であり、Activation Block Tags に State.Dead が設定されているアビリティ
		（例えば PassiveArmor ）が CharacterAbilities に設定されていた場合、
		AddCharacterAbilities() より前に ClearDead() しておかないと、リスポーン時にアビリティが有効化されません。
	*/
	ClearDead();

}

void UGHOAbilitySystemComponent::AddCharacterAbilities(class AGHOCharacterBase* InSourceObject)
{
	/*
	by GASDocumentation
		Grant abilities, but only on the server	
	和訳
		アビリティの付与、ただしサーバー上のみ
	*/
	if (bCharacterAbilitiesGiven)
	{
		return;
	}
	const TArray<TSubclassOf<class UGHOGameplayAbility>>& CharacterAbilities = InSourceObject->GetCharacterAbilities();
	for (const TSubclassOf<UGHOGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(StartupAbility, InSourceObject->GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), InSourceObject));
	}

	bCharacterAbilitiesGiven = true;
}

void UGHOAbilitySystemComponent::RemoveCharacterAbilities(class AGHOCharacterBase* InSourceObject)
{
	if (!bCharacterAbilitiesGiven)
	{
		return;
	}
	const TArray<TSubclassOf<class UGHOGameplayAbility>>& CharacterAbilities = InSourceObject->GetCharacterAbilities();

	/*
	by GASDocumentation
		Remove any abilities added from a previous call.
		This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	和訳
		以前の呼び出しで追加されたアビリティを削除する。
		起動した 'CharacterAbilities' 配列にアビリティがあるかどうか確認します。
	*/
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if ((Spec.SourceObject == InSourceObject) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	/*
	by GASDocumentation
		Do in two passes so the removal happens after we have the full list
	和訳
		2つのパスを実行し、完全なリストを手に入れたあとに削除を行います。
	*/
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		ClearAbility(AbilitiesToRemove[i]);
	}

	bCharacterAbilitiesGiven = false;
}

void UGHOAbilitySystemComponent::AddStartupEffects(class AGHOCharacterBase* InSourceObject)
{
	if (bStartupEffectsApplied)
	{
		return;
	}
	UGHOAttributeSetBase* AttributeSet = InSourceObject->GetAttributeSet();
	const TArray<TSubclassOf<class UGameplayEffect>>& StartupEffects = InSourceObject->GetStartupEffects();

	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(InSourceObject);

	for (const TSubclassOf<UGameplayEffect>& GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = MakeOutgoingSpec(GameplayEffect, AttributeSet->GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), this);
		}
	}

	bStartupEffectsApplied = true;
}

void UGHOAbilitySystemComponent::ClearDead()
{
	/*
	by GASDocumentation
		Forcibly set the DeadTag count to 0
	和訳
		DeadTag のカウントを強制的に 0 に設定します。
	*/
	SetTagMapCount(DeadTag, 0);
}

bool UGHOAbilitySystemComponent::IsDead() const
{
	return HasMatchingGameplayTag(DeadTag);
}

void UGHOAbilitySystemComponent::Die()
{
	CancelAllAbilities();

	FGameplayTagContainer EffectTagsToRemove;
	EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
	const int32 NumEffectsRemoved = RemoveActiveEffectsWithTags(EffectTagsToRemove);

	AddLooseGameplayTag(DeadTag);

}

bool UGHOAbilitySystemComponent::IsKnockedDown() const
{
	return HasMatchingGameplayTag(KnockedDownTag);
}

void UGHOAbilitySystemComponent::Down(const class UGameplayEffect* GameplayEffect)
{
	CancelAllAbilities();

	FGameplayTagContainer EffectTagsToRemove;
	EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
	int32 NumEffectsRemoved = RemoveActiveEffectsWithTags(EffectTagsToRemove);

	ApplyGameplayEffectToSelf(GameplayEffect, 1.0f, MakeEffectContext());
}

bool UGHOAbilitySystemComponent::IsStun() const
{
	return HasMatchingGameplayTag(StunTag);
}

void UGHOAbilitySystemComponent::CancelAbilitiesByStun()
{
	FGameplayTagContainer AbilityTagsToCancel;
	AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName(TAG_Ability)));

	FGameplayTagContainer AbilityTagsToIgnore;
	AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName(TAG_Ability_NotCanceledByStun)));

	CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
}

void UGHOAbilitySystemComponent::ReceiveDamage(UGHOAbilitySystemComponent * SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}

void UGHOAbilitySystemComponent::ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Executed, GameplayCueParameters);
}

void UGHOAbilitySystemComponent::AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::OnActive, GameplayCueParameters);
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::WhileActive, GameplayCueParameters);
}

void UGHOAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Removed, GameplayCueParameters);
}
