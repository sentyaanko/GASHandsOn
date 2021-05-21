// (C) Sentya Anko 2021


#include "Characters/Abilities/GHOGA_FireGun.h"
#include "AbilitySystemComponent.h"
//#include "Kismet/KismetMathLibrary.h"
#include "Characters/Abilities/AbilityTasks/GHOAbilityTask_PlayMontageAndWaitForEvent.h"
#include "Characters/GHOCharacterBase.h"

UGHOGA_FireGun::UGHOGA_FireGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTag Ability1Tag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Ability1"));
	AbilityTags.AddTag(Ability1Tag);
	ActivationOwnedTags.AddTag(Ability1Tag);

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));

	Range = 1000.0f;
	Damage = 12.0f;
}

void UGHOGA_FireGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData * TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	UAnimMontage* MontageToPlay = FireHipMontage;

	if (GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.AimDownSights"))) &&
		!GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.AimDownSights.Removal"))))
	{
		MontageToPlay = FireIronsightsMontage;
	}

	/*
	by GASDocumentation
		Play fire montage and wait for event telling us to spawn the projectile
	和訳
		発砲のモンタージュを再生し、投射物のスポーンを知らせるイベントを待つ。
	*/
	UGHOAbilityTask_PlayMontageAndWaitForEvent* Task = UGHOAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UGHOGA_FireGun::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UGHOGA_FireGun::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGHOGA_FireGun::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UGHOGA_FireGun::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UGHOGA_FireGun::EventReceived);
	/*
	by GASDocumentation
		ReadyForActivation() is how you activate the AbilityTask in C++. 
		Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	和訳
		ReadyForActivation() は C++ で AbilityTask を起動する方法です。
		Blueprint には ReadyForActivation() を自動的に呼び出す K2Node_LatentGameplayTaskCall の魔術があります。
	*/
	Task->ReadyForActivation();
}

void UGHOGA_FireGun::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGHOGA_FireGun::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGHOGA_FireGun::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// Montage told us to end the ability before the montage finished playing.
	// Montage was set to continue playing animation even after ability ends so this is okay.
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility")))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	/*
	by GASDocumentation
		Only spawn projectiles on the Server.
		Predicting projectiles is an advanced topic not covered in this example.
	和訳
		投射物はサーバーでのみ生成されます。
		投射物の Predicting （予測）についてはこの example では説明できない高度なトピックです。
	*/
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.SpawnProjectile")))
	{
#if 0
		AGHOHeroCharacter* Hero = Cast<AGHOHeroCharacter>(GetAvatarActorFromActorInfo());
		if (!Hero)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		FVector Start = Hero->GetGunComponent()->GetSocketLocation(FName("Muzzle"));
		FVector End = Hero->GetCameraBoom()->GetComponentLocation() + Hero->GetFollowCamera()->GetForwardVector() * Range;
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

		FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());

		// Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
		DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage);

		FTransform MuzzleTransform = Hero->GetGunComponent()->GetSocketTransform(FName("Muzzle"));
		MuzzleTransform.SetRotation(Rotation.Quaternion());
		MuzzleTransform.SetScale3D(FVector(1.0f));

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AGHOProjectile* Projectile = GetWorld()->SpawnActorDeferred<AGHOProjectile>(ProjectileClass, MuzzleTransform, GetOwningActorFromActorInfo(),
			Hero, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Projectile->DamageEffectSpecHandle = DamageEffectSpecHandle;
		Projectile->Range = Range;
		Projectile->FinishSpawning(MuzzleTransform);
#else
		AGHOCharacterBase* Character = Cast<AGHOCharacterBase>(GetAvatarActorFromActorInfo());
		if (!Character)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());

		/*
		by GASDocumentation
			Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
		和訳
			GameplayEffectSpec の SetByCaller の値を介して Damage Execution Calculation にダメージを渡す。
		*/
		DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage);

		const FTransform MuzzleTransform = Character->GetProjectileTransform(Range);

		AGHOProjectile* Projectile = GetWorld()->SpawnActorDeferred<AGHOProjectile>(ProjectileClass, MuzzleTransform, GetOwningActorFromActorInfo(),
			Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Projectile->SetDamageEffectSpecHandle( DamageEffectSpecHandle );
		Projectile->SetRange( Range );
		Projectile->FinishSpawning(MuzzleTransform);
#endif
	}
}
