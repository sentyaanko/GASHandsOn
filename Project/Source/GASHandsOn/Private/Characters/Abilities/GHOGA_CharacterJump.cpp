// (C) Sentya Anko 2021


#include "Characters/Abilities/GHOGA_CharacterJump.h"
#include "GameFramework/Character.h"
#include "Settings/GHOGameplayTags.h"


// This code is based on Engine\Plugins\Runtime\GameplayAbilities\Source\GameplayAbilities\Private\Abilities\GameplayAbility_CharacterJump.cpp

UGHOGA_CharacterJump::UGHOGA_CharacterJump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TAG_Ability_Jump)));

	AbilityInputID = EGHOAbilityInputID::Jump;
}

bool UGHOGA_CharacterJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed))
	{
		return Character->CanJump();

	}
	return false;
}

/**
by GASDocumentation
	Canceling an non instanced ability is tricky. 
	Right now this works for Jump since there is nothing that can go wrong by calling StopJumping() if you aren't already jumping. 
	If we had a montage playing non instanced ability, it would need to make sure the Montage that *it* played was still playing, and if so, to cancel it. 
	If this is something we need to support, we may need some light weight data structure to represent 'non intanced abilities in action' with a way to cancel/end them.
和訳
	インスタンスではないアビリティをキャンセルするのは厄介です。
	今のところ、ジャンプについては、すでにジャンプしていない場合にStopJumping()を呼び出しても何の問題もないので、これは有効です。
	もしインスタンス化されていないアビリティを再生するモンタージュがあった場合、 **それ** が再生したモンタージュがまだ再生されているかどうかを確認し、もしそうであればそれをキャンセルする必要があります。
	もしこれをサポートする必要があるなら、「インスタンス化されていないアビリティの動作」を表す軽量のデータ構造と、それをキャンセル／終了する方法が必要になるかもしれません。
*/
void UGHOGA_CharacterJump::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGHOGA_CharacterJump::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (ACharacter * Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		Character->StopJumping();
	}
}

void UGHOGA_CharacterJump::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UGHOGA_CharacterJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			//TODO:
			// why not return? 
			// We need to change it so that it costs more and see how it behaves when CommitAbility fails.
			// If there is a problem, report it to the author.
			// Normally, the cost is checked by CanActivateAbility(), so being rejected here is a special case.
			//UE_LOG(LogTemp, Error, TEXT("%s() CommitAbility() is falied."), *FString(__FUNCTION__));
		}

		if (ACharacter * Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			//UE_LOG(LogTemp, Error, TEXT("%s() call Jump()."), *FString(__FUNCTION__));
			Character->Jump();
		}
	}
}

