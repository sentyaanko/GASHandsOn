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

public:
	/*
	by GASDocumentation
		Version of function in AbilitySystemGlobals that returns correct type
	和訳
		AbilitySystemGlobals の関数の正しい方を返すバージョン。
	*/
	static UGHOAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);

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
	/** 	 
	by Epic
		Allows GameCode to add loose gameplaytags which are not backed by a GameplayEffect.
		
		Tags added this way are not replicated!
		
		It is up to the calling GameCode to make sure these tags are added on clients/server where necessary
	和訳
		ゲームコードが GameplayEffect に裏付けられていないゆるい GameplayTags を追加できるようにします。

		この方法で追加されたタグは複製されません！

		必要に応じて、これらのタグがクライアント/サーバーに追加されていることを確認するのは呼び出したゲームコードの責任です。
	*/
	/**
	by GASShooter
		Exposes AddLooseGameplayTag to Blueprint. This tag is *not* replicated.
	和訳
		AddLooseGameplayTag をブループリントに公開します。このタグは複製*されません*。
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities", Meta = (DisplayName = "AddLooseGameplayTag"))
	void K2_AddLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count = 1);

	/**
	by GASShooter
		Exposes AddLooseGameplayTags to Blueprint. These tags are *not* replicated.
	和訳
		AddLooseGameplayTags をブループリントに公開します。このタグは複製*されません*。
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities", Meta = (DisplayName = "AddLooseGameplayTags"))
	void K2_AddLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);

	/**
	by GASShooter
		Exposes RemoveLooseGameplayTag to Blueprint. This tag is *not* replicated.
	和訳
		RemoveLooseGameplayTag をブループリントに公開します。このタグは複製*されません*。
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities", Meta = (DisplayName = "RemoveLooseGameplayTag"))
	void K2_RemoveLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count = 1);

	/**
	by GASShooter
		Exposes RemoveLooseGameplayTags to Blueprint. These tags are *not* replicated.
	和訳
		RemoveLooseGameplayTags をブループリントに公開します。このタグは複製*されません*。
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities", Meta = (DisplayName = "RemoveLooseGameplayTags"))
	void K2_RemoveLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Abilities")
	FGameplayAbilitySpecHandle FindAbilitySpecHandleForClass(TSubclassOf<UGameplayAbility> AbilityClass, UObject* OptionalSourceObject=nullptr);

	/**
	by GASShooter
		Attempts to activate the given ability handle and batch all RPCs into one. 
		This will only batch all RPCs that happen in one frame. 
		Best case scenario we batch ActivateAbility, SendTargetData, and EndAbility into one RPC instead of three.
		Worst case we batch ActivateAbility and SendTargetData into one RPC instead of two and call EndAbility later in a separate RPC. 
		If we can't batch SendTargetData or EndAbility with ActivateAbility because they don't happen in the same frame due to latent ability tasks for example, then batching doesn't help and we should just activate normally.
		Single shots (semi auto fire) combine ActivateAbility, SendTargetData, and EndAbility into one RPC instead of three.
		Full auto shots combine ActivateAbility and SendTargetData into one RPC instead of two for the first bullet. 
		Each subsequent bullet is one RPC for SendTargetData. 
		We then send one final RPC for the EndAbility when we're done firing.
	和訳
		与えられたアビリティハンドルを起動し、全てのRPCを一つにまとめようとします。
		これは、 1 フレーム内に発生する全ての RPC を一括して処理します。
		最良のシナリオでは、 ActivateAility, SendTargetData, EndAbility を 3 つの RPC ではなく 1 つの RPC にまとめます。
		ワーストケースでは、 ActivateAility, SendTargetData を 2 つの RPC ではなく 1 つの RPC にまとめ、EndAbility を後で別の RPC で呼び出します。
		もし、SendTargetData や EndAbility が ActivateAbility でバッチ処理できない、例えば latent （潜在的）なアビリティタスクのために同じフレーム内で発生しない場合は、バッチ処理は役に立たないので、普通に起動するべきです。
		単発射撃（セミオート射撃）では、 ActivateAbility, SendTargetData, EndAbility を 3 つではなく 1 つの RPC にまとめます。
		フルオート射撃では、 初弾は ActivateAbility, SendTargetData を 2 つではなく 1 つの RPC にまとめます。
		その後の各弾は、 SendTargetData のための 1 つの RPC です。
		撃ち終わったら、最後に EndAbility 用の RPC を 1 つ送ります。

	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	/*virtual*/ bool BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately);

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
