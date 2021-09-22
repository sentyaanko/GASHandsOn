// (C) Sentya Anko 2021

#pragma once

/*
by GASShooter
	This header is for Ability-specific structures and enums that are shared across a project
	Every game will probably need a file like this to handle their extensions to the system
	This file is a good place for subclasses of FGameplayEffectContext and FGameplayAbilityTargetData
和訳
	このヘッダーは、プロジェクト全体で共有される、アビリティ固有の構造体や列挙体のためのものです。
	どんなゲームでも、システムの拡張を扱うために、このようなファイルが必要になるでしょう。
	このファイルは FGameplayEffectContext や FGameplayAbilityTargetData のサブクラスを置くのに適しています。
*/

#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEffectStackChange.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEffectRemoved.h"
#include "Characters/Types/GHOTargetTypes.h"
#include "GHOGameplayEffectTypes.generated.h"

class UGameplayEffect;

/*
by GASShooter
	Struct defining a list of gameplay effects, a tag, and targeting info
	These containers are defined statically in blueprints or assets and then turn into Specs at runtime
和訳
	GameplayEffect のリスト、タグ、ターゲティング情報を定義する構造体。
	これらのコンテナはブループリントやアセットで静的に定義され、実行しには Specs に変わります。
*/
USTRUCT(BlueprintType)
struct FGHOGameplayEffectContainer
{
	GENERATED_BODY()

public:
	FGHOGameplayEffectContainer() {}

	/*
	by GASShooter
		Sets the way that targeting happens
	和訳
		ターゲティングの方法を設定する。
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TSubclassOf<UGHOTargetType> TargetType;

	/*
	by GASShooter
		List of gameplay effects to apply to the targets
	和訳
		ターゲットの適用する GameplayEffect のリスト。
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<TSubclassOf<UGameplayEffect>> TargetGameplayEffectClasses;
};

/*
by GASShooter
	A "processed" version of GHOGameplayEffectContainer that can be passed around and eventually applied
和訳
	GHOGameplayEffectContainer の「処理済み」バージョンで、周囲に渡して最終的に適用することができます。
*/
USTRUCT(BlueprintType)
struct FGHOGameplayEffectContainerSpec
{
	GENERATED_BODY()

public:
	FGHOGameplayEffectContainerSpec() {}

	/*
	by GASShooter
		Computed target data
	和訳
		計算されたターゲットデータ。
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayAbilityTargetDataHandle TargetData;

	/*
	by GASShooter
		List of gameplay effects to apply to the targets
	和訳
		ターゲットに適用する GameplayEffect のリスト。
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;

	/*
	by GASShooter
		Returns true if this has any valid effect specs
	和訳
		何らかな有効な EffectSpec がある場合は true を返す。
	*/
	bool HasValidEffects() const;

	/*
	by GASShooter
		Returns true if this has any valid targets
	和訳
		何らかな有効なターゲットがある場合は true を返す。
	*/
	bool HasValidTargets() const;

	/*
	by GASShooter
		Adds new targets to target data
	和訳
		ターゲットデータに新しいターゲットを追加する。
	*/
	void AddTargets(const TArray<FGameplayAbilityTargetDataHandle>& TargetData, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	/*
	by GASShooter
		Clears target data
	和訳
		ターゲットデータをクリアする。
	*/
	void ClearTargets();
};

