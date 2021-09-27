// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "GASHandsOn.h"
#include "GHOWeapon.generated.h"

class UAnimMontage;
class UPaperSprite;
class USkeletalMeshComponent;

class AGHOGATA_LineTrace;
class AGHOGATA_SphereTrace;
class AGHOHeroCharacterBase;
class UGHOAbilitySystemComponent;
class UGHOGameplayAbility;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponAmmoChangedDelegate, int32, OldValue, int32, NewValue);


UCLASS()
class GASHANDSON_API AGHOWeapon : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGHOWeapon();

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
		Event when this actor overlaps another actor, for example a player walking into a trigger.
		For events when objects have a blocking collision, for example a player hitting a wall, see 'Hit' events.
		@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
	和訳
		このアクターげ他のアクターと重なった時のイベント。例えば、プレイヤーがトリガーの中に入っていくなど。
		プレイヤーが壁にぶつかるなど、オブジェクトがブロックコリジョンを起こした時のイベントについては、「 Hit　」イベントを参照してください。
		@note オーバーラップイベントを生成するには、このアクターともう一方のアクターの両方にあるコンポーネントの bGenerateOverlapEvents が true に設定されている必要があります。
	by GASShooter
		Pickup on touch
	和訳
		触れることで拾う
	*/
	virtual void NotifyActorBeginOverlap(class AActor* Other) override;

	/*
	by Epic
		Called on the actor right before replication occurs.
		Only called on Server, and for autonomous proxies if recording a Client Replay.
	和訳
		レプリケーションが発生する直前にアクターに呼び出されます。
		サーバーのみで呼び出され、クライアントリプレイを記録する場合は、 autonomous proxies でのみ呼び出されます。
	*/
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

protected:
	/*
	by Epic
		Overridable native event for when play begins for this actor.
	和訳
		このアクタの Play が始まるときのオーバーライド可能なネイティブイベント。
	*/
	virtual void BeginPlay()override;

	/**
	by Epic
		Overridable function called whenever this actor is being removed from a level
	和訳
		このアクタがレベルから削除されるたびに呼び出されるオーバーライド可能な関数。
	*/
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// End of AActor interface


	// IAbilitySystemInterface interface
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// End of IAbilitySystemInterface interface




public:	
	void SetOwningCharacter(AGHOHeroCharacterBase* InOwningCharacter);
	void SetSpawnWithCollision(bool bInSpawnWithCollision);

	/**
	by GASShooter
		Called when the player equips this weapon
	和訳
		プレイヤーがこの武器を装備した時に呼び出される。
	*/
	virtual void Equip();

	/**
	by GASShooter
		Called when the player unequips this weapon
	和訳
		プレイヤーがこの武器を装備解除した時に呼び出される。
	*/
	virtual void UnEquip();

	virtual void AddAbilities();

	virtual void RemoveAbilities();

	virtual int32 GetAbilityLevel(EGHOAbilityInputID AbilityID);

	/**
	by GASShooter
		Resets things like fire mode to default
	和訳
		ファイアモードなどをデフォルトに戻す。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	virtual void ResetWeapon();

	UFUNCTION(NetMulticast, Reliable)
	void OnDropped(FVector NewLocation);
	virtual void OnDropped_Implementation(FVector NewLocation);
	virtual bool OnDropped_Validate(FVector NewLocation);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	virtual int32 GetPrimaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	virtual int32 GetMaxPrimaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	virtual int32 GetSecondaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	virtual int32 GetMaxSecondaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	virtual void SetPrimaryClipAmmo(int32 NewPrimaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	virtual void SetMaxPrimaryClipAmmo(int32 NewMaxPrimaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	virtual void SetSecondaryClipAmmo(int32 NewSecondaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	virtual void SetMaxSecondaryClipAmmo(int32 NewMaxSecondaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	TSubclassOf<class UGHOHUDReticle> GetPrimaryHUDReticleClass() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	virtual bool HasInfiniteAmmo() const;

#if 0
	//TODO: 1st / 3rd person view
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Animation")
	UAnimMontage* GetEquip1PMontage() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Animation")
	UAnimMontage* GetEquip3PMontage() const;

#else
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Animation")
	UAnimMontage* GetEquipMontage() const;

#endif
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Audio")
	class USoundCue* GetPickupSound() const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GHOWeapon")
	FText GetDefaultStatusText() const;

	/**
	by GASShooter
		Getter for LineTraceTargetActor. Spawns it if it doesn't exist yet.
	和訳
		LineTraceTargetActor のゲッター。存在していなければスポーンします。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Targeting")
	AGHOGATA_LineTrace* GetLineTraceTargetActor();

	/**
	by GASShooter
		Getter for SphereTraceTargetActor. Spawns it if it doesn't exist yet.
	和訳
		SphereTraceTargetActor のゲッター。存在していなければスポーンします。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Targeting")
	AGHOGATA_SphereTrace* GetSphereTraceTargetActor();


#if 0
	//TODO: 1st / 3rd person view
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|GHOWeapon")
	virtual USkeletalMeshComponent* GetWeaponMesh1P() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|GHOWeapon")
	virtual USkeletalMeshComponent* GetWeaponMesh3P() const;

#else
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|GHOWeapon")
	virtual USkeletalMeshComponent* GetWeaponMesh() const;

#endif

private:
	/**
	by GASShooter
		Called when the player picks up this weapon
	和訳
		プレイヤーがこの武器を手に入れた時に呼び出される。
	*/
	virtual void PickUpOnTouch(AGHOHeroCharacterBase* InCharacter);

	UFUNCTION()
	virtual void OnRep_PrimaryClipAmmo(int32 OldPrimaryClipAmmo);

	UFUNCTION()
	virtual void OnRep_MaxPrimaryClipAmmo(int32 OldMaxPrimaryClipAmmo);

	UFUNCTION()
	virtual void OnRep_SecondaryClipAmmo(int32 OldSecondaryClipAmmo);

	UFUNCTION()
	virtual void OnRep_MaxSecondaryClipAmmo(int32 OldMaxSecondaryClipAmmo);

