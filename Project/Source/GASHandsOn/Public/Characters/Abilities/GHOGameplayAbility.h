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

//	// UGameplayAbility interface
//public:
//	/** Called when the avatar actor is set/changes */
//	// If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
//	// Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
//	/*
//		アビリティに「ActivateAbilityOnGranted」が設定されている場合は、ここで与えられたときにすぐに起動する。
//		エピックのコメント : プロジェクトでは、パッシブを開始したり、その他の「BeginPlay」タイプのロジックをここで行うことができます。
//	*/
//	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
//
//	// End of UGameplayAbility interface

public:
	// Abilities with this set will automatically activate when the input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGHOAbilityInputID AbilityInputID = EGHOAbilityInputID::None;

	// Value to associate an ability with an slot without tying it to an automatically activated input.
	// Passive abilities won't be tied to an input so we need a way to generically associate abilities with slots.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EGHOAbilityInputID AbilityID = EGHOAbilityInputID::None;

	//// Tells an ability to activate immediately when its granted. Used for passive abilities and abilities forced on others.
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	//bool bActivateAbilityOnGranted = false;

};
