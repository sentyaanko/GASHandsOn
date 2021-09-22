// (C) Sentya Anko 2021


#include "Characters/Abilities/GHOGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Characters/Components/GHOAbilitySystemComponent.h"
#include "Settings/GHOGameplayTags.h"
#include "Characters/GHOHeroCharacterBase.h"



UGHOGameplayAbility::UGHOGameplayAbility()
{
	bActivateOnInput = true;
	bCannotActivateWhileInteracting = true;
	bSourceObjectMustEqualCurrentWeaponToActivate = false;


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

	if (bSourceObjectMustEqualCurrentWeaponToActivate)
	{
		if (AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(ActorInfo->AvatarActor))
		{
			if (auto Weapon = Hero->GetCurrentWeapon())
			{
				if ((UObject*)Weapon == GetSourceObject(Handle, ActorInfo))
				{
					return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
				}
			}
		}
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

bool UGHOGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) && GHOCheckCost(Handle, *ActorInfo);
}

void UGHOGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	GHOApplyCost(Handle, *ActorInfo, ActivationInfo);
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

bool UGHOGameplayAbility::GHOCheckCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const
{
	return true;
}

void UGHOGameplayAbility::GHOApplyCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
}

UObject* UGHOGameplayAbility::K2_GetSourceObject(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const
{
	return GetSourceObject(Handle, &ActorInfo);
}

bool UGHOGameplayAbility::IsInputPressed() const
{
	if (FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		return Spec->InputPressed;

	}
	return false;
}

bool UGHOGameplayAbility::BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately)
{
	if (UGHOAbilitySystemComponent* ASC = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		return ASC->BatchRPCTryActivateAbility(InAbilityHandle, EndAbilityImmediately);
	}

	return false;
}

void UGHOGameplayAbility::ExternalEndAbility()
{
	check(CurrentActorInfo);

	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

//解説
//	GASShooter では BlueprintNativeEvent を使用しているため、 C++ の実装関数が必要。なのだが、実装が空。
//	GASHandsOn では  BlueprintImplementableEvent を使用し、 C++ の実装をなくしている。
//bool UGHOGameplayAbility::GHOCheckCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const
//{
//	return true;
//}
//
//void UGHOGameplayAbility::GHOApplyCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
//{
//}

FGHOGameplayEffectContainerSpec UGHOGameplayAbility::MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	if (FGHOGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag))
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
	}
	return FGHOGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> UGHOGameplayAbility::ApplyEffectContainerSpec(const FGHOGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	/*
	by GASShooter
		Iterate list of effect specs and apply them to their target data
	和訳
		EffectSpec のリストを反復処理し、それらのターゲットデータに適用する。
	*/
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

FGHOGameplayEffectContainerSpec UGHOGameplayAbility::MakeEffectContainerSpecFromContainer(const FGHOGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	/*
	by GASShooter
		First figure out our actor info
	和訳
		まず、 ActorInfo を把握します。
	*/
	FGHOGameplayEffectContainerSpec ReturnSpec;
	AActor* OwningActor = GetOwningActorFromActorInfo();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AGHOCharacterBase* AvatarCharacter = Cast<AGHOCharacterBase>(AvatarActor);
	UGHOAbilitySystemComponent* OwningASC = UGHOAbilitySystemComponent::GetAbilitySystemComponentFromActor(OwningActor);

	if (OwningASC)
	{
		/*
		by GASShooter
			If we have a target type, run the targeting logic.
			This is optional, targets can be added later
		和訳
			ターゲットタプがある場合、ターゲティングロジックを実行する。
			これはオプションで、ターゲットは後で追加できる。
		*/
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			TArray<FGameplayAbilityTargetDataHandle> TargetData;
			const UGHOTargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
			TargetTypeCDO->GetTargets(AvatarCharacter, AvatarActor, EventData, TargetData, HitResults, TargetActors);
			ReturnSpec.AddTargets(TargetData, HitResults, TargetActors);
		}

		/*
		by GASShooter
			If we don't have an override level, use the ability level
		和訳
			override level がない場合、アビリティレベルを使用します。
		*/
		if (OverrideGameplayLevel == INDEX_NONE)
		{
			//OverrideGameplayLevel = OwningASC->GetDefaultAbilityLevel();
			OverrideGameplayLevel = GetAbilityLevel();
		}

		/*
		by GASShooter
			Build GameplayEffectSpecs for each applied effect
		和訳
			適用するエフェクト毎に GameplayEffectSpecs を構築します。
		*/
		for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffectClasses)
		{
			ReturnSpec.TargetGameplayEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
		}
	}
	return ReturnSpec;
}