public:
	/**
	by GASShooter
		This tag is primarily used by the first person Animation Blueprint to determine which animations to play
		(Rifle vs Rocket Launcher)
	和訳
		このタグは主に一人称視点のアニメーションブループリントが再生するアニメーションを決定するために使用されます。
		(ライフル vs ロケットランチャー)
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASHandsOn|GHOWeapon")
	FGameplayTag WeaponTag;

	/**
	by GASShooter
		UI HUD Primary Icon when equipped. Using Sprites because of the texture atlas from ShooterGame.
	和訳
		装備時の UI HUD プライマリアイコン。 ShooterGame のテクスチャアトラスを使用しているため、スプライトを使用しています。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|UI")
	UPaperSprite* PrimaryIcon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|UI")
	UPaperSprite* SecondaryIcon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASHandsOn|GHOWeapon")
	FGameplayTag PrimaryAmmoType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASHandsOn|GHOWeapon")
	FGameplayTag SecondaryAmmoType;


	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "GASHandsOn|GHOWeapon")
	FText WeaponName;

	/**
	by GASShooter
		Things like fire mode for rifle
	和訳
		ライフルの射撃モードなど。
	*/
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "GASHandsOn|GHOWeapon")
	FText StatusText;


	UPROPERTY(BlueprintAssignable, Category = "GASHandsOn|GHOWeapon")
	FWeaponAmmoChangedDelegate OnPrimaryClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "GASHandsOn|GHOWeapon")
	FWeaponAmmoChangedDelegate OnMaxPrimaryClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "GASHandsOn|GHOWeapon")
	FWeaponAmmoChangedDelegate OnSecondaryClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "GASHandsOn|GHOWeapon")
	FWeaponAmmoChangedDelegate OnMaxSecondaryClipAmmoChanged;


protected:
	/**
	by GASShooter
		Whether or not to spawn this weapon with collision enabled (pickup mode).
		Set to false when spawning directly into a player's inventory or true when spawning into the world in pickup mode.
	和訳
		この武器をコリジョンを有効にしてスポーンするかどうか（ピックアップモード）。
		プレイヤーのインベントリに直接スポーンする場合は false に、ピックアップモードでワールドにスポーンする場合は true に設定されます。
	*/
	UPROPERTY(BlueprintReadWrite)
	bool bSpawnWithCollision;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GASHandsOn|GHOWeapon")
	FGameplayTagContainer RestrictedPickupTags;
	
	/**
	by GASShooter
		UI HUD Primary Clip Icon when equipped
	和訳
		装備時の UI HUD プライマリクリップアイコン。 
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|UI")
	UPaperSprite* PrimaryClipIcon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|UI")
	UPaperSprite* SecondaryClipIcon;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "GASHandsOn|GHOWeapon")
	FGameplayTag FireMode;


	/**
	by GASShooter
		How much ammo in the clip the gun starts with
	和訳
		銃の最初のクリップに入っている弾薬の量。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_PrimaryClipAmmo, Category = "GASHandsOn|GHOWeapon|Ammo")
	int32 PrimaryClipAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_MaxPrimaryClipAmmo, Category = "GASHandsOn|GHOWeapon|Ammo")
	int32 MaxPrimaryClipAmmo;

	/**
	by GASShooter
		How much ammo in the clip the gun starts with. Used for things like rifle grenades.
	和訳
		銃の最初のクリップに入っている弾薬の量。ライフルグレネードなどに使用します。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_SecondaryClipAmmo, Category = "GASHandsOn|GHOWeapon|Ammo")
	int32 SecondaryClipAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_MaxSecondaryClipAmmo, Category = "GASHandsOn|GHOWeapon|Ammo")
	int32 MaxSecondaryClipAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|GHOWeapon|Ammo")
	bool bInfiniteAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|UI")
	TSubclassOf<class UGHOHUDReticle> PrimaryHUDReticleClass;

	/**
	by GASShooter
		Collision capsule for when weapon is in pickup mode
	和訳
		武器がピックアップモードのときのコリジョンカプセル。
	*/
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* CollisionComp;

