// (C) Sentya Anko 2021


#include "Characters/AttributeSets/GHOAttributeSetBase.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Characters/GHOCharacterBase.h"
#include "Player/GHOPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Settings/GHOGameplayTags.h"


UGHOAttributeSetBase::UGHOAttributeSetBase()
{
	// Cache tags
	HeadShotTag = TAG_Effect_Damage_HeadShot;
}

void UGHOAttributeSetBase::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, HealthMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, ManaMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, ManaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, StaminaMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, ShieldMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, ShieldRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, CharacterLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, XP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, XPBounty, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, Gold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, GoldBounty, COND_None, REPNOTIFY_Always);
}

void UGHOAttributeSetBase::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		PostGameplayEffectExecute_Damage(Data);
	}// Damage
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		/*
		by GASDocumentation
			Handle other health changes.
			Health loss should go through Damage.
		??????
			?????? Health ?????????????????????
			Health ???????????? Damage ???????????????????????????????????????
		*/
		/*
		??????
			??????????????????????????????????????????????????????????????????????????????
			????????????????????????????????? Health ??? HealthRegen ???????????????????????????????????? HealthMax ???????????????????????????????????????
		*/
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetHealthMax()));
	} // Health
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		/*
		by GASDocumentation
			Handle mana changes.
		??????
			Mana ?????????????????????
		*/
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetManaMax()));
	} // Mana
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		/*
		by GASDocumentation
			Handle stamina changes.
		??????
			Stamina ?????????????????????
		*/
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetStaminaMax()));
	}
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		/*
		by GASDocumentation
			Handle shield changes.
		??????
			Shield ?????????????????????
		*/
		//UE_LOG(LogTemp, Log, TEXT("%s Shield:Set Shield before;%f / %f"), *FString(__FUNCTION__), GetShield(), GetShieldMax());
		SetShield(FMath::Clamp(GetShield(), 0.0f, GetShieldMax()));
		//UE_LOG(LogTemp, Log, TEXT("%s Shield:Set Shield after;%f / %f"), *FString(__FUNCTION__), GetShield(), GetShieldMax());
	}
}

