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

};
