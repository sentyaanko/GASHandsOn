// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GHOCharacterBase.h"
#include "UI/GHOFloatingStatusBarWidget.h"
#include "Characters/Interfaces/GHOInteractable.h"
#include "GHOHeroCharacterBase.generated.h"


class UGameplayEffect;

class AGHOWeapon;

UENUM(BlueprintType)
enum class EGHOHeroWeaponState : uint8
{
	// 0
	Rifle					UMETA(DisplayName = "Rifle"),
	// 1
	RifleAiming				UMETA(DisplayName = "Rifle Aiming"),
	// 2
	RocketLauncher			UMETA(DisplayName = "Rocket Launcher"),
	// 3
	RocketLauncherAiming	UMETA(DisplayName = "Rocket Launcher Aiming")
};

USTRUCT()
struct GASHANDSON_API FGHOHeroInventory
{
//	GENERATED_USTRUCT_BODY()
	GENERATED_BODY()

	UPROPERTY()
	TArray<AGHOWeapon*> Weapons;

	// Consumable items

	// Passive items like armor

	// Door keys

	// Etc
};

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
public:
	/*
	by Epic
		Returns the properties used for network replication, this needs to be overridden by all actor classes with native replicated properties
	和訳
		ネットワークレプリケーションに使用されるプロパティを返します。これはネイティブレプリケートされたプロパティを持つ全てのアクタークラスによってオーバーライドされる必要があります。
	*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*
	by Epic
		Allow actors to initialize themselves on the C++ side after all of their components have been initialized, only called during gameplay
	和訳
		全てのコンポーネントが初期化された後、　C++ 側でアクターが自分自身を初期化できるようにし、ゲームプレイ中にのみ呼び出されるようにする。
	*/
	virtual void PostInitializeComponents() override;

protected:
	/*
	by Epic
		Overridable native event for when play begins for this actor.
	和訳
		このアクタの Play が始まるときのオーバーライド可能なネイティブイベント。
	*/
	virtual void BeginPlay()override;

	/*
	by Epic
		Overridable function called whenever this actor is being removed from a level
	和訳
		このアクタがレベルから削除するたびに呼び出されるのオーバーライド可能な関数。
	*/
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

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

	// End of IGHOInteractable interface

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void Turn(float Rate);
	void LookUp(float Rate);

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
		ただ、使用する前にポインタが有効かどうかをテストする必要があります。
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


//public:
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	AGHOWeapon* GetCurrentWeapon() const{return nullptr;};
//
//	FName GetWeaponAttachPoint() { return FName(); };

public:
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
	AGHOWeapon* GetCurrentWeapon() const;

	FName GetWeaponAttachPoint() const;

