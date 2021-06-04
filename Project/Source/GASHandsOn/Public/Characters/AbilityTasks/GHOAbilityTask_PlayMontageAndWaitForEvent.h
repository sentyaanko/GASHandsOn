// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2020 Dan Kestranek.
// (C) Sentya Anko 2021
/*
解説
	このファイルは Epic によるサンプル ActionRPG に同梱されているソースの複製です。
		https://www.unrealengine.com/marketplace/ja/product/action-rpg
		Source/ActionRPG/Private/Abilities/RPGAbilityTask_PlayMontageAndWaitForEvent.cpp
		Source/ActionRPG/Public/Abilities/RPGAbilityTask_PlayMontageAndWaitForEvent.h
*/

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GHOAbilityTask_PlayMontageAndWaitForEvent.generated.h"

class UGHOAbilitySystemComponent;

/*
by Epic
	Delegate type used, EventTag and Payload may be empty if it came from the montage callbacks
和訳
	使用されるデリゲートの型、モンタージュのコールバックから呼ばれた場合、EventTag と Payload は空になることがあります。
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGHOPlayMontageAndWaitForEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

/*
by Epic
	This task combines PlayMontageAndWait and WaitForEvent into one task, so you can wait for multiple types of activations such as from a melee combo
	Much of this code is copied from one of those two ability tasks
	This is a good task to look at as an example when creating game-specific tasks
	It is expected that each game will have a set of game-specific tasks to do what they want
和訳
	このタスクは PlayMontageAndWait と WaitForEvent を1つのタスクにまとめたもので、 Melee Combo などの複数のタイプの起動を待つことが出来ます。
	このコードの多くは、これらの２つの AbilityTask のうちの１つからコピーされています。
	これは、ゲーム固有のタスクを作成する際に、例としてみるのに適したタスクです。
	各ゲームには、ゲーム固有のタスクが用意されていることが予想されます。
*/
UCLASS()
class GASHANDSON_API UGHOAbilityTask_PlayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	UGHOAbilityTask_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer);

	/*
	by Epic
		The Blueprint node for this task, PlayMontageAndWaitForEvent, has some black magic from the plugin that automagically calls Activate()
		inside of K2Node_LatentAbilityCall as stated in the AbilityTask.h. 
		Ability logic written in C++ probably needs to call Activate() itself manually.
	和訳
		このタスクの BluePrint ノードである PlayMontageAndWaitForEvent には、 AbilityTask.h で記載されているように、 
		K2Node_LatentAbilityCall の内部で Activate() を自動的に呼び出すプラグインによる黒魔法があります。
		C++ で書かれたアビリティロジックではおそらく手動で Activate() を呼び出す必要があります。
	解説
		AbilityTask.h には 40 行ほどの AbilityTask に関するドキュメントが記載されています。
		和訳は [](https://github.com/sentyaanko/GASHandsOn/blob/main/Documents/Add-AbilityTask_PlayMontageAndWaitForEvent.md) で記載しています。
	*/
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	/*
	by Epic
		The montage completely finished playing
	和訳
		モンタージュの再生が完了した
	*/
	UPROPERTY(BlueprintAssignable)
	FGHOPlayMontageAndWaitForEventDelegate OnCompleted;

	/*
	by Epic
		The montage started blending out
	和訳
		モンタージュのブレンドアウトが開始した
	*/
	UPROPERTY(BlueprintAssignable)
	FGHOPlayMontageAndWaitForEventDelegate OnBlendOut;

	/*
	by Epic
		The montage was interrupted
	和訳
		モンタージュが中断された
	*/
	UPROPERTY(BlueprintAssignable)
	FGHOPlayMontageAndWaitForEventDelegate OnInterrupted;

	/*
	by Epic
		The ability task was explicitly cancelled by another ability
	和訳
		AbilityTask が 他の Ability によって明示的にキャンセルされた
	*/
	UPROPERTY(BlueprintAssignable)
	FGHOPlayMontageAndWaitForEventDelegate OnCancelled;

	/*
	by Epic
		One of the triggering gameplay events happened
	和訳
		トリガーとなる GameplayEvent の１つが発生した
	*/
	UPROPERTY(BlueprintAssignable)
	FGHOPlayMontageAndWaitForEventDelegate EventReceived;

	/**
	by Epic
		Play a montage and wait for it end. 
		If a gameplay event happens that matches EventTags (or EventTags is empty), the EventReceived delegate will fire with a tag and event data.
		If StopWhenAbilityEnds is true, this montage will be aborted if the ability ends normally. 
		It is always stopped when the ability is explicitly cancelled.
		On normal execution, OnBlendOut is called when the montage is blending out, and OnCompleted when it is completely done playing
		OnInterrupted is called if another montage overwrites this, and OnCancelled is called if the ability or task is cancelled
		
		@param TaskInstanceName Set to override the name of this task, for later querying
		@param MontageToPlay The montage to play on the character
		@param EventTags Any gameplay events matching this tag will activate the EventReceived callback. If empty, all events will trigger callback
		@param Rate Change to play the montage faster or slower
		@param bStopWhenAbilityEnds If true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled
		@param AnimRootMotionTranslationScale Change to modify size of root motion or set to 0 to block it entirely
	和訳
		モンタージュを再生して、終わるのを待ちます。
		もし GameplayEvent が発生した場合（もしくは EventTags が空の場合）、 EventReceived デリゲートが タグとイベントデータとともに発火します。
		StopWhenAbilityEnds が true の場合、アビリティが正常に終了するとこのモンタージュは中止されます。
		アビリティが明示的にキャンセルされた場合は常に停止されます。
		通常の実行では、OnBlendOut はモンタージュがブレンドアウトしたときにが呼び出され、OnCompleted は再生が完全に終了したときに呼び出されます。
		OnInterrupted は別のモンタージュがこれを上書きした場合に呼び出され、 OnCancelled はアビリティやタスクがキャンセルされた場合に呼び出されます。
		
		@param TaskInstanceName このタスクの名前を上書きするために設定し、後で照会できるようにする
		@param MontageToPlay キャラクターで再生するモンタージュ
		@param EventTags このタグにマッチする GameplayEvent は EventReceived コールバックを起動する。からの場合は、全てのイベントがコールバックを起動する。
		@param Rate モンタージュの再生速度を早くしたり遅くしたり変更する
		@param bStopWhenAbilityEnds もし ture なら、愛リティが正常に終了した場合、このモンタージュは中止される。アビリティが明示的にキャンセルされた場合は、常に停止される。
		@param AnimRootMotionTranslationScale ルートモーションのサイズを変更するか 0 にして完全にブロックする。
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UGHOAbilityTask_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(
			UGameplayAbility* OwningAbility,
			FName TaskInstanceName,
			UAnimMontage* MontageToPlay,
			FGameplayTagContainer EventTags,
			float Rate = 1.f,
			FName StartSection = NAME_None,
			bool bStopWhenAbilityEnds = true,
			float AnimRootMotionTranslationScale = 1.f);

private:
	/*
	by Epic
		Montage that is playing
	和訳
		再生中のモンタージュ
	*/
	UPROPERTY()
	UAnimMontage* MontageToPlay;

	/*
	by Epic
		List of tags to match against gameplay events
	和訳
		GameplayEvent と照合するタグのリスト
	*/
	UPROPERTY()
	FGameplayTagContainer EventTags;

	/*
	by Epic
		Playback rate
	和訳
		再生レート
	*/
	UPROPERTY()
	float Rate;

	/*
	by Epic
		Section to start montage from
	和訳
		モンタージュを開始するセクション
	*/
	UPROPERTY()
	FName StartSection;

	/*
	by Epic
		Modifies how root motion movement to apply
	和訳
		ルートモーションの適用方法を変更する
	*/
	UPROPERTY()
	float AnimRootMotionTranslationScale;

	/*
	by Epic
		Rather montage should be aborted if ability ends
	和訳
		アビリティが終了した場合は、いっその事モンタージュを中止するべき
	*/
	UPROPERTY()
	bool bStopWhenAbilityEnds;

	/*
	by Epic
		Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not.
	和訳
		アビリティがモンタージュを再生しているかどうかをチェックし、そのモンタージュの再生を停止します。
		モンタージュを停止した場合は ture を、そうでない場合は false を返します。
	*/
	/**  */
	bool StopPlayingMontage();

	/*
	by Epic
		Returns our ability system component
	和訳
		AbilitySystemComponent を返します。
	*/
	UGHOAbilitySystemComponent* GetTargetASC();

	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
	
};
