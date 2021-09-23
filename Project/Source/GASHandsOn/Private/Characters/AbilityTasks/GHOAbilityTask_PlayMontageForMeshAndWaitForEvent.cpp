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


#include "Characters/AbilityTasks/GHOAbilityTask_PlayMontageForMeshAndWaitForEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "Characters/Components/GHOAbilitySystemComponent.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "GameFramework/Character.h"

#if 0 //for multiple USkeletalMeshComponents on the AvatarActor
UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

UGHOAbilitySystemComponent* UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::GetTargetASC()
{
	return Cast<UGHOAbilitySystemComponent>(AbilitySystemComponent);
}

void UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
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

void UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::OnAbilityCancelled()
{
	/*
	by Epic
		TODO: Merge this fix back to engine, it was calling the wrong callback
	和訳
		TODO: 誤ったコールバックを呼び出していたため、この修正をエンジンにマージしました。
	*/

	if (StopPlayingMontage(OverrideBlendOutTimeForCancelAbility))
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

void UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
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

void UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;

		EventReceived.Broadcast(EventTag, TempData);
	}
}

UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent* UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::PlayMontageForMeshAndWaitForEvent(UGameplayAbility* OwningAbility,
	FName TaskInstanceName, USkeletalMeshComponent* InMesh, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate, FName StartSection, 
	bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale, bool bReplicateMontage, float OverrideBlendOutTimeForCancelAbility,
	float OverrideBlendOutTimeForStopWhenEndAbility)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent* MyObj = NewAbilityTask<UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent>(OwningAbility, TaskInstanceName);
	MyObj->Mesh = InMesh;
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->EventTags = EventTags;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->bReplicateMontage = bReplicateMontage;
	MyObj->OverrideBlendOutTimeForCancelAbility = OverrideBlendOutTimeForCancelAbility;
	MyObj->OverrideBlendOutTimeForStopWhenEndAbility = OverrideBlendOutTimeForStopWhenEndAbility;

	return MyObj;
}

void UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	if (Mesh == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s invalid Mesh"), *FString(__FUNCTION__));
		return;
	}

	bool bPlayedMontage = false;
	UGHOAbilitySystemComponent* GHOAbilitySystemComponent = GetTargetASC();

	if (GHOAbilitySystemComponent)
	{
		//const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			/*
			by Epic
				Bind to event callback
			和訳
				イベントコールバックにバインドする
			*/
			EventHandle = GHOAbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::OnGameplayEvent));

			if (GHOAbilitySystemComponent->PlayMontageForMesh(Ability, Mesh, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection, bReplicateMontage) > 0.f)
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

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::OnAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::OnMontageEnded);
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
			UE_LOG(LogTemp, Warning, TEXT("UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent call to PlayMontage failed!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			//ABILITY_LOG(Display, TEXT("%s: OnCancelled"), *GetName());
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}

	SetWaitingOnAvatar();
}

void UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::ExternalCancel()
{
	check(AbilitySystemComponent);

	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::OnDestroy(bool AbilityEnded)
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
			StopPlayingMontage(OverrideBlendOutTimeForStopWhenEndAbility);
		}
	}

	UGHOAbilitySystemComponent* GHOAbilitySystemComponent = GetTargetASC();
	if (GHOAbilitySystemComponent)
	{
		GHOAbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);

}

bool UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::StopPlayingMontage(float OverrideBlendOutTime)
{
	if (Mesh == nullptr)
	{
		return false;
	}

	UGHOAbilitySystemComponent* GHOAbilitySystemComponent = GetTargetASC();
	if (!GHOAbilitySystemComponent)
	{
		return false;
	}

	//const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	//if (!ActorInfo)
	//{
	//	return false;
	//}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
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
	if (GHOAbilitySystemComponent && Ability)
	{
		if (GHOAbilitySystemComponent->GetAnimatingAbilityFromAnyMesh() == Ability
			&& GHOAbilitySystemComponent->GetCurrentMontageForMesh(Mesh) == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			GHOAbilitySystemComponent->CurrentMontageStopForMesh(Mesh, OverrideBlendOutTime);
			return true;
		}
	}

	return false;
}

FString UGHOAbilityTask_PlayMontageForMeshAndWaitForEvent::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability && Mesh)
	{
		//const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = Mesh->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageForMeshAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}


#endif
