// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASHandsOn.h"
#include "GHOGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGHOGameplayAbility();

	// UGameplayAbility interface
public:
	/*
	by Epic
		Called when the avatar actor is set/changes
	和訳
		Avator Actor が設定/変更されたときに呼び出される。
	by GASDocumentation
		If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
		Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	和訳
		アビリティに「ActivateAbilityOnGranted」が設定されている場合は、ここで与えられたときにすぐに起動する。
		エピックのコメント : プロジェクトでは、パッシブを開始したり、その他の「BeginPlay」タイプのロジックをここで行うことができます。
	*/
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	/**
	by Epic
		const function to see if ability is activatable. Callable by UI etc

		Returns true if this ability can be activated right now. Has no side effects 
	和訳
		アビリティが起動可能かどうかを確認するための const 関数です。 UI などで呼び出し可能です。

		このアビリティが今すぐにでも起動できる場合は true を返します。副作用はありません。
	*/
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	// End of UGameplayAbility interface

public:
	bool IsActivateOnInput()const { return bActivateOnInput; }

protected:
	/*
	by GASShooter
		Is the player's input currently pressed? Only works if the ability is bound to input.
	和訳
		プレイヤーの入力は現在押されているか？アビリティが入力にバインドされている場合のみ動作します。
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual bool IsInputPressed() const;

public:
	/*
	by GASDocumentation
		Abilities with this set will automatically activate when the input is pressed
	和訳
		このセットのアビリティは入力が押されると自動的にアクティブになります。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGHOAbilityInputID AbilityInputID = EGHOAbilityInputID::None;

	/*
	by GASDocumentation
		Value to associate an ability with an slot without tying it to an automatically activated input.
		Passive abilities won't be tied to an input so we need a way to generically associate abilities with slots.
	和訳
		アビリティを自動的にアクティブ化された入力に結び付けずに関連付ける値。
		パッシブアビリティは入力に関連付けられないため、アビリティをスロットに一般的に関連付ける方法が必要です。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGHOAbilityInputID AbilityID = EGHOAbilityInputID::None;

protected:
	/*
	by GASDocumentation
		Tells an ability to activate immediately when its granted. 
		Used for passive abilities and abilities forced on others.
	和訳
		アビリティが付与されたときにすぐに発動するようにします。
		パッシブアビリティや他の人に強制するアビリティに使用します。
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bActivateAbilityOnGranted = false;

	/*
	by GASShooter
		If true, this ability will activate when its bound input is pressed. 
		Disable if you want to bind an ability to an input but not have it activate when pressed.
	和訳
		true の場合、このアビリティはバインドされた入力が押された時に起動します。
		入力にアビリティを割り当てたいが、押されても起動しないようにしたい場合は無効にします。
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bActivateOnInput;

	/*
	by GASShooter
		If true, only activate this ability if not interacting with something via GA_Interact.
	和訳
		true の場合、 GA_Interact を介して何かとインタラクトしていない場合にのみ、このアビリティを起動します。
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bCannotActivateWhileInteracting;

};
