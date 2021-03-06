// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "UI/GHOHUDWidgetParameter.h"
#include "GHOAttributeSetMacros.h"
#include "GHOAttributeSetBase.generated.h"


/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UGHOAttributeSetBase();

	// UObject interface
public:
	/*
	by Epic
		Returns properties that are replicated for the lifetime of the actor channel
	和訳
		アクターチャンネルのライフタイム中にレプリケーションされるプロパティを返します。
	*/
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;

	// End of UObject interface

	// UAttributeSet interface
public:
	/*
	by Epic
		Called just before a GameplayEffect is executed to modify the base value of an attribute. 
		No more changes can be made.
		Note this is only called during an 'execute'. 
		E.g., a modification to the 'base value' of an attribute. 
		It is not called during an application of a GameplayEffect, such as a 5 ssecond +10 movement speed buff.
	和訳
		アトリビュートの base の値を変更する GameplayEffect が実行される直前に呼び出されます。
		それ以上の変更はできません。
		これは execute の間のみ呼び出されることに注意してください。
		例：アトリビュートの base の値を変更する場合。
		GameplayEffect の適用時には呼び出されません。例えば 5 秒間 +10 の移動速度バフなど。
	*/
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;

	/*
	by Epic
		Called just before any modification happens to an attribute. 
		This is lower level than PreAttributeModify/PostAttribute modify.
		There is no additional context provided here since anything can trigger this. 
		Executed effects, duration based effects, effects being removed, immunity being applied, stacking rules changing, etc.
		This function is meant to enforce things like "Health = Clamp(Health, 0, HealthMax)" 
		and NOT things like "trigger this extra thing if damage is applied, etc".
	
		NewValue is a mutable reference so you are able to clamp the newly applied value as well.
	和訳
		アトリビュートの修正が行われる直前に呼び出されます。
		これは PreAttributeModify/PostAttribute modify よりも低レベルです。
		何が起こってもおかしくないため、ここでは追加のコンテキストは提供されません。
		実行されたエフェクト、持続時間に基づくエフェクト、エフェクトの除去、免疫の適用、スタッキングルールの変更などです。
		この関数は「 Health = Clamp(Health, 0, HealthMax) 」 のようなことを強制するためのものであり、 
		「ダメージが適用された際にこの追加事項をトリガーする」などを行うものではありません。

		NewValue は変更可能な参照なので、新しく適用された値もクランプすることが出来ます。

	*/
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// End of UAttributeSet interface

