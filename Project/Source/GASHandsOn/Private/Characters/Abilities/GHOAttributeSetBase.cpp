// (C) Sentya Anko 2021


#include "Characters/Abilities/GHOAttributeSetBase.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


UGHOAttributeSetBase::UGHOAttributeSetBase()
{
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
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UGHOAttributeSetBase::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		/*
		by GASDocumentation
			Handle other health changes.
			Health loss should go through Damage.
		和訳
			他の Health の変更を扱う。
			Health の減少は Damage を経由する必要があります。
		*/
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetHealthMax()));
	} // Health
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		/*
		by GASDocumentation
			Handle mana changes.
		和訳
			Mana の変更を扱う。
		*/
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetManaMax()));
	} // Mana
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		/*
		by GASDocumentation
			Handle stamina changes.
		和訳
			Stamina の変更を扱う。
		*/
		// 
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetStaminaMax()));
	}
}

void UGHOAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	/*
	by GASDocumentation
		This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	和訳
		アトリビュートが変更する度に呼び出されるので、 health/mana の最大値が変更される場合は現在の合計値をスケーリングさせて一致させます。
	*/
	Super::PreAttributeChange(Attribute, NewValue);

	/*
	by GASDocumentation
		If a Max value changes, adjust current to keep Current % of Current to Max
		GetHealthMaxAttribute comes from the Macros defined at the top of the header
	和訳
		Max 値が変更された場合、 Current の Max に対する % を保つように Current を調整する。
		GetHealthMaxAttribute() はヘッダの先頭で定義されたマクロから来ています。
	*/
	if (Attribute == GetHealthMaxAttribute())
	{
		AdjustAttributeForMaxChange(Health, HealthMax, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetManaMaxAttribute())
	{
		AdjustAttributeForMaxChange(Mana, ManaMax, NewValue, GetManaAttribute());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		/*
		by GASDocumentation
			Cannot slow less than 150 units/s and cannot boost more than 1000 units/s
		和訳
			150 units/s 以下の減速と 1000 units/s 以上の加速を制限する。
		*/
		NewValue = FMath::Clamp<float>(NewValue, 150.f, 1000.f);
	}
}

void UGHOAttributeSetBase::InitializeAttributesOnSpawned()
{
	// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
	SetHealth(GetHealthMax());
	SetMana(GetManaMax());
	SetStamina(GetStaminaMax());
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
		GetMoveSpeed(),
		GetHealth(),
		GetHealthMax(),
		GetHealthRegenRate(),
		GetMana(),
		GetManaMax(),
		GetManaRegenRate(),
		GetStamina(),
		GetStaminaMax(),
		GetStaminaRegenRate()
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
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UGHOAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Health, OldHealth);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldHealth.GetCurrentValue(), Health.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_HealthMax(const FGameplayAttributeData& OldHealthMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, HealthMax, OldHealthMax);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldHealthMax.GetCurrentValue(), HealthMax.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, HealthRegenRate, OldHealthRegenRate);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldHealthRegenRate.GetCurrentValue(), HealthRegenRate.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Mana, OldMana);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldMana.GetCurrentValue(), Mana.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_ManaMax(const FGameplayAttributeData& OldManaMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, ManaMax, OldManaMax);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldManaMax.GetCurrentValue(), ManaMax.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, ManaRegenRate, OldManaRegenRate);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldManaRegenRate.GetCurrentValue(), ManaRegenRate.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Stamina, OldStamina);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldStamina.GetCurrentValue(), Stamina.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_StaminaMax(const FGameplayAttributeData& OldStaminaMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, StaminaMax, OldStaminaMax);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldStaminaMax.GetCurrentValue(), StaminaMax.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, StaminaRegenRate, OldStaminaRegenRate);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldStaminaRegenRate.GetCurrentValue(), StaminaRegenRate.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, MoveSpeed, OldMoveSpeed);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldMoveSpeed.GetCurrentValue(), MoveSpeed.GetCurrentValue());
}

