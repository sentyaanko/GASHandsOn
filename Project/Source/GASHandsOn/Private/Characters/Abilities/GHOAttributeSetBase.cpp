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
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, HealthRegenRate, COND_None, REPNOTIFY_Always);
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
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	} // Health
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
		GetMaxHealthAttribute comes from the Macros defined at the top of the header
	和訳
		Max 値が変更された場合、 Current の Max に対する % を保つように Current を調整する。
		GetMaxHealthAttribute() はヘッダの先頭で定義されたマクロから来ています。
	*/
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
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
	SetHealth(GetMaxHealth());
}

bool UGHOAttributeSetBase::IsAlive()const
{
	return GetHealth() > 0.f;
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

void UGHOAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, MaxHealth, OldMaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldMaxHealth.GetCurrentValue(), MaxHealth.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, HealthRegenRate, OldHealthRegenRate);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldHealthRegenRate.GetCurrentValue(), HealthRegenRate.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, MoveSpeed, OldMoveSpeed);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldMoveSpeed.GetCurrentValue(), MoveSpeed.GetCurrentValue());
}

