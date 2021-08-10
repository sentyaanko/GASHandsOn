// (C) Sentya Anko 2021


#include "Characters/Abilities/GHOGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Characters/Components/GHOAbilitySystemComponent.h"
#include "Settings/GHOGameplayTags.h"


UGHOGameplayAbility::UGHOGameplayAbility()
{
	bActivateOnInput = true;
	bCannotActivateWhileInteracting = true;


	/*
	by GASDocumentation
		Default to Instance Per Actor
	和訳
		デフォルトは InstancedPerActor
	*/
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	/*
	by GASDocumentation
		Default tags that block this ability from activating
	和訳
		このアビリティの発動を阻止するデフォルトのタグ
	*/
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TAG_State_Dead)));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TAG_State_KnockedDown)));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TAG_State_CrowdControl_Stun)));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TAG_Ability_BlocksInteraction)));

}

void UGHOGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bActivateAbilityOnGranted)
	{
		bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

bool UGHOGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (bCannotActivateWhileInteracting)
	{
		if(UGHOAbilitySystemComponent* ASC = Cast<UGHOAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
		{
			if (ASC->IsInteractingBeforeRemoval())
			{
				return false;
			}
		}
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

bool UGHOGameplayAbility::IsInputPressed() const
{
	if (FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		return Spec->InputPressed;

	}
	return false;
}