void UGHOAttributeSetBase::PostGameplayEffectExecute_Damage(const struct FGameplayEffectModCallbackData& Data)
{
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	//const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	//FGameplayTagContainer SpecAssetTags;
	//Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	/*
	by GASDocumentation
		Get the Target actor, which should be our owner
	??????
		owner ??????????????????????????????????????????????????????????????????
	*/
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AGHOCharacterBase* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AGHOCharacterBase>(TargetActor);
	}

	/*
	by GASDocumentation
		Get the Source actor
	??????
		??????????????????????????????????????????
	*/
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	AGHOCharacterBase* SourceCharacter = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		/*
		by GASDocumentation
			Use the controller to find the source pawn
		??????
			??????????????????????????????????????????????????????????????????????????????
		*/
		if (SourceController)
		{
			SourceCharacter = Cast<AGHOCharacterBase>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<AGHOCharacterBase>(SourceActor);
		}

		/*
		by GASDocumentation
			Set the causer actor based on context if it's set
		??????
			??????????????????????????????????????????????????????????????????????????????????????????????????????
		*/
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	/*
	by GASDocumentation
		Store a local copy of the amount of damage done and clear the damage attribute
	??????
		????????????????????????????????????????????????????????????????????????????????????????????????????????????
	*/
	const float LocalDamageDone = GetDamage();
	SetDamage(0.f);

	if (LocalDamageDone > 0.0f)
	{
		/*
		by GASDocumentation
			If character was alive before damage is added, handle damage
			This prevents damage being added to dead things and replaying death animations
		??????
			???????????????????????????????????????????????????????????????????????????????????????????????????????????????
			??????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
		*/
		const bool WasAlive = TargetCharacter? TargetCharacter->IsAlive(): true;

		//if (!WasAlive)
		//{
		//	//UE_LOG(LogTemp, Warning, TEXT("%s() %s is NOT alive when receiving damage"), TEXT(__FUNCTION__), *TargetCharacter->GetName());
		//}

		/*
		by GASDocumentation
			Apply the damage to shield first if it exists
		??????
			???????????????????????????????????????????????????????????????????????????
		*/
		const float OldShield = GetShield();
		const float DamageAfterShield = LocalDamageDone - OldShield;
		if (OldShield > 0)
		{
			const float NewShield = OldShield - LocalDamageDone;
			SetShield(FMath::Clamp<float>(NewShield, 0.0f, GetShieldMax()));
		}

		if (DamageAfterShield > 0)
		{
			/*
			by GASDocumentation
				Apply the health change and then clamp it
			??????
				Health ???????????????????????????????????????????????????
			*/
			const float NewHealth = GetHealth() - DamageAfterShield;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetHealthMax()));
		}

		if (TargetCharacter && WasAlive)
		{
			/*
			by GASDocumentation
				Show damage number for the Source player unless it was self damage
			??????
				?????????????????????????????????????????????????????????????????????????????????????????????????????????
			??????
				DamageVolume ??????????????????????????????????????????????????????????????????????????????????????????????????????
			*/
			//if (SourceActor != TargetActor)
			{
				if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(SourceController))
				{
					EGHOFloatingTextType TextType = Data.EffectSpec.DynamicAssetTags.HasTag(HeadShotTag) ? EGHOFloatingTextType::Critical : EGHOFloatingTextType::Damage;
					PC->RPCClientShowDamageText(LocalDamageDone, TextType, TargetCharacter);
				}
			}
			if (!TargetCharacter->IsAlive())
			{
				/*
				by GASDocumentation
					TargetCharacter was alive before this damage and now is not alive, give XP and Gold bounties to Source.
					Don't give bounty to self.
				??????
					TargetCharacter ????????????????????????????????????????????????????????????????????????????????????????????? XP ??? GOLD ????????????????????? Source ???????????????
					?????????????????????????????????????????????
				*/
				if (SourceController != TargetController)
				{
					/*
					by GASDocumentation
						Create a dynamic instant Gameplay Effect to give the bounties
					??????
						????????????????????????????????????????????? instant GameplayEffect ?????????
					*/
					UGameplayEffect* GEBounty = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Bounty")));
					GEBounty->DurationPolicy = EGameplayEffectDurationType::Instant;

					int32 Idx = GEBounty->Modifiers.Num();
					GEBounty->Modifiers.SetNum(Idx + 2);

					FGameplayModifierInfo& InfoXP = GEBounty->Modifiers[Idx];
					InfoXP.ModifierMagnitude = FScalableFloat(GetXPBounty());
					InfoXP.ModifierOp = EGameplayModOp::Additive;
					InfoXP.Attribute = UGHOAttributeSetBase::GetXPAttribute();

					FGameplayModifierInfo& InfoGold = GEBounty->Modifiers[Idx + 1];
					InfoGold.ModifierMagnitude = FScalableFloat(GetGoldBounty());
					InfoGold.ModifierOp = EGameplayModOp::Additive;
					InfoGold.Attribute = UGHOAttributeSetBase::GetGoldAttribute();

					Source->ApplyGameplayEffectToSelf(GEBounty, 1.0f, Source->MakeEffectContext());
				}
			}
		}
	}
}

void UGHOAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	/*
	by GASDocumentation
		This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	??????
		????????????????????????????????????????????????????????????????????? health/mana ????????????????????????????????????????????????????????????????????????????????????????????????????????????
	*/
	Super::PreAttributeChange(Attribute, NewValue);

	/*
	by GASDocumentation
		If a Max value changes, adjust current to keep Current % of Current to Max
		GetHealthMaxAttribute comes from the Macros defined at the top of the header
	??????
		Max ?????????????????????????????? Current ??? Max ???????????? % ?????????????????? Current ??????????????????
		GetHealthMaxAttribute() ????????????????????????????????????????????????????????????????????????
	*/
	if (Attribute == GetHealthMaxAttribute())
	{
		AdjustAttributeForMaxChange(Health, HealthMax, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetManaMaxAttribute())
	{
		AdjustAttributeForMaxChange(Mana, ManaMax, NewValue, GetManaAttribute());
	}
	else if (Attribute == GetStaminaMaxAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, StaminaMax, NewValue, GetStaminaAttribute());
	}
	else if (Attribute == GetShieldMaxAttribute())
	{
		//UE_LOG(LogTemp, Log, TEXT("%s Shield;change shield max(%f)before: %f / %f"), *FString(__FUNCTION__), NewValue, GetShield(), GetShieldMax());
		AdjustAttributeForMaxChange(Shield, ShieldMax, NewValue, GetShieldAttribute());
		//UE_LOG(LogTemp, Log, TEXT("%s Shield;change shield max(%f)after: %f / %f"), *FString(__FUNCTION__), NewValue, GetShield(), GetShieldMax());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		/*
		by GASDocumentation
			Cannot slow less than 150 units/s and cannot boost more than 1000 units/s
		??????
			150 units/s ?????????????????? 1000 units/s ?????????????????????????????????
		*/
		NewValue = FMath::Clamp<float>(NewValue, 150.f, 1000.f);
	}
	/*
	??????
		????????????????????????????????????????????????????????????????????????????????????????????????
			* ?????????????????????????????????????????????????????? PostGameplayEffectExecute ??????????????????????????????????????????????????????????????????
			* ????????????????????????????????????????????????????????????????????????
		???????????????????????????????????? AGHOPlayerState ??????????????????????????????????????????????????????????????????????????????????????????
		[???GASDocumentation?????????4.4.6 PostGameplayEffectExecute()???](https://github.com/tranek/GASDocumentation#concepts-as-postgameplayeffectexecute) [(??????)](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#concepts-as-postgameplayeffectexecute) ????????????
		GASDocumentation ?????????????????????????????????????????????????????????
		GASHandsOn ????????????????????????????????????????????????????????????????????????
			* ???????????????????????????????????????????????????????????????
			* ????????????????????????????????????????????????????????????????????????????????????????????????
				* ????????? Health ???????????????????????? HealthRegen ??????????????????????????? HealthMax ?????????????????????????????????????????????????????????????????????
	*/
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetHealthMax());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetManaMax());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetStaminaMax());
	}
	else if (Attribute == GetShieldAttribute())
	{
		//UE_LOG(LogTemp, Log, TEXT("%s Shield;change shield(%f)before: %f / %f"), *FString(__FUNCTION__), NewValue, GetShield(), GetShieldMax());
		NewValue = FMath::Clamp<float>(NewValue, 0, GetShieldMax());
		//UE_LOG(LogTemp, Log, TEXT("%s Shield;change shield(%f)after: %f / %f"), *FString(__FUNCTION__), NewValue, GetShield(), GetShieldMax());
	}
}

void UGHOAttributeSetBase::InitializeAttributesOnSpawned()
{
	// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
	SetHealth(GetHealthMax());
	SetMana(GetManaMax());
	SetStamina(GetStaminaMax());
	SetShield(GetShieldMax());
	//UE_LOG(LogTemp, Log, TEXT("%s Health;%f / %f"), *FString(__FUNCTION__), GetHealth(), GetHealthMax());
	//UE_LOG(LogTemp, Log, TEXT("%s Mana;%f / %f"), *FString(__FUNCTION__), GetMana(), GetManaMax());
	//UE_LOG(LogTemp, Log, TEXT("%s Stamina;%f / %f"), *FString(__FUNCTION__), GetStamina(), GetStaminaMax());
	//UE_LOG(LogTemp, Log, TEXT("%s Shield;%f / %f"), *FString(__FUNCTION__), GetShield(), GetShieldMax());
}

