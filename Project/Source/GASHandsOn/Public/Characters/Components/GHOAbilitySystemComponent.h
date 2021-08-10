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

	// UAbilitySystemComponent interface
public:
	/** Called to handle ability bind input */
	virtual void AbilityLocalInputPressed(int32 InputID)override;

	// End of UAbilitySystemComponent interface

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

	/**
	by GASShooter
		If this ASC has a valid prediction key, attempt to predictively apply this GE. 
		Used in Pre/PostInteract() on the Interacter's ASC.
		If the key is not valid, it will apply the GE without prediction.
	和訳
		この ASC が有効な prediction key （予測キー）を持っている場合、この GE を predictively （予測的）に適用を試みます。
		Interacter の ASC の Pre/PostInteract() で使用されます。
		有効なキーが存在しない場合、 prediction （予測）なしで GE を適用します。
	解説
		この関数の定義/実装方法は UAbilitySystemComponent::BP_ApplyGameplayEffectToSelf() のコピーをもとにしています。
	*/
	UFUNCTION(BlueprintCallable, Category = "GameplayEffects", Meta = (DisplayName = "ApplyGameplayEffectToSelfWithPrediction"))
	FActiveGameplayEffectHandle BP_ApplyGameplayEffectToSelfWithPrediction(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, FGameplayEffectContextHandle EffectContext);

	/**
	by GASShooter
		If this ASC has a valid prediction key, attempt to predictively apply this GE to the target. 
		Used in Pre/PostInteract() on the Interacter's ASC.
		If the key is not valid, it will apply the GE to the target without prediction.
	和訳
		この ASC が有効な prediction key （予測キー）を持っている場合、この GE をターゲットに predictively （予測的）に適用を試みます。
		Interacter の ASC の Pre/PostInteract() で使用されます。
		有効なキーが存在しない場合、 prediction （予測）なしで GE をターゲットに適用します。
	解説
		この関数の定義/実装方法は UAbilitySystemComponent::BP_ApplyGameplayEffectToTarget() のコピーをもとにしています。
	*/
	UFUNCTION(BlueprintCallable, Category = "GameplayEffects", Meta = (DisplayName = "ApplyGameplayEffectToTargetWithPrediction"))
	FActiveGameplayEffectHandle BP_ApplyGameplayEffectToTargetWithPrediction(TSubclassOf<UGameplayEffect> GameplayEffectClass, UAbilitySystemComponent* Target, float Level, FGameplayEffectContextHandle Context);

	/*
	by GASShooter
		Exposes GetTagCount to Blueprint
	和訳
		Blueprint に GetTagCount を公開
	解説
		GASShooter では ScriptName も指定していますが、ドキュメントを見る限り ScriptName は列挙型用のメタデータ指定子なので、外しています。
		//オリジナル
		//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities", Meta = (DisplayName = "GetTagCount", ScriptName = "GetTagCount"))
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities", Meta = (DisplayName = "GetTagCount"))
	int32 K2_GetTagCount(FGameplayTag TagToCheck) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
	bool IsInteractingBeforeRemoval() const;

private:
	FGameplayTag DeadTag;
	FGameplayTag KnockedDownTag;
	FGameplayTag StunTag;
	FGameplayTag InteractingTag;
	FGameplayTag InteractingRemovalTag;
	FGameplayTag ReviveTag;
	FGameplayTag EffectRemoveOnDeathTag;

	bool bCharacterAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;

	FReceivedDamageDelegate ReceivedDamage;
};
