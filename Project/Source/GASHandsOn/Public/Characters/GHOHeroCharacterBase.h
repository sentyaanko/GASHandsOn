// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GHOCharacterBase.h"
#include "UI/GHOFloatingStatusBarWidget.h"
#include "Characters/Interfaces/GHOInteractable.h"
#include "GHOHeroCharacterBase.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API AGHOHeroCharacterBase : public AGHOCharacterBase, public IGHOInteractable
{
	GENERATED_BODY()
	
public:
	AGHOHeroCharacterBase(const FObjectInitializer& ObjectInitializer);

	// AActor interface
protected:
	/*
	by Epic
		Overridable native event for when play begins for this actor.
	和訳
		このアクタの Play が始まるときのオーバーライド可能なネイティブイベント。
	*/
	virtual void BeginPlay()override;

	// End of AActor interface

	// APawn interface
public:
	virtual void OnRep_PlayerState()override;
	
	/*
	by Epic
		Called when this Pawn is possessed. Only called on the server (or in standalone).
		@param NewController The controller possessing this pawn
	by GASDocumentation
		Only called on the Server. 
		Calls before Server's AcknowledgePossession.
	和訳
		このポーンが週されたときに呼び出されます。
		サーバー上（またはスタンドアローン）でのみ呼び出されます。
		@param NewController このポーンを周遊するコントローラー
		サーバー上のみで呼び出されます。
		AcknowledgePossession の前に呼び出されます。
	*/
	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface


	// IAbilitySystemInterface interface
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// End of IAbilitySystemInterface interface

	// AGHOCharacterBase interface
public:
	virtual class UGHOAttributeSetBase* GetAttributeSet() override;
	virtual const class UGHOAttributeSetBase* GetAttributeSet() const override;
	virtual void FinishDying()override;
	virtual FTransform GetProjectileTransform(float Range)const override;

	// End of AGHOCharacterBase interface


protected:
	// IGHOInteractable interface

	/**
	by GASShooter
		We can Interact with other heroes when:
		Knocked Down - to revive them
	和訳
		他のヒーローとインタラクトできます:
		Knocked Down - 蘇生させる
	*/
	virtual bool IsAvailableForInteraction_Implementation(UPrimitiveComponent* InteractionComponent) const override;

	/**
	by GASShooter
		How long to interact with this player:
		Knocked Down - to revive them
	和訳
		このプレイヤーとのインタラクト時間:
		Knocked Down - 蘇生させる
	*/
	virtual float GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const override;

	/**
	by GASShooter
		Interaction:
		Knocked Down - activate revive GA (plays animation)
	和訳
		インタラクト:
		Knocked Down - 蘇生の GA の有効化（アニメーション再生）
	*/
	virtual void PreInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) override;

	/**
	by GASShooter
		Interaction:
		Knocked Down - apply revive GE
	和訳
		インタラクト:
		Knocked Down - 蘇生の GE の適用
	*/
	virtual void PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent) override;

	/**
	by GASShooter
		Should we wait and who should wait to sync before calling PreInteract():
		Knocked Down - Yes, client. 
			This will sync the local player's Interact Duration Timer with the knocked down player's revive animation. 
			If we had a picking a player up animation, we could play it on the local player in PreInteract().
	和訳
		PreInteract() を呼び出す前に、同期すべきか、と、誰が同期すべきか:
		Knocked Down - 同期すべき、クライアントが。
			これにより、ローカルプレイヤーの Interact Duration タイマーがノックダウンしているプレイヤーの蘇生アニメーションと同期します。
			プレイヤーを抱き上げるアニメーションがあれば、 PreInteract() でローカルプレイヤーに再生することができます。
	*/
	/**
	*/
	virtual void GetPreInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const override;

	/**
	by GASShooter
		Cancel interaction:
		Knocked Down - cancel revive ability
	和訳
		インタラクトのキャンセル:
		Knocked Down - 蘇生アビリティのキャンセル
	*/
	virtual void CancelInteraction_Implementation(UPrimitiveComponent* InteractionComponent) override;

	/**
	by GASShooter
		Get the delegate for this Actor canceling interaction:
		Knocked Down - cancel being revived if killed
	和訳
		このアクターがキャンセルしたインタラクトのデリゲートを取得:
		Knocked Down - 殺されたなら、蘇生をキャンセル
	*/
	FSimpleMulticastDelegate* GetTargetCancelInteractionDelegate(UPrimitiveComponent* InteractionComponent) override;

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	class UGHOFloatingStatusBarWidget* GetFloatingStatusBar();


	// Server handles knockdown - cancel abilities, remove effects, activate knockdown ability
	void KnockDown();

	// Plays knockdown effects for all clients from KnockedDown tag listener on PlayerState
	void PlayKnockDownEffects();

	// Plays revive effects for all clients from KnockedDown tag listener on PlayerState
	void PlayReviveEffects();



