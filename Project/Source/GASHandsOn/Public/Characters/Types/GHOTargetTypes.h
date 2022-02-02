// (C) Sentya Anko 2021

#pragma once

/*
補足
	このファイルの内容は Epic のサンプルプロジェクトである ActionRPG の RPGTargetType を元に作られている
	GASShooter で拡張された GSTargetType を元にしています。
	クラス名が変わっているだけで、実装内容は同じです。
*/

#include "Abilities/GameplayAbilityTypes.h"
#include "GHOTargetTypes.generated.h"

class AGHOCharacterBase;
class AActor;
struct FGameplayEventData;


/*
by GASShooter
	Class that is used to determine targeting for abilities
	It is meant to be blueprinted to run target logic
	This does not subclass GameplayAbilityTargetActor because this class is never instanced into the world
	This can be used as a basis for a game-specific targeting blueprint
	If your targeting is more complicated you may need to instance into the world once or as a pooled actor
和訳
	アビリティのターゲティングを決定するために使用されるクラス。
	ターゲットロジックを実行するためにブループリントされることを意図しています。
	これは GameplayAbilityTargetActor のサブクラスではありません。なぜなら、このクラスはワールドにインスタンス化されないからです。
	これはゲーム固有のターゲティングブループリントのベースとして使用できます。
	もしあなたのターゲティングがより複雑ならば、一度ワールドにインスタンス化するか、プールされたアクターが必要かもしれません。
*/
UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class GASHANDSON_API UGHOTargetType : public UObject
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGHOTargetType() {}

	/*
	by GASShooter
		Called to determine targets to apply gameplay effects to
	和訳
		GameplayEffect を適用するターゲットを決定するために呼び出されます。
	解説
		UGHOTargetType::GetTargets_Implementation() の実装は空だが、派生クラスでオーバーライドし拡張する。
		そのため、 BlueprintImplementableEvent を使用しないのが正しい。
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "GASHandsOn")
	void GetTargets(AGHOCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
	virtual void GetTargets_Implementation(AGHOCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

/*
by GASShooter
	Trivial target type that uses the owner
和訳
	オーナーを使用する平凡なターゲットタイプ
*/
UCLASS(NotBlueprintable)
class GASHANDSON_API UGHOTargetType_UseOwner : public UGHOTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGHOTargetType_UseOwner() {}

	/*
	by GASShooter
		Uses the passed in event data
	和訳
		渡されたイベントデータを使用。
	*/
	virtual void GetTargets_Implementation(AGHOCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/*
by GASShooter
	Trivial target type that pulls the target out of the event data
和訳
	イベントデータからターゲットを抽出する平凡なターゲットタイプ
*/
UCLASS(NotBlueprintable)
class GASHANDSON_API UGHOTargetType_UseEventData : public UGHOTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGHOTargetType_UseEventData() {}

	/*
	by GASShooter
		Uses the passed in event data
	和訳
		渡されたイベントデータを使用。
	*/
	virtual void GetTargets_Implementation(AGHOCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/*
	SphereTrace などで取得した HitResults をフィルターを行う基底クラス
*/
UCLASS(Blueprintable)
class GASHANDSON_API UGHOHitResultFilterType : public UObject
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGHOHitResultFilterType() {}

	/*
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GASHandsOn")
	void FilterHitResults(TArray<FHitResult>& OutHitResults, UPARAM(ref) TArray<FHitResult>& InHitResults) const;
	virtual void FilterHitResults_Implementation(TArray<FHitResult>& OutHitResults, UPARAM(ref) TArray<FHitResult>& InHitResults) const;
};
