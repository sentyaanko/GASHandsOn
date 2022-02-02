// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "GameplayAbilitySpec.h"
#include "GHOBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	* FGameplayAbilityTargetDataHandle
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability|TargetData")
	static void ClearTargetData(UPARAM(ref) FGameplayAbilityTargetDataHandle& TargetData);

	/**
	* FGameplayAbilitySpecHandle
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|Ability")
	static bool IsAbilitySpecHandleValid(FGameplayAbilitySpecHandle Handle);

	/**
	* GameplayAbility
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability")
	static UGHOGameplayAbility* GetPrimaryAbilityInstanceFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability")
	static UGHOGameplayAbility* GetPrimaryAbilityInstanceFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> InAbilityClass, bool bAllowInheritedType);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability")
	static bool IsPrimaryAbilityInstanceActive(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle);

	/**
	* FGHOGameplayEffectContainerSpec
	*/

	/*
	by GASShooter
		Adds targets to a copy of the passed in effect container spec and returns it
	和訳
		ターゲットを渡された EffectContainerSpec のコピーに追加し、それを返します。
	解説
		この関数は Epic のサンプルプロジェクトである ActionRPG の RPGBlueprintLibrary で作成されている機能を元に
		GASShooter の GSBlueprintFunctionLibrary で拡張されたものです。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability|Container", Meta = (AutoCreateRefTerm = "TargetData, HitResults, TargetActors"))
	static void AddTargetsToEffectContainerSpec(UPARAM(ref) FGHOGameplayEffectContainerSpec& ContainerSpec, const TArray<FGameplayAbilityTargetDataHandle>& TargetData, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	/*
	by GASShooter
		Applies container spec that was made from an ability
	和訳
		アビリティから作られた EffectContainerSpec を適用
	解説
		この関数は Epic のサンプルプロジェクトである ActionRPG の RPGBlueprintLibrary で作成されている機能を元に
		GASShooter の GSBlueprintFunctionLibrary で拡張されたものです。
		実装内容は同じです。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability|Container")
	static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(const FGHOGameplayEffectContainerSpec& ContainerSpec);

	/**
	* FGHOGameplayEffectContext
	*/

	// Returns TargetData
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability|EffectContext", Meta = (DisplayName = "GetTargetData"))
	static FGameplayAbilityTargetDataHandle EffectContextGetTargetData(FGameplayEffectContextHandle EffectContext);

	// Adds TargetData
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability|EffectContext", Meta = (DisplayName = "AddTargetData"))
	static void EffectContextAddTargetData(FGameplayEffectContextHandle EffectContextHandle, const FGameplayAbilityTargetDataHandle& TargetData);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|Ability|TargetData")
	static FGameplayAbilityTargetDataHandle	AbilityTargetDataFromInt32(int32 Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|Ability|TargetData")
	static FGameplayAbilityTargetDataHandle	AbilityTargetDataFromName(FName Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|Ability|TargetData")
	static int32 GetInt32FromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|Ability|TargetData")
	static FName GetNameFromTargetData(const FGameplayAbilityTargetDataHandle& TargetData, int32 Index);

};
