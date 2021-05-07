// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GHOAbilitySystemComponent.generated.h"

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

private:
	FGameplayTag DeadTag;
	FGameplayTag StunTag;
	FGameplayTag EffectRemoveOnDeathTag;

	bool bCharacterAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;
};
