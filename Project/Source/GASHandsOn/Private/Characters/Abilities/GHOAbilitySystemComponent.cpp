// (C) Sentya Anko 2021


#include "Characters/Abilities/GHOAbilitySystemComponent.h"
#include "Characters/GHOCharacterBase.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "Characters/Abilities/GHOAttributeSetBase.h"

UGHOAbilitySystemComponent::UGHOAbilitySystemComponent()
{
	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));
}

void UGHOAbilitySystemComponent::InitializeAttributes(class AGHOCharacterBase* InSourceObject)
{
	UGHOAttributeSetBase* AttributeSet = InSourceObject->GetAttributeSet();

	TSubclassOf<class UGameplayEffect>& DefaultAttributes = InSourceObject->GetDefaultAttributes();

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

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if ((Spec.SourceObject == InSourceObject) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		ClearAbility(AbilitiesToRemove[i]);
	}

	bCharacterAbilitiesGiven = false;

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