private:
	UFUNCTION()
	void OnRep_Inventory();

	
	//解説
	//	引数付き OnRep 
	//	https://answers.unrealengine.com/questions/110910/view.html
	//	https://forums.unrealengine.com/t/replicatedusing-with-arguments-shootergame-epicstaff-help/39773
	//		>The properties are updated first, then the rep notify is called. So by the time you get the rep notify, the property will be up to date.
	//		>If you want the old value, you can add the value as a parameter to the rep notify, and it will pass the old value in that case.
	//	和訳
	//		プロパティが先に更新されてから rep notify が呼ばれます。そのため、 rep notify が呼ばれる頃には、プロパティは最新の状態になっています。
	//		古い値が必要な場合は、 rep notify にパラメータとして値を追加すれば、その場合は古い値が渡されます。
	UFUNCTION()
	void OnRep_CurrentWeapon(AGHOWeapon* LastWeapon);

	/*
	by GASShooter
		The CurrentWeapon is only automatically replicated to simulated clients.
		The autonomous client can use this to request the proper CurrentWeapon from the server when it knows it may be out of sync with it from predictive client-side changes.
	和訳
		CurrentWeapon は simulated clients のみに自動的にレプリケーションされます。
		autonomous client は、 predictive （予測的）なクライアントサイドの変更により、サーバーと同期が取れなくなる可能性があると知った場合、これを使用してサーバーに適切な CurrentWeapon を要求することができます。
	*/
	UFUNCTION(Server, Reliable)
	void RPCServerSyncCurrentWeapon();
	void RPCServerSyncCurrentWeapon_Implementation();
	bool RPCServerSyncCurrentWeapon_Validate();

	/*
	by GASShooter
		The CurrentWeapon is only automatically replicated to simulated clients.
		Use this function to manually sync the autonomous client's CurrentWeapon when we're ready to.
		This allows us to predict weapon changes (changing weapons fast multiple times in a row so that the server doesn't replicate and clobber our CurrentWeapon).
	和訳
		CurrentWeapon は simulated clients のみに自動的にレプリケーションされます。
		この関数を使用して、準備ができたら、 autonomous client の CurrentWeapon を手動で同期します。
		これにより、武器の変更を predict （予測）することができます（素早く何度も連続して武器を変更してもサーバーは CurrentWeapon はレプリケーションせず、壊しません）。
	*/
	UFUNCTION(Client, Reliable)
	void RPCClientSyncCurrentWeapon(AGHOWeapon* InWeapon);
	void RPCClientSyncCurrentWeapon_Implementation(AGHOWeapon* InWeapon);
	bool RPCClientSyncCurrentWeapon_Validate(AGHOWeapon* InWeapon);

	UFUNCTION(Server, Reliable)
	void RPCServerEquipWeapon(AGHOWeapon* NewWeapon);
	void RPCServerEquipWeapon_Implementation(AGHOWeapon* NewWeapon);
	bool RPCServerEquipWeapon_Validate(AGHOWeapon* NewWeapon);

	void SetCurrentWeapon(AGHOWeapon* NewWeapon, AGHOWeapon* LastWeapon);

	/*
	by GASShooter
		Unequips the specified weapon. Used when OnRep_CurrentWeapon fires.
	和訳
		指定された武器を装備解除します。 OnRep_CurrentWeapon が発火した時に使用されます。
	*/
	void UnEquipWeapon(AGHOWeapon* WeaponToUnEquip);

	/*
	by GASShooter
		Unequips the current weapon. Used if for example we drop the current weapon.
	和訳
		現在の武器を装備解除します。現在使用している武器を落とした場合などに使用します。
	*/
	void UnEquipCurrentWeapon();

	/*
	by GASShooter
		Adds a new weapon to the inventory.
		Returns false if the weapon already exists in the inventory, true if it's a new weapon.
	和訳
		新しい武器をインベントリに追加します。
		インベントリに既にその武器がある場合は false を、新しい武器の場合は true を返します。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
	bool AddWeaponToInventory(AGHOWeapon* NewWeapon, bool bEquipWeapon = false);

	/*
	by GASShooter
		Removes a weapon from the inventory.
		Returns true if the weapon exists and was removed. False if the weapon didn't exist in the inventory.
	和訳
		インベントリから武器を削除します。
		武器が存在し、削除された場合は true を返します。 武器がインベントリに存在していない場合は fakse を返します。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
	bool RemoveWeaponFromInventory(AGHOWeapon* WeaponToRemove);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
	void RemoveAllWeaponsFromInventory();

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
	void EquipWeapon(AGHOWeapon* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
	int32 GetPrimaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
	int32 GetMaxPrimaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
	int32 GetPrimaryReserveAmmo() const;


	UFUNCTION()
	virtual void CurrentWeaponPrimaryClipAmmoChanged(int32 OldPrimaryClipAmmo, int32 NewPrimaryClipAmmo);

	UFUNCTION()
	virtual void CurrentWeaponSecondaryClipAmmoChanged(int32 OldSecondaryClipAmmo, int32 NewSecondaryClipAmmo);

	// Attribute changed callbacks
	virtual void CurrentWeaponPrimaryReserveAmmoChanged(const FOnAttributeChangeData& Data);
	virtual void CurrentWeaponSecondaryReserveAmmoChanged(const FOnAttributeChangeData& Data);

	// Tag changed callbacks
	virtual void WeaponChangingDelayReplicationTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	// Server spawns default inventory
	void SpawnDefaultInventory();

	bool DoesWeaponExistInInventory(AGHOWeapon* InWeapon);

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|GHOHeroCharacter")
	FName WeaponAttachPoint;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASHandsOn|Inventory")
	TArray<TSubclassOf<AGHOWeapon>> DefaultInventoryWeaponClasses;

private:
	FGameplayTag NoWeaponTag;
	FGameplayTag WeaponChangingDelayReplicationTag;
	FGameplayTag WeaponAmmoTypeNoneTag;
	FGameplayTag WeaponAbilityTag;

	FGameplayTag CurrentWeaponTag;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	FGHOHeroInventory Inventory;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	AGHOWeapon* CurrentWeapon;

	//UPROPERTY()
	//class UGHOAttributeSetAmmo* AttributeSetAmmo;
	TWeakObjectPtr<class UGHOAttributeSetAmmo> AttributeSetAmmo;

	/*
	by GASShooter
		Set to true when we change the weapon predictively and flip it to false when the Server replicates to confirm.
		We use this if the Server refused a weapon change ability's activation to ask the Server to sync the client back up with the correct CurrentWeapon.
	和訳
		predictively （予測的）に武器を変更する場合は true を設定し、サーバーが確認のためにレプレケーションする場合は false に反転させます。
		サーバーが武器変更アビリティの発動を拒否した場合に、サーバーに正しい CurrentWeapon でクライアントを同期し直すように依頼するために使用します。
	*/
	bool bChangedWeaponLocally;

	// Attribute changed delegate handles
	FDelegateHandle PrimaryReserveAmmoChangedDelegateHandle;
	FDelegateHandle SecondaryReserveAmmoChangedDelegateHandle;

	// Tag changed delegate handles
	FDelegateHandle WeaponChangingDelayReplicationTagChangedDelegateHandle;

	//protected:
//	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASHandsOn|Inventory")
//	TArray<TSubclassOf<AGHOWeapon>> DefaultInventoryWeaponClasses;
//
//	// Adds a new weapon to the inventory.
//	// Returns false if the weapon already exists in the inventory, true if it's a new weapon.
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	bool AddWeaponToInventory(AGHOWeapon* NewWeapon, bool bEquipWeapon = false);
//
//	// Removes a weapon from the inventory.
//	// Returns true if the weapon exists and was removed. False if the weapon didn't exist in the inventory.
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	bool RemoveWeaponFromInventory(AGHOWeapon* WeaponToRemove);
//
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	void RemoveAllWeaponsFromInventory();
//
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	void EquipWeapon(AGHOWeapon* NewWeapon);
//
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	virtual void NextWeapon();
//
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	virtual void PreviousWeapon();
//
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	int32 GetPrimaryClipAmmo() const;
//
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	int32 GetMaxPrimaryClipAmmo() const;
//
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	int32 GetPrimaryReserveAmmo() const;
//
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	int32 GetSecondaryClipAmmo() const;
//
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	int32 GetMaxSecondaryClipAmmo() const;
//
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	int32 GetSecondaryReserveAmmo() const;
//
//	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Inventory")
//	int32 GetNumWeapons() const;
//
//private:
//
//
//
//	void SetupStartupPerspective();
//
//	bool DoesWeaponExistInInventory(AGHOWeapon* InWeapon);
//
//
//
//
//	void OnAbilityActivationFailed(const UGameplayAbility* FailedAbility, const FGameplayTagContainer& FailTags);
	
};
