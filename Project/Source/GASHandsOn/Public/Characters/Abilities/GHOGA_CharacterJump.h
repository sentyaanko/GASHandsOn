// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "GHOGA_CharacterJump.generated.h"

// This code is based on Engine\Plugins\Runtime\GameplayAbilities\Source\GameplayAbilities\Public\Abilities\GameplayAbility_CharacterJump.h

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOGA_CharacterJump : public UGHOGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGHOGA_CharacterJump();

	// UGameplayAbility interface
public:
	/*
	by Epic
		Returns true if this ability can be activated right now. Has no side effects
	和訳
		このアビリティが今すぐに起動できる場合は true を返します。副作用はありません。
	*/
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	/*
	by Epic
		Destroys instanced-per-execution abilities. 
		Instance-per-actor abilities should 'reset'. 
		Any active ability state tasks receive the 'OnAbilityStateInterrupted' event. 
		Non instance abilities - what can we do?
	和訳
		instanced-per-execution のアビリティを破棄する。
		instanced-per-execution のアビリティは「リセット」されるべきである。
		アクティブなアビリティ状態タスクは 'OnAbilityStateInterrupted' イベントを受け取ります。
		非インスタンスアビリティ - 何ができる？
	*/
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	/*
	by Epic
		Input binding stub.
	和訳
		入力バインディングのスタブ。
	*/
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	/*
	by Epic
		Actually activate ability, do not call this directly
	和訳
		実際にアビリティを起動する、これを直接呼び出さないでください。
	*/
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// End of UGameplayAbility interface

};
