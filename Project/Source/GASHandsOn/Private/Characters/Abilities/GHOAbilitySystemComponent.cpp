// (C) Sentya Anko 2021


#include "Characters/Abilities/GHOAbilitySystemComponent.h"
#include "Characters/GHOCharacterBase.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "Characters/Abilities/GHOAttributeSetBase.h"

UGHOAbilitySystemComponent::UGHOAbilitySystemComponent()
{
	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	StunTag = FGameplayTag::RequestGameplayTag(FName("State.CrowdControl.Stun"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));
}

void UGHOAbilitySystemComponent::InitializeAttributes(class AGHOCharacterBase* InSourceObject)
{
	UGHOAttributeSetBase* AttributeSet = InSourceObject->GetAttributeSet();

	const TSubclassOf<class UGameplayEffect>& DefaultAttributes = InSourceObject->GetDefaultAttributes();

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(InSourceObject);

	FGameplayEffectSpecHandle NewHandle = MakeOutgoingSpec(DefaultAttributes, AttributeSet->GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), this);
	}

}

void UGHOAbilitySystemComponent::AddCharacterAbilities(class AGHOCharacterBase* InSourceObject)
{
	// Grant abilities, but only on the server	
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

bool UGHOAbilitySystemComponent::IsStun() const
{
	return HasMatchingGameplayTag(StunTag);
}

