// (C) Sentya Anko 2021


#include "Characters/AttributeSets/GHOAttributeSetAmmo.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "Settings/GHOGameplayTags.h"

UGHOAttributeSetAmmo::UGHOAttributeSetAmmo()
{
	//RifleAmmoTag = FGameplayTag::RequestGameplayTag(FName(TAG_Weapon_Ammo_Rifle));
	//RocketAmmoTag = FGameplayTag::RequestGameplayTag(FName(TAG_Weapon_Ammo_Rocket));
	//ShotgunAmmoTag = FGameplayTag::RequestGameplayTag(FName(TAG_Weapon_Ammo_Shotgun));
}

void UGHOAttributeSetAmmo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetAmmo, RifleReserveAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetAmmo, MaxRifleReserveAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetAmmo, RocketReserveAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetAmmo, MaxRocketReserveAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetAmmo, ShotgunReserveAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetAmmo, MaxShotgunReserveAmmo, COND_None, REPNOTIFY_Always);
}

void UGHOAttributeSetAmmo::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UGHOAttributeSetAmmo::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetRifleReserveAmmoAttribute())
	{
		float Ammo = GetRifleReserveAmmo();
		SetRifleReserveAmmo(FMath::Clamp<float>(Ammo, 0, GetMaxRifleReserveAmmo()));
	}
	else if (Data.EvaluatedData.Attribute == GetRocketReserveAmmoAttribute())
	{
		float Ammo = GetRocketReserveAmmo();
		SetRocketReserveAmmo(FMath::Clamp<float>(Ammo, 0, GetMaxRocketReserveAmmo()));
	}
	else if (Data.EvaluatedData.Attribute == GetShotgunReserveAmmoAttribute())
	{
		float Ammo = GetShotgunReserveAmmo();
		SetShotgunReserveAmmo(FMath::Clamp<float>(Ammo, 0, GetMaxShotgunReserveAmmo()));
	}
}

FGameplayAttribute UGHOAttributeSetAmmo::GetReserveAmmoAttributeFromTag(const FGameplayTag& PrimaryAmmoTag)
{
	static FGameplayTag RifleAmmoTag = FGameplayTag::RequestGameplayTag(FName(TAG_Weapon_Ammo_Rifle));
	static FGameplayTag RocketAmmoTag = FGameplayTag::RequestGameplayTag(FName(TAG_Weapon_Ammo_Rocket));
	static FGameplayTag ShotgunAmmoTag = FGameplayTag::RequestGameplayTag(FName(TAG_Weapon_Ammo_Shotgun));
	if (PrimaryAmmoTag == RifleAmmoTag)
	{
		return GetRifleReserveAmmoAttribute();
	}
	else if (PrimaryAmmoTag == RocketAmmoTag)
	{
		return GetRocketReserveAmmoAttribute();
	}
	else if (PrimaryAmmoTag == ShotgunAmmoTag)
	{
		return GetShotgunReserveAmmoAttribute();
	}

	return FGameplayAttribute();
}

FGameplayAttribute UGHOAttributeSetAmmo::GetMaxReserveAmmoAttributeFromTag(const FGameplayTag& PrimaryAmmoTag)
{
	static FGameplayTag RifleAmmoTag = FGameplayTag::RequestGameplayTag(FName(TAG_Weapon_Ammo_Rifle));
	static FGameplayTag RocketAmmoTag = FGameplayTag::RequestGameplayTag(FName(TAG_Weapon_Ammo_Rocket));
	static FGameplayTag ShotgunAmmoTag = FGameplayTag::RequestGameplayTag(FName(TAG_Weapon_Ammo_Shotgun));
	if (PrimaryAmmoTag == RifleAmmoTag)
	{
		return GetMaxRifleReserveAmmoAttribute();
	}
	else if (PrimaryAmmoTag == RocketAmmoTag)
	{
		return GetMaxRocketReserveAmmoAttribute();
	}
	else if (PrimaryAmmoTag == ShotgunAmmoTag)
	{
		return GetMaxShotgunReserveAmmoAttribute();
	}

	return FGameplayAttribute();
}

//void UGHOAttributeSetAmmo::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
//{
//	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
//	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
//	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
//	{
//		// Change current value to maintain the current Val / Max percent
//		const float CurrentValue = AffectedAttribute.GetCurrentValue();
//		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;
//
//		/*
//		解説
//			TODO: UGHOAttributeSetBase と実装方法が異なる。あちらと合わせること。
//		*/
//		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
//	}
//}

void UGHOAttributeSetAmmo::OnRep_RifleReserveAmmo(const FGameplayAttributeData& OldRifleReserveAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetAmmo, RifleReserveAmmo, OldRifleReserveAmmo);
}

void UGHOAttributeSetAmmo::OnRep_MaxRifleReserveAmmo(const FGameplayAttributeData& OldMaxRifleReserveAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetAmmo, MaxRifleReserveAmmo, OldMaxRifleReserveAmmo);
}

void UGHOAttributeSetAmmo::OnRep_RocketReserveAmmo(const FGameplayAttributeData& OldRocketReserveAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetAmmo, RocketReserveAmmo, OldRocketReserveAmmo);
}

void UGHOAttributeSetAmmo::OnRep_MaxRocketReserveAmmo(const FGameplayAttributeData& OldMaxRocketReserveAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetAmmo, MaxRocketReserveAmmo, OldMaxRocketReserveAmmo);
}

void UGHOAttributeSetAmmo::OnRep_ShotgunReserveAmmo(const FGameplayAttributeData& OldShotgunReserveAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetAmmo, ShotgunReserveAmmo, OldShotgunReserveAmmo);
}

void UGHOAttributeSetAmmo::OnRep_MaxShotgunReserveAmmo(const FGameplayAttributeData& OldMaxShotgunReserveAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetAmmo, MaxShotgunReserveAmmo, OldMaxShotgunReserveAmmo);
}