private:
	void InitializeAbilitySystemWeakObjects(class AGHOPlayerState* playerState);
	void InitializeAfterAbilitySystem();
	void BindASCInput();

	/*
	by GASDocumentation
		Creates and initializes the floating status bar for heroes.
		Safe to call many times because it checks to make sure it only executes once.
	和訳
		ヒーロー用のフローティングステータスバーを作成、初期化します。
		一度しか実行されないことを確認しているので、何度も呼び出しても安全です。
	*/
	UFUNCTION()
	void InitializeFloatingStatusBar();

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAGHandsOn|UI", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* UIFloatingStatusBarComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAGHandsOn|UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGHOFloatingStatusBarWidget> UIFloatingStatusBarClass;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY()
	class UGHOFloatingStatusBarWidget* UIFloatingStatusBar;

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAGHandsOn|GHOHeroCharacterBase")
	TSubclassOf<UGameplayEffect> KnockDownEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAGHandsOn|GHOHeroCharacterBase")
	TSubclassOf<UGameplayEffect> ReviveEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAGHandsOn|GHOHeroCharacterBase")
	TSubclassOf<UGameplayEffect> DeathEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAGHandsOn|Abilities")
	float ReviveDuration;

private:
	/*
	by GASDocumentation
		Instead of TWeakObjectPtrs, you could just have UPROPERTY() hard references or no references at all and just call
		GetAbilitySystem() and make a GetAttributeSetBase() that can read from the PlayerState or from child classes.
		Just make sure you test if the pointer is valid before using.
		I opted for TWeakObjectPtrs because I didn't want a shared hard reference here and I didn't want an extra function call of getting
		the ASC/AttributeSet from the PlayerState or child classes every time I referenced them in this base class.
	和訳
		TWeakObjectPtrs の代わりに、 UPROPERTY() を使用してハードリファレンスを用意するか、
		もしくはリファレンスを全く用意せず、 GetAbilitySystem() を呼び出して、
		PlayerState や子クラスから読み取れる GetAttributeSetBase() を作ることが出来ます。
		ただ、使用する前にポインタが有効かどうかをテストするいつようがあります。
		TWeakObjectPtrs を選択したのは、ここでハードリファレンスを共有したくなかったのと、このベースクラスで参照する度に 
		PlayerState や子クラスから ASC/AttributeSet を取得する余分な関数呼び出しをしたくなかったからです。
	補足
		GASDocumentation では、この変数はこのクラスの基底である AGHOCharacterBase で所持していました。
		私的には以下のルールにしたほうが自然と感じたため、ここに移動しています。
			AGHOCharacterBase から使用する場合は仮想関数経由で取得する
				nullptr チェックを行うのは、何れにせよ必須な為、何を利用しても変わらない。
				基底クラスで用意する GetHealth() などが仮想関数呼び出し一回分の関数コールが増えることにはなる。
			プレイヤーが操作するキャラクターは PlayerState が所持するハードリファレンスの弱参照をここで保持する
			AI が操作するキャラクターは ミニオン派生の基底クラスでハードリファレンスを保持する
	*/
	TWeakObjectPtr<class UGHOAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UGHOAttributeSetBase> AttributeSetBase;

	bool bASCInputBound = false;

	FSimpleMulticastDelegate InteractionCanceledDelegate;
};
