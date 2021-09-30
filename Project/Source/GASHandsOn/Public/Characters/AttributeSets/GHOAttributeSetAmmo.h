// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GHOAttributeSetMacros.h"
#include "GHOAttributeSetAmmo.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOAttributeSetAmmo : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UGHOAttributeSetAmmo();

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
	UPROPERTY(BlueprintReadOnly, Category = "Ammo", ReplicatedUsing = OnRep_RifleReserveAmmo)
	FGameplayAttributeData RifleReserveAmmo;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetAmmo, RifleReserveAmmo)

	UPROPERTY(BlueprintReadOnly, Category = "Ammo", ReplicatedUsing = OnRep_MaxRifleReserveAmmo)
	FGameplayAttributeData MaxRifleReserveAmmo;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetAmmo, MaxRifleReserveAmmo)

	UPROPERTY(BlueprintReadOnly, Category = "Ammo", ReplicatedUsing = OnRep_RocketReserveAmmo)
	FGameplayAttributeData RocketReserveAmmo;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetAmmo, RocketReserveAmmo)

	UPROPERTY(BlueprintReadOnly, Category = "Ammo", ReplicatedUsing = OnRep_MaxRocketReserveAmmo)
	FGameplayAttributeData MaxRocketReserveAmmo;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetAmmo, MaxRocketReserveAmmo)

	UPROPERTY(BlueprintReadOnly, Category = "Ammo", ReplicatedUsing = OnRep_ShotgunReserveAmmo)
	FGameplayAttributeData ShotgunReserveAmmo;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetAmmo, ShotgunReserveAmmo)

	UPROPERTY(BlueprintReadOnly, Category = "Ammo", ReplicatedUsing = OnRep_MaxShotgunReserveAmmo)
	FGameplayAttributeData MaxShotgunReserveAmmo;
	ATTRIBUTE_ACCESSORS(UGHOAttributeSetAmmo, MaxShotgunReserveAmmo)

	static FGameplayAttribute GetReserveAmmoAttributeFromTag(const FGameplayTag& PrimaryAmmoTag);
	static FGameplayAttribute GetMaxReserveAmmoAttributeFromTag(const FGameplayTag& PrimaryAmmoTag);

protected:
	///*
	//by GASShooter
	//	Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
	//	(i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before)
	//和訳
	//	関連する最大属性が変更されたときに、属性の値を比例的に調整するヘルパー関数です。
	//	（例： HealthMax が増加すると、 Health は以前と同じ割合を維持する量だけ増加します。）
	//*/
	//void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	/*
	by GASShooter
		These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
	和訳
		これらの OnRep 関数はレプリケーションの際に、アビリティシステムの内部表現が適切に同期されるようにするために存在します。
	*/
	
	UFUNCTION()
	virtual void OnRep_RifleReserveAmmo(const FGameplayAttributeData& OldRifleReserveAmmo);

	UFUNCTION()
	virtual void OnRep_MaxRifleReserveAmmo(const FGameplayAttributeData& OldMaxRifleReserveAmmo);

	UFUNCTION()
	virtual void OnRep_RocketReserveAmmo(const FGameplayAttributeData& OldRocketReserveAmmo);

	UFUNCTION()
	virtual void OnRep_MaxRocketReserveAmmo(const FGameplayAttributeData& OldMaxRocketReserveAmmo);

	UFUNCTION()
	virtual void OnRep_ShotgunReserveAmmo(const FGameplayAttributeData& OldShotgunReserveAmmo);

	UFUNCTION()
	virtual void OnRep_MaxShotgunReserveAmmo(const FGameplayAttributeData& OldMaxShotgunReserveAmmo);

//private:
//	// Cache tags
//	FGameplayTag RifleAmmoTag;
//	FGameplayTag RocketAmmoTag;
//	FGameplayTag ShotgunAmmoTag;

};
