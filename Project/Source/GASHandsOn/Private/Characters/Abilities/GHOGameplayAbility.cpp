// (C) Sentya Anko 2021


#include "Characters/Abilities/GHOGameplayAbility.h"
#include "AbilitySystemComponent.h"


UGHOGameplayAbility::UGHOGameplayAbility()
{
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
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.CrowdControl.Stun")));

}

void UGHOGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bActivateAbilityOnGranted)
	{
		bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