bool UGHOAttributeSetBase::IsAlive()const
{
	return GetHealth() > 0.f;
}

FGHOHUDWidgetParameter UGHOAttributeSetBase::GetHUDParameter()const
{
	return FGHOHUDWidgetParameter
	(
		GetCharacterLevel(),
		GetXP(),
		GetGold(),
		GetMoveSpeed(),
		GetHealth(),
		GetHealthMax(),
		GetHealthRegenRate(),
		GetMana(),
		GetManaMax(),
		GetManaRegenRate(),
		GetStamina(),
		GetStaminaMax(),
		GetStaminaRegenRate(),
		GetShield(),
		GetShieldMax(),
		GetShieldRegenRate()
	);
}

void UGHOAttributeSetBase::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();

		/*
		??????
			Current ??? 0 ????????????????????????????????????
			GASDocumentation ?????? Current / Max ????????? 0 ???????????? Current ??? Max ????????????????????????????????????
			????????? PreAttributeChange() ??????????????????????????????????????????
			?????????????????????????????????????????????????????????????????? Current ??? Max ???????????????????????? Max ???????????????????????????????????? Current ??? Max ??????????????????????????????
			GASHandsOn ?????????????????????????????????????????????????????????
				* PreAttributeChange() ???????????????????????????????????? Current ??? Max ??????????????????????????????????????????
				* AdjustAttributeForMaxChange() ??? Current ??? 0 ???????????? Current ?????????????????????????????????????????????
				* AICharacter ??? Current ?????????????????????????????????????????????????????? InitializeAttributesOnSpawned() ??????????????????????????????

		*/
		if (CurrentValue != 0.f)
		{
			float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;
			AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
		}
	}
}

void UGHOAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Health, OldHealth);
}

void UGHOAttributeSetBase::OnRep_HealthMax(const FGameplayAttributeData& OldHealthMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, HealthMax, OldHealthMax);
}

void UGHOAttributeSetBase::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, HealthRegenRate, OldHealthRegenRate);
}

void UGHOAttributeSetBase::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Mana, OldMana);
}

void UGHOAttributeSetBase::OnRep_ManaMax(const FGameplayAttributeData& OldManaMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, ManaMax, OldManaMax);
}

void UGHOAttributeSetBase::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, ManaRegenRate, OldManaRegenRate);
}

void UGHOAttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Stamina, OldStamina);
}

void UGHOAttributeSetBase::OnRep_StaminaMax(const FGameplayAttributeData& OldStaminaMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, StaminaMax, OldStaminaMax);
}

void UGHOAttributeSetBase::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, StaminaRegenRate, OldStaminaRegenRate);
}

void UGHOAttributeSetBase::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Shield, OldShield);
}

void UGHOAttributeSetBase::OnRep_ShieldMax(const FGameplayAttributeData& OldShieldMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, ShieldMax, OldShieldMax);
}

void UGHOAttributeSetBase::OnRep_ShieldRegenRate(const FGameplayAttributeData& OldShieldRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, ShieldRegenRate, OldShieldRegenRate);
}

void UGHOAttributeSetBase::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Armor, OldArmor);
}

void UGHOAttributeSetBase::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, MoveSpeed, OldMoveSpeed);
}

void UGHOAttributeSetBase::OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, CharacterLevel, OldCharacterLevel);
}

void UGHOAttributeSetBase::OnRep_XP(const FGameplayAttributeData& OldXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, XP, OldXP);
}

void UGHOAttributeSetBase::OnRep_XPBounty(const FGameplayAttributeData& OldXPBounty)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, XPBounty, OldXPBounty);
}

void UGHOAttributeSetBase::OnRep_Gold(const FGameplayAttributeData& OldGold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Gold, OldGold);
}

void UGHOAttributeSetBase::OnRep_GoldBounty(const FGameplayAttributeData& OldGoldBounty)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, GoldBounty, OldGoldBounty);
}