#if 0
	//TODO: 1st / 3rd person view
	UPROPERTY(VisibleAnywhere, Category = "GASHandsOn|GHOWeapon")
	USkeletalMeshComponent* WeaponMesh1P;

	UPROPERTY(VisibleAnywhere, Category = "GASHandsOn|GHOWeapon")
	USkeletalMeshComponent* WeaponMesh3P;

	/**
	by GASShooter
		Relative Location of weapon 3P Mesh when in pickup mode
		1P weapon mesh is invisible so it doesn't need one
	和訳
		ピックアップモードでの武器の 3P メッシュの相対位置。
		1P 武器のメッシュは不可視なので不要。
	*/
	UPROPERTY(EditDefaultsOnly, Category = "GASHandsOn|GHOWeapon")
	FVector WeaponMesh3PickupRelativeLocation;

	/**
	by GASShooter
		Relative Location of weapon 1P Mesh when equipped
	和訳
		装備時の武器の 1P メッシュの相対位置。
	*/
	UPROPERTY(EditDefaultsOnly, Category = "GASHandsOn|GHOWeapon")
	FVector WeaponMesh1PEquippedRelativeLocation;

	/**
	by GASShooter
		Relative Location of weapon 3P Mesh when equipped
	和訳
		装備時の武器の 3P メッシュの相対位置。
	*/
	UPROPERTY(EditDefaultsOnly, Category = "GASHandsOn|GHOWeapon")
	FVector WeaponMesh3PEquippedRelativeLocation;

#else
	UPROPERTY(VisibleAnywhere, Category = "GASHandsOn|GHOWeapon")
	USkeletalMeshComponent* WeaponMesh;

	/**
	by GASShooter
		Relative Location of weapon Mesh when in pickup mode
	和訳
		ピックアップモードでの武器のメッシュの相対位置。
	*/
	UPROPERTY(EditDefaultsOnly, Category = "GASHandsOn|GHOWeapon")
	FVector WeaponMeshickupRelativeLocation;

	/**
	by GASShooter
		Relative Location of weapon Mesh when equipped
	和訳
		装備時の武器のメッシュの相対位置。
	*/
	UPROPERTY(EditDefaultsOnly, Category = "GASHandsOn|GHOWeapon")
	FVector WeaponMeshEquippedRelativeLocation;

#endif
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "GASHandsOn|GHOWeapon")
	AGHOHeroCharacterBase* OwningCharacter;

	UPROPERTY(EditAnywhere, Category = "GASHandsOn|GHOWeapon")
	TArray<TSubclassOf<UGHOGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadOnly, Category = "GASHandsOn|GHOWeapon")
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|GHOWeapon")
	FGameplayTag DefaultFireMode;

	/**
	by GASShooter
		Things like fire mode for rifle
	和訳
		ライフルの射撃モードなど。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|GHOWeapon")
	FText DefaultStatusText;

#if 0
	//TODO: 1st / 3rd person view
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Animation")
	UAnimMontage* Equip1PMontage;

	UPROPERTY(BlueprintReadonly, EditAnywhere, Category = "GASHandsOn|Animation")
	UAnimMontage* Equip3PMontage;

#else
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Animation")
	UAnimMontage* EquipMontage;

#endif
	/**
	by GASShooter
		Sound played when player picks it up
	和訳
		拾った時に鳴らす音。
	*/
	UPROPERTY(EditDefaultsOnly, Category = "GASHandsOn|Audio")
	class USoundCue* PickupSound;

private:
	UPROPERTY()
	UGHOAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	AGHOGATA_LineTrace* LineTraceTargetActor;

	UPROPERTY()
	AGHOGATA_SphereTrace* SphereTraceTargetActor;

private:
	/**
	by GASShooter
		Cache tags
	和訳
		タグのキャッシュ
	*/
	FGameplayTag WeaponPrimaryInstantAbilityTag;
	FGameplayTag WeaponSecondaryInstantAbilityTag;
	FGameplayTag WeaponAlternateInstantAbilityTag;
	FGameplayTag WeaponIsFiringTag;
};
