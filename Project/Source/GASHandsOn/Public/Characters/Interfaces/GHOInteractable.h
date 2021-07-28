// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "GHOInteractable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGHOInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
by GASShooter
	Interface for Actors that can be interacted with through the GameplayAbilitySystem.
和訳
	GameplayAbilitySystem を通してインタラクトすることができるアクターのためのインターフェイス。
*/
class GASHANDSON_API IGHOInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/**
	by GASShooter
		Is this object available for player interaction at right now?
	
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	和訳
		このオブジェクトは現在プレイヤーが操作できる状態にあるか？

		@param InteractionComponent UPrimitiveComponent はアクターが複数の独立したインタラクト可能なエリアを持っている場合に使用します。
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	bool IsAvailableForInteraction(UPrimitiveComponent* InteractionComponent) const;
	virtual bool IsAvailableForInteraction_Implementation(UPrimitiveComponent* InteractionComponent) const;

	/**
	by GASShooter
		How long does the player need to hold down the interact button to interact with this?
		
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	和訳
		プレイヤーはどのくらいの時間、インタラクトボタンを押していれば、これとインタラクトができるか？

		@param InteractionComponent UPrimitiveComponent はアクターが複数の独立したインタラクト可能なエリアを持っている場合に使用します。
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	float GetInteractionDuration(UPrimitiveComponent* InteractionComponent) const;
	virtual float GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const;

	/**
	by GASShooter
		Should we sync and who should sync before calling PreInteract()? 
		Defaults to false and OnlyServerWait.
		OnlyServerWait - client predictively calls PreInteract().
		OnlyClientWait - client waits for server to call PreInteract(). 
		                 This is useful if we are activating an ability on another ASC (player) and want to sync actions or animations with our Interact Duration timer.
		BothWait - client and server wait for each other before calling PreInteract().
		
		Player revive uses OnlyClientWait so that the player reviving is in sync with the server since we can't locally predict an ability run on another player. 
		The downed player's reviving animation will be in sync with the local player's Interact Duration Timer.
		
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	和訳
		PreInteract() を呼び出す前に、同期すべきか、と、誰が同期すべきか。
		デフォルトでは、 false と OnlyServerWait です。
		OnlyServerWait - クライアントが predictively （予測敵）に PreInteract() を呼び出します。
		OnlyClientWait - クライアントは、サーバーが PreInteract() を呼び出すのを待ちます。
		                 これは、他の ASC(player) のアビリティを起動中に、アクションやアニメーションを Interact Duration タイマーと同期させたい場合に便利です。
		BothWait - クライアントとサーバーは、PreInteract() を呼び出す前に、お互いに待機します。

		プレイヤーの蘇生には OnlyClientWait を使用し、蘇生するプレイヤーがサーバーと同期するようにしています。他のプレイヤーが実行したアビリティは locally predict （ローカル予測）できないためです。
		ダウンしたプレイヤーの蘇生アニメーションは、ローカルプレイヤーの Interact Duration タイマーに同期します。

		@param InteractionComponent UPrimitiveComponent はアクターが複数の独立したインタラクト可能なエリアを持っている場合に使用します。
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void GetPreInteractSyncType(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const;
	virtual void GetPreInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const;

	/**
	by GASShooter
		Should we sync and who should sync before calling PostInteract()? 
		Defaults to false and OnlyServerWait.
		OnlyServerWait - client predictively calls PostInteract().
		OnlyClientWait - client waits for server to call PostInteract().
		BothWait - client and server wait for each other before calling PostInteract().
		
		Player revive uses OnlyServerWait so that the client isn't stuck waiting for the server after the Interaction Duration ends.
		Revive's PostInteract() will only run code on the server so it's fine for the client to be "finished" ahead of the server.
		
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	和訳
		PostInteract() を呼び出す前に、同期すべきか、と、誰が同期すべきか。
		デフォルトでは、 false と OnlyServerWait です。
		OnlyServerWait - クライアントが predictively （予測敵）に PostInteract() を呼び出します。
		OnlyClientWait - クライアントは、サーバーが PostInteract() を呼び出すのを待ちます。
		BothWait - クライアントとサーバーは、PostInteract() を呼び出す前に、お互いに待機します。

		プレイヤーの蘇生には OnlyServerWait を使用し、 Interaction Duration が終了した後でクライアントがサーバーを待たないようにしています。
		蘇生の PostInteract() は、サーバー上でのみコードを実行する（ように実装している）ので、クライアントがサーバーより先に「終了」しても問題ありません。

		@param InteractionComponent UPrimitiveComponent はアクターが複数の独立したインタラクト可能なエリアを持っている場合に使用します。
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void GetPostInteractSyncType(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const;
	void GetPostInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const;

	/**
	by GASShooter
		Interact with this Actor. 
		This will call before starting the Interact Duration timer. 
		This might do things, apply (predictively or not) GameplayEffects, trigger (predictively or not) GameplayAbilities, etc.
		
		You can use this function to grant abilities that will be predictively activated on PostInteract() to hide the AbilitySpec replication time.
		
		If you want to do something predictively, you can get the ASC from the InteractingActor and use its ScopedPredictionKey.
		
		Player revives use PreInteract() to trigger a ability that plays an animation that lasts the same duration as the Interact Duration. 
		If this ability finishes, it will revive the player in PostInteract().
		
		@param InteractingActor The Actor interacting with this Actor. It will be the AvatarActor from a GameplayAbility.
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	和訳
		このアクターとインタラクトします。
		この関数は Interact Duration タイマーが開始する前に呼び出されます。
		この関数では、 (predictively （予測可能）またはそうではない) GameplayEffects の適用、 (predictively （予測可能）またはそうではない) GameplayAbilities の起動などの処理を行います。
		
		この関数を使用して、 PostInteract() で predictively （予測的）にアクティブ化されるアビリティを付与して、 AbilitySpec のレプリケーション時間を隠すことができます。
		
		predictively （予測的）に何かを行いたい場合は、 InteractingActor から ASC を取得し、 スコープ付き Prediction （予測）キー を使用します。 
		
		プレイヤーの蘇生では、 PreInteract() を、 Interact Duration と同じ期間続くアニメーションを再生するアビリティの起動に使用しています。
		このアビリティが終了すると、 PostInteract() でプレイヤーを蘇生させます。
		Player revives use PreInteract() to trigger a ability that plays an animation that lasts the same duration as the Interact Duration.
		If this ability finishes, it will revive the player in PostInteract().
		
		@param InteractingActor このアクターとインタラクトするアクター。これは GameplayAbility の AvatarActor になります。
		@param InteractionComponent UPrimitiveComponent はアクターが複数の独立したインタラクト可能なエリアを持っている場合に使用します。
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void PreInteract(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent);
	virtual void PreInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) {};

	/**
	by GASShooter
		Interact with this Actor. 
		This will call after the Interact Duration timer completes. 
		This might do things, apply (predictively or not) GameplayEffects, trigger (predictively or not) GameplayAbilities, etc.
		
		If you want to do something predictively, you can get the ASC from the InteractingActor and use its ScopedPredictionKey.
		
		If you need to trigger a GameplayAbility predictively, the player's ASC needs to have been granted the ability ahead of time. 
		If you don't want to grant every possible predictive ability at game start, you can hide the time needed to replicate the AbilitySpec inside the time needed to interact by granted it in PreInteract().
		
		@param InteractingActor The Actor interacting with this Actor. It will be the AvatarActor from a GameplayAbility.
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	和訳
		このアクターとインタラクトします。
		この関数は Interact Duration タイマーが完了した後に呼び出されます。
		この関数では、 (predictively （予測可能）またはそうではない) GameplayEffects の適用、 (predictively （予測可能）またはそうではない) GameplayAbilities の起動などの処理を行います。

		predictively （予測的）に何かを行いたい場合は、 InteractingActor から ASC を取得し、 スコープ付き Prediction （予測）キー を使用します。

		GameplayAbility を predictively （予測的）に起動する必要がある場合、プレイヤーの ASC に事前にアビリティが付与されている必要があります。
		ゲーム開始時に全ての predictive （予測）可能なアビリティを付与したくない場合は、 PreInteract() で付与することで、インタラクトに必要な時間の中に AbilitySpec のレプリケーション時間を隠すことができます。

		@param InteractingActor このアクターとインタラクトするアクター。これは GameplayAbility の AvatarActor になります。
		@param InteractionComponent UPrimitiveComponent はアクターが複数の独立したインタラクト可能なエリアを持っている場合に使用します。
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void PostInteract(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent);
	virtual void PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) {};

	/**
	by GASShooter
		Cancel an ongoing interaction, typically anything happening in PreInteract() while waiting on the Interact Duration Timer. 
		This will be called if the player releases input early.
		
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	和訳
		進行中のインタラクトをキャンセルします。通常は、 Interact Duration タイマーを待っている間に PreInteract() で起こっている事をキャンセルします。
		これは、プレイヤーの入力の開放が早かった場合に呼び出されます。

		@param InteractionComponent UPrimitiveComponent はアクターが複数の独立したインタラクト可能なエリアを持っている場合に使用します。
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void CancelInteraction(UPrimitiveComponent* InteractionComponent);
	virtual void CancelInteraction_Implementation(UPrimitiveComponent* InteractionComponent) {};

	/**
	by GASShooter
		Returns a delegate for GA_Interact to bind to that fires when this Actor is canceling the interactiong (e.g. died).
		
		@param InteractionComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	和訳
		GA_Interact がバインドできるように、アクターがインタラクトをキャンセルした時（例えば死亡した時）に起動するデリゲートを返します。
		
		@param InteractionComponent UPrimitiveComponent はアクターが複数の独立したインタラクト可能なエリアを持っている場合に使用します。
	*/
	virtual FSimpleMulticastDelegate* GetTargetCancelInteractionDelegate(UPrimitiveComponent* InteractionComponent);

	/**
	by GASShooter
		Registers an Actor interacting with this Interactable. 
		Used to send a GameplayEvent to them when this Interactable wishes to cancel interaction prematurely (e.g. a reviving player dies mid-revival). 
		Not meant to be overriden.
	和訳
		この Interactable とインタラクトするアクターを登録する。
		この Interactable がインタラクトを早期にキャンセルしたい場合（例えば蘇生中のプレイヤーが蘇生中に死亡した場合）に GameplayEvent を送るために使用します。
		overriden することはできません。
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable|Do Not Override")
	void RegisterInteracter(UPrimitiveComponent* InteractionComponent, AActor* InteractingActor);
	void RegisterInteracter_Implementation(UPrimitiveComponent* InteractionComponent, AActor* InteractingActor);

	/**
	by GASShooter
		Unregisters an interacting Actor from this Interactable. 
		Not meant to be overriden.
	和訳
		この Interactable とインタラクトするアクターの登録を解除する。
		overriden することはできません。
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable|Do Not Override")
	void UnregisterInteracter(UPrimitiveComponent* InteractionComponent, AActor* InteractingActor);
	void UnregisterInteracter_Implementation(UPrimitiveComponent* InteractionComponent, AActor* InteractingActor);

	/**
	by GASShooter
		Interactable (or an external Actor, not the Interacter) wants to cancel the interaction (e.g. the reviving player dies mid-revival). 
		Not meant to be overriden.
	和訳
		Interactable （または Interacter ではない外部のアクター）がインタラクトをキャンセルしたい場合（例えば蘇生中のプレイヤーが蘇生中に死亡した場合）に使用します。
		overriden することはできません。
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable|Do Not Override")
	void InteractableCancelInteraction(UPrimitiveComponent* InteractionComponent);
	void InteractableCancelInteraction_Implementation(UPrimitiveComponent* InteractionComponent);

protected:
	TMap<UPrimitiveComponent*, TArray<AActor*>> Interacters;


};
