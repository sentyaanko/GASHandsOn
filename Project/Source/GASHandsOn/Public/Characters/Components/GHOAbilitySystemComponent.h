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

	bool IsKnockedDown() const;

	void Down(const class UGameplayEffect* GameplayEffect);

	bool IsStun() const;
	void CancelAbilitiesByStun();

	bool IsInteracting() const;

	void TryActivateRevive();
	void CancelRevive();

	/*
	by GASDocumentation
		Called from GHODamageExecCalculation. Broadcasts on ReceivedDamage whenever this ASC receives damage.
	和訳
		GHODamageExecCalculation から呼び出されます。この ASC がダメージを受けるたびに ReceivedDamage でブロードキャストします。
	*/ 
	void ReceiveDamage(UGHOAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);

	FReceivedDamageDelegate& GetReceivedDamage() { return ReceivedDamage; };

public:
	/*
	解説
		ローカルで GameplayCue を再生するための口金。
		GameplayEffect による GameplayCue を使用する代わりにこちらを使用することで、 GameplayEffect の同期のための RPC が発生しなくなり、帯域に優しくなる。
		同期されないため、利用する際はサウンドやパーティクルなど装飾用のものに制限すること。
		詳しくは [「GASDocumentation」の「4.8.3 Local Gameplay Cues （ローカル Gameplay Cues）」](https://github.com/tranek/GASDocumentation/blob/master/README.md#concepts-gc-local) [（和訳）](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#concepts-gc-local) を参照。
	*/
	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

private:
	FGameplayTag DeadTag;
	FGameplayTag KnockedDownTag;
	FGameplayTag StunTag;
	FGameplayTag InteractingTag;
	FGameplayTag ReviveTag;
	FGameplayTag EffectRemoveOnDeathTag;

	bool bCharacterAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;

	FReceivedDamageDelegate ReceivedDamage;
};
