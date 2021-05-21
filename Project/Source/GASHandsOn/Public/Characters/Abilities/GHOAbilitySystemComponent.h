// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GHOAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UGHOAbilitySystemComponent*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UGHOAbilitySystemComponent();

public:
	void InitializeAttributes(class AGHOCharacterBase* InSourceObject);
	void AddCharacterAbilities(class AGHOCharacterBase* InSourceObject);
	void RemoveCharacterAbilities(class AGHOCharacterBase* InSourceObject);
	void AddStartupEffects(class AGHOCharacterBase* InSourceObject);

	void ClearDead();
	bool IsDead() const;
	void Die();

	bool IsStun() const;

	/*
	by GASDocumentation
		Called from GHODamageExecCalculation. Broadcasts on ReceivedDamage whenever this ASC receives damage.
	和訳
		GHODamageExecCalculation から呼び出されます。この ASC がダメージを受けるたびに ReceivedDamage でブロードキャストします。
	*/ 
	void ReceiveDamage(UGHOAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);

	FReceivedDamageDelegate& GetReceivedDamage() { return ReceivedDamage; };

private:
	FGameplayTag DeadTag;
	FGameplayTag StunTag;
	FGameplayTag EffectRemoveOnDeathTag;

	bool bCharacterAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;

	FReceivedDamageDelegate ReceivedDamage;
};
