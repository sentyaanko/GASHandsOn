// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2020 Dan Kestranek.
// (C) Sentya Anko 2021
/*
解説
	このファイルは Epic によるサンプル ActionRPG に同梱されているソースの複製です。
		https://www.unrealengine.com/marketplace/ja/product/action-rpg
		Source/ActionRPG/Private/Abilities/RPGAbilityTask_PlayMontageAndWaitForEvent.cpp
		Source/ActionRPG/Public/Abilities/RPGAbilityTask_PlayMontageAndWaitForEvent.h
*/


#include "Characters/Abilities/AbilityTasks/GHOAbilityTask_PlayMontageAndWaitForEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "Characters/Abilities/GHOAbilitySystemComponent.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "GameFramework/Character.h"

UGHOAbilityTask_PlayMontageAndWaitForEvent::UGHOAbilityTask_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

UGHOAbilitySystemComponent* UGHOAbilityTask_PlayMontageAndWaitForEvent::GetTargetASC()
{
	return Cast<UGHOAbilitySystemComponent>(AbilitySystemComponent);
}

void UGHOAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			AbilitySystemComponent->ClearAnimatingAbility(Ability);

			/*
			by Epic
				Reset AnimRootMotionTranslationScale
			和訳
				AnimRootMotionTranslationScale をリセットする
			*/
			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character && (Character->GetLocalRole() == ROLE_Authority ||
				(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				Character->SetAnimRootMotionTranslationScale(1.f);
			}

		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UGHOAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
	/*
	by Epic
		TODO: Merge this fix back to engine, it was calling the wrong callback
	和訳
		TODO: 誤ったコールバックを呼び出していたため、この修正をエンジンにマージしました。
	*/

	if (StopPlayingMontage())
	{
		/*
		by Epic
			Let the BP handle the interrupt as well
		和訳
			割り込みも BP に任せましょう
		*/
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void UGHOAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	EndTask();
}

void UGHOAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

UGHOAbilityTask_PlayMontageAndWaitForEvent* UGHOAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(UGameplayAbility* OwningAbility,
	FName TaskInstanceName, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate, FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UGHOAbilityTask_PlayMontageAndWaitForEvent* MyObj = NewAbilityTask<UGHOAbilityTask_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->EventTags = EventTags;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return MyObj;
}

void UGHOAbilityTask_PlayMontageAndWaitForEvent::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;
	UGHOAbilitySystemComponent* GHOAbilitySystemComponent = GetTargetASC();

	if (GHOAbilitySystemComponent)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			/*
			by Epic
				Bind to event callback
			和訳
				イベントコールバックにバインドする
			*/
			EventHandle = GHOAbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UGHOAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent));

			if (GHOAbilitySystemComponent->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
			{
				/*
				by Epic
					Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				和訳
					モンタージュを再生すると、ゲームコードにコールバックが発生し、このアビリティがキルされる可能性があります！キルが保留されている場合は早めに確認してください。
				*/
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UGHOAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &UGHOAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UGHOAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
					(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UGHOAbilityTask_PlayMontageAndWaitForEvent call to PlayMontage failed!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGHOAbilityTask_PlayMontageAndWaitForEvent called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGHOAbilityTask_PlayMontageAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			//ABILITY_LOG(Display, TEXT("%s: OnCancelled"), *GetName());
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UGHOAbilityTask_PlayMontageAndWaitForEvent::ExternalCancel()
{
	check(AbilitySystemComponent);

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UGHOAbilityTask_PlayMontageAndWaitForEvent::OnDestroy(bool AbilityEnded)
{
	/*
	by Epic
		Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
		(If we are destroyed, it will detect this and not do anything)

		This delegate, however, should be cleared as it is a multicast
	和訳
		Note: MontageEnd デリゲートのクリアは、マルチキャストではないので必要なく、次のモンタージュが再生されるとクリアされます。
		(私達が破棄すると、これを検知して何もしません。)

		しかしながら、このデリゲートはマルチキャストであるため、クリアする必要があります。
	*/
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	UGHOAbilitySystemComponent* GHOAbilitySystemComponent = GetTargetASC();
	if (GHOAbilitySystemComponent)
	{
		GHOAbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);

}

bool UGHOAbilityTask_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo)
	{
		return false;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	/*
	by Epic
		Check if the montage is still playing
		The ability would have been interrupted, in which case we should automatically stop the montage
	和訳
		モンタージュがまだ再生されているか確認。
		アビリティが中断された場合、モンタージュを自動的に停止する必要がある。
	*/
	if (AbilitySystemComponent && Ability)
	{
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability
			&& AbilitySystemComponent->GetCurrentMontage() == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}

	return false;
}

FString UGHOAbilityTask_PlayMontageAndWaitForEvent::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}