public:
	/*
	by GASDocumentation
		Current Health, when 0 we expect owner to die unless prevented by an ability. Capped by HealthMax.
		Positive changes can directly use this.
		Negative changes to Health should go through Damage meta attribute.
	和訳
		現在の Health で、0 のときは、アビリティで防がれない限り、オーナーは死ぬと思われる。 HealthMax が上限です。
		ポジティブな変更は直接使用することが出来ます。
		ネガティブな変更は Damage メタ Attribute を使用するべきです。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, Health)

	/*
	by GASDocumentation
		HealthMax is its own attribute since GameplayEffects may modify it
	和訳
		HealthMax は GameplayEffects が変更する可能性があるため、独自のアトリビュートです。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Health", ReplicatedUsing = OnRep_HealthMax)
	FGameplayAttributeData HealthMax;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, HealthMax)

	/*
	by GASDocumentation
		Health regen rate will passively increase Health every second
	和訳
		HealthRegenRate は毎秒パッシブに Health を増加します。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Health", ReplicatedUsing = OnRep_HealthRegenRate)
	FGameplayAttributeData HealthRegenRate;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, HealthRegenRate)

	/*
	by GASDocumentation
		Current Mana, used to execute special abilities. Capped by ManaMax.
	和訳
		現在の Mana で、特別なアビリティを実行する際に使用します。 ManaMax が上限です。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Mana", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, Mana)

	/*
	by GASDocumentation
		ManaMax is its own attribute since GameplayEffects may modify it
	和訳
		ManaMax は GameplayEffects が変更する可能性があるため、独自のアトリビュートです。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Mana", ReplicatedUsing = OnRep_ManaMax)
	FGameplayAttributeData ManaMax;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, ManaMax)

	/*
	by GASDocumentation
		Mana regen rate will passively increase Mana every second
	和訳
		ManaRegenRate は毎秒パッシブに Mana を増加します。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Mana", ReplicatedUsing = OnRep_ManaRegenRate)
	FGameplayAttributeData ManaRegenRate;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, ManaRegenRate)

	/*
	by GASDocumentation
		Current stamina, used to execute special abilities. Capped by StaminaMax.
	和訳
		現在の Stamina で、特別なアビリティを実行する際に使用します。 StaminaMax が上限です。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Stamina", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, Stamina)

	/*
	by GASDocumentation
		StaminaMax is its own attribute since GameplayEffects may modify it
	和訳
		StaminaMax は GameplayEffects が変更する可能性があるため、独自のアトリビュートです。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Stamina", ReplicatedUsing = OnRep_StaminaMax)
	FGameplayAttributeData StaminaMax;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, StaminaMax)

	/*
	by GASDocumentation
		Stamina regen rate will passively increase Stamina every second
	和訳
		StaminaRegenRate は毎秒パッシブに Stamina を増加します。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Stamina", ReplicatedUsing = OnRep_StaminaRegenRate)
	FGameplayAttributeData StaminaRegenRate;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, StaminaRegenRate)

	/*
	by GASDocumentation
		Current shield acts like temporary health. 
		When depleted, damage will drain regular health.
	和訳
		現在のシールド値で、一時的な Health のような働きをする。
		枯渇すると、ダメージによって通常の Health を消耗する。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Shield", ReplicatedUsing = OnRep_Shield)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, Shield)

	/*
	by GASDocumentation
		Maximum shield that we can have.
	和訳
		蓄えられるシールドの最大値。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Shield", ReplicatedUsing = OnRep_ShieldMax)
	FGameplayAttributeData ShieldMax;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, ShieldMax)

	/*
	by GASDocumentation
		Shield regen rate will passively increase Shield every second
	和訳
		ShieldRegenRate は毎秒パッシブに Shield を増加します。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Shield", ReplicatedUsing = OnRep_ShieldRegenRate)
	FGameplayAttributeData ShieldRegenRate;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, ShieldRegenRate)

	/*
	by GASDocumentation
		Armor reduces the amount of damage done by attackers
	和訳
		Armor は攻撃者から受けるダメージを軽減する。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Armor", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, Armor)

	/*
	by GASDocumentation
		Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
		Temporary value that only exists on the Server. Not replicated.
	和訳
		Damage は DamageExecution が最終的なダメージを計算するために使用するメタ・アトリビュートで、それが -Health に変わります。
		サーバーのみに存在する一時的な値です。レプリケーションされません。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, Damage)

	/*
	by GASDocumentation
		MoveSpeed affects how fast characters can move.
	和訳
		MoveSpeed は キャラクターの移動速度に影響を与えます。
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, MoveSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Character Level", ReplicatedUsing = OnRep_CharacterLevel)
	FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, CharacterLevel)

	/*
	by GASDocumentation
		Experience points gained from killing enemies. Used to level up (not implemented in this project).
	和訳
		経験値。敵を倒すと得られる。レベルアップに使用します。（本プロジェクトでは未実装）
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|XP", ReplicatedUsing = OnRep_XP)
	FGameplayAttributeData XP;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, XP)

	/*
	by GASDocumentation
		Experience points awarded to the character's killers. Used to level up (not implemented in this project).
	和訳
		キャラクターを倒したものに与えられる経験値。レベルアップに使用します。（本プロジェクトでは未実装）
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|XP", ReplicatedUsing = OnRep_XPBounty)
	FGameplayAttributeData XPBounty;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, XPBounty)

	/*
	by GASDocumentation
		Gold gained from killing enemies. Used to purchase items (not implemented in this project).
	和訳
		ゴールド。敵を倒すと得られる。アイテムの購入に使用します。（本プロジェクトでは未実装）
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Gold", ReplicatedUsing = OnRep_Gold)
	FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, Gold)

	/*
	by GASDocumentation
		Gold awarded to the character's killer. Used to purchase items (not implemented in this project).
	和訳
		キャラクターを倒したものに与えられるゴールド。アイテムの購入に使用します。（本プロジェクトでは未実装）
	*/
	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|Gold", ReplicatedUsing = OnRep_GoldBounty)
	FGameplayAttributeData GoldBounty;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetBase, GoldBounty)

public:
	void InitializeAttributesOnSpawned();

	bool IsAlive()const;

	FGHOHUDWidgetParameter GetHUDParameter()const;

protected:
	/*
	by GASDocumentation
		Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
		(i.e. When HealthMax increases, Health increases by an amount that maintains the same percentage as before)
	和訳
		関連する最大属性が変更されたときに、属性の値を比例的に調整するヘルパー関数です。
		（例： HealthMax が増加すると、 Health は以前と同じ割合を維持する量だけ増加します。）
	*/
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

private:
	/*
	by GASDocumentation
		These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
	和訳
		これらの OnRep 関数はレプリケーションの際に、アビリティシステムの内部表現が適切に同期されるようにするために存在します。
	*/

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_HealthMax(const FGameplayAttributeData& OldHealthMax);

	UFUNCTION()
	void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana);

	UFUNCTION()
	void OnRep_ManaMax(const FGameplayAttributeData& OldManaMax);

	UFUNCTION()
	void OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate);

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
	void OnRep_StaminaMax(const FGameplayAttributeData& OldStaminaMax);

	UFUNCTION()
	void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate);

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield);

	UFUNCTION()
	void OnRep_ShieldMax(const FGameplayAttributeData& OldShieldMax);

	UFUNCTION()
	void OnRep_ShieldRegenRate(const FGameplayAttributeData& OldShieldRegenRate);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);

	UFUNCTION()
	void OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel);

	UFUNCTION()
	void OnRep_XP(const FGameplayAttributeData& OldXP);

	UFUNCTION()
	void OnRep_XPBounty(const FGameplayAttributeData& OldXPBounty);

	UFUNCTION()
	void OnRep_Gold(const FGameplayAttributeData& OldGold);

	UFUNCTION()
	void OnRep_GoldBounty(const FGameplayAttributeData& OldGoldBounty);

	void PostGameplayEffectExecute_Damage(const struct FGameplayEffectModCallbackData& Data);

private:
	FGameplayTag HeadShotTag;
};
