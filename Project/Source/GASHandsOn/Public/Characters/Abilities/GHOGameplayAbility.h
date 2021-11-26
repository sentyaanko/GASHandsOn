// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASHandsOn.h"
#include "Characters/Types/GHOGameplayEffectTypes.h"
#include "GHOGameplayAbility.generated.h"

#if 0 //for multiple USkeletalMeshComponents on the AvatarActor
USTRUCT()
struct GASHANDSON_API FAbilityMeshMontage
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class USkeletalMeshComponent* Mesh;

	UPROPERTY()
	class UAnimMontage* Montage;

	FAbilityMeshMontage() : Mesh(nullptr), Montage(nullptr)
	{
	}

	FAbilityMeshMontage(class USkeletalMeshComponent* InMesh, class UAnimMontage* InMontage)
		: Mesh(InMesh), Montage(InMontage)
	{
	}
};
#endif

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGHOGameplayAbility();

	// UGameplayAbility interface
public:
	/*
	by Epic
		Called when the avatar actor is set/changes
	和訳
		Avator Actor が設定/変更されたときに呼び出される。
	by GASDocumentation
		If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
		Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	和訳
		アビリティに「ActivateAbilityOnGranted」が設定されている場合は、ここで与えられたときにすぐに起動する。
		エピックのコメント : プロジェクトでは、パッシブを開始したり、その他の「BeginPlay」タイプのロジックをここで行うことができます。
	*/
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	/**
	by Epic
		const function to see if ability is activatable. Callable by UI etc

		Returns true if this ability can be activated right now. Has no side effects 
	和訳
		アビリティが起動可能かどうかを確認するための const 関数です。 UI などで呼び出し可能です。

		このアビリティが今すぐにでも起動できる場合は true を返します。副作用はありません。
	*/
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	/**
	by Epic
		Checks cost. returns true if we can pay for the ability. False if not
	和訳
		コストをチェックします。アビリティ用のコストが支払い可能なら true を返します。そうでなければ false を返します。
	*/
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	/**
	by Epic
		Applies the ability's cost to the target
	和訳
		アビリティのコストをターゲットに適用する。
	*/
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	// End of UGameplayAbility interface

public:
	bool IsActivateOnInput()const { return bActivateOnInput; }

	/*
	by GASShooter
		Same as calling K2_EndAbility. Meant for use with batching system to end the ability externally.
	和訳
		K2_EndAbility の呼び出しと同じです。
		アビリティを外部で終了させるバッチシステムでの使用を目的としています。
	*/
	virtual void ExternalEndAbility();

protected:
	/*
	by GASShooter
		Expose GetSourceObject to Blueprint. 
		Retrieves the SourceObject associated with this ability. 
		Callable on non instanced abilities.
	和訳
		GetSourceObject をブループリントに公開します。
		このアビリティに関連する SourceObject を取得します。
		non instanced abilities で呼び出し可能です。
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|Ability", meta = (DisplayName = "Get Source Object"))
	UObject* K2_GetSourceObject(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;

	/*
	by GASShooter
		Is the player's input currently pressed? Only works if the ability is bound to input.
	和訳
		プレイヤーの入力は現在押されているか？アビリティが入力にバインドされている場合のみ動作します。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability")
	/*virtual*/ bool IsInputPressed() const;

	/*
	by GASShooter
		Attempts to activate the given ability handle and batch all RPCs into one. 
		This will only batch all RPCs that happen in one frame. 
		Best case scenario we batch ActivateAbility, SendTargetData, and EndAbility into one RPC instead of three.
	和訳
		与えられたアビリティハンドルを起動し、全てのRPCを一つにまとめようとします。
		これは、 1 フレーム内に発生する全ての RPC を一括して処理します。
		最良のシナリオでは、 ActivateAility, SendTargetData, EndAbility を 3 つの RPC ではなく 1 つの RPC にまとめます。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability")
	/*virtual*/ bool BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately);

	/*
	by GASShooter
		Allows C++ and Blueprint abilities to override how cost is checked in case they don't use a GE like weapon ammo
	和訳
		C++ 及びブループリントのアビリティで、武器の弾薬のように、 GEを使用しない場合のコストのチェック方法を上書きできるようにします。
	解説
		BlueprintImplementableEvent を使用すると、ブループリントでオーバーライドしない場合、戻り値が false として扱われる。
		C++ の実装は無いが、、 BlueprintImplementableEvent ではなく、 BlueprintNativeEvent を使用する。
	*/
	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Ability")
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GASHandsOn|Ability")
	bool GHOCheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;
	virtual bool GHOCheckCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const;

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability")
	virtual void SetHUDReticle(TSubclassOf<class UGHOHUDReticle> ReticleClass);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability")
	virtual void ResetHUDReticle();

	TSubclassOf<class UGHOHUDReticle> GetCurrentWeaponHUDReticle();

	/*
	by GASShooter
		Allows C++ and Blueprint abilities to override how cost is applied in case they don't use a GE like weapon ammo
	和訳
		C++ 及びブループリントのアビリティで、武器の弾薬のように、 GEを使用しない場合のコストの適用方法を上書きできるようにします。
	解説
		BlueprintNativeEvent / BlueprintImplementableEvent については前述のとおり。
	*/
	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Ability")
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GASHandsOn|Ability")
	void GHOApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const;
	virtual void GHOApplyCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const;

	/*
	by GASShooter
		Search for and make a gameplay effect container spec to be applied later, from the EffectContainerMap
	和訳
		GameplayEffectContainerSpec を EffectContainerMap から検索、（なければ）作成して、後から適用できるようにする、
	解説
		この関数は Epic のサンプルプロジェクトである ActionRPG の RPGGameplayAbility で作成されている機能を元に
		GASShooter の GSGameplayAbility で拡張されたものです。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability", meta = (AutoCreateRefTerm = "EventData"))
	/*virtual*/ FGHOGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/*
	by GASShooter
		Applies a gameplay effect container spec that was previously created
	和訳
		以前に作成した、GameplayEffectContainerSpec を適用する。
	解説
		この関数は Epic のサンプルプロジェクトである ActionRPG の RPGGameplayAbility で作成されている機能を元に
		GASShooter の GSGameplayAbility で拡張されたものです。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability")
	/*virtual*/ TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FGHOGameplayEffectContainerSpec& ContainerSpec);

private:
	/*
	by GASShooter
		Make gameplay effect container spec to be applied later, using the passed in container
	和訳
		受け取ったコンテナを使用して、 GameplayEffectContainerSpec を作成し、後から適用できるようにする。
	解説
		この関数は Epic のサンプルプロジェクトである ActionRPG の RPGGameplayAbility で作成されている機能を元に
		GASShooter の GSGameplayAbility で拡張されたものです。
		GASShooter では Blueprint に公開していましたが、使用されていないため、 GASHandsOn では公開せず、 private に変更しています。
	*/
	//UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability", meta = (AutoCreateRefTerm = "EventData"))
	/*virtual*/ FGHOGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FGHOGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

public:
	/*
	by GASDocumentation
		Abilities with this set will automatically activate when the input is pressed
	和訳
		このセットのアビリティは入力が押されると自動的にアクティブになります。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Ability")
	EGHOAbilityInputID AbilityInputID = EGHOAbilityInputID::None;

	/*
	by GASDocumentation
		Value to associate an ability with an slot without tying it to an automatically activated input.
		Passive abilities won't be tied to an input so we need a way to generically associate abilities with slots.
	和訳
		アビリティを自動的にアクティブ化された入力に結び付けずに関連付ける値。
		パッシブアビリティは入力に関連付けられないため、アビリティをスロットに一般的に関連付ける方法が必要です。
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASHandsOn|Ability")
	EGHOAbilityInputID AbilityID = EGHOAbilityInputID::None;

protected:
	/*
	by GASDocumentation
		Tells an ability to activate immediately when its granted. 
		Used for passive abilities and abilities forced on others.
	和訳
		アビリティが付与されたときにすぐに発動するようにします。
		パッシブアビリティや他の人に強制するアビリティに使用します。
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|Ability")
	bool bActivateAbilityOnGranted = false;

	/*
	by GASShooter
		If true, this ability will activate when its bound input is pressed. 
		Disable if you want to bind an ability to an input but not have it activate when pressed.
	和訳
		true の場合、このアビリティはバインドされた入力が押された時に起動します。
		入力にアビリティを割り当てたいが、押されても起動しないようにしたい場合は無効にします。
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|Ability")
	bool bActivateOnInput;

	/*
	by GASShooter
		If true, only activate this ability if not interacting with something via GA_Interact.
	和訳
		true の場合、 GA_Interact を介して何かとインタラクトしていない場合にのみ、このアビリティを起動します。
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|Ability")
	bool bCannotActivateWhileInteracting;

	/*
	by GASShooter
		If true, only activate this ability if the weapon that granted it is the currently equipped weapon.
	和訳
		true の場合、 このアビリティを付与した武器が現在装備されているものである場合のみこのアビリティを発動します。
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|Ability")
	bool bSourceObjectMustEqualCurrentWeaponToActivate;

	/*
	by GASShooter
		Map of gameplay tags to gameplay effect containers
	和訳
		GameplayTag から GameplayEffectContainer へのマップ
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASHandsOn|GameplayEffects")
	TMap<FGameplayTag, FGHOGameplayEffectContainer> EffectContainerMap;

#if 0 //for multiple USkeletalMeshComponents on the AvatarActor
public:
	// ----------------------------------------------------------------------------------------------------------------
	/**
	by GASShooter
		Animation Support for multiple USkeletalMeshComponents on the AvatarActor
	和訳
		AvatarActor で複数の USkeletalMeshComponents を持つもののアニメーションのサポートです。
	解説
		これらの関数群は UGameplayAbility で定義されている、名前に ForMesh がつかない関数群を拡張したものです。
		全てを使用しているわけではないですが、関連している機能としてほぼ一式実装されています。
		機能の違いから、(FindAbillityMeshMontage / MontageStopForAllMeshes)が追加されています。
		関数の説明コメントもほぼ Epic のもののままです（複数形に直されているぐらい）。
	*/

	/**
	by Epic
		Returns the currently playing montage for this ability, if any
	和訳
		このアビリティで現在再生されているモンタージュがあれば、それを返す。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Animation")
	UAnimMontage* GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh);

	/**
	by Epic
		Call to set/get the current montage from a montage task. Set to allow hooking up montage events to ability events
	和訳
		モンタージュタスクから現在のモンタージュを設定/取得するために呼び出します。モンタージュイベントをアビリティイベントに接続できるように設定します。
	*/
	virtual void SetCurrentMontageForMesh(USkeletalMeshComponent* InMesh, class UAnimMontage* InCurrentMontage);

protected:
	/**
	by Epic
		Active montages being played by this ability
	和訳
		このアビリティで再生されるアクティブなモンタージュ
	*/
	UPROPERTY()
	TArray<FAbilityMeshMontage> CurrentAbilityMeshMontages;

	bool FindAbillityMeshMontage(USkeletalMeshComponent* InMesh, FAbilityMeshMontage& InAbilityMontage);

	/**
	by Epic
		Immediately jumps the active montage to a section
	和訳
		アクティブなモンタージュをすぐにセクションにジャンプさせる
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability|Animation")
	void MontageJumpToSectionForMesh(USkeletalMeshComponent* InMesh, FName SectionName);

	/**
	by Epic
		Sets pending section on active montage
	和訳
		アクティブなモンタージュで保留中のセクションを設定
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability|Animation")
	void MontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, FName FromSectionName, FName ToSectionName);

	/**
	by Epic
		Stops the current animation montage.
		@param OverrideBlendOutTime If >= 0, will override the BlendOutTime parameter on the AnimMontage instance
	和訳
		現在のアニメーションモンタージュを停止します。
		@param OverrideBlendOutTime 0 より大きい場合、 AnimMontage インスタンスの BlendOutTime パラメータを上書きします。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability|Animation", Meta = (AdvancedDisplay = "OverrideBlendOutTime"))
	void MontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime = -1.0f);

	/**
	by Epic
		Stops all currently animating montages
		@param OverrideBlendOutTime If >= 0, will override the BlendOutTime parameter on the AnimMontage instance
	和訳
		現在のアニメーションモンタージュを全て停止します。
		@param OverrideBlendOutTime 0 より大きい場合、 AnimMontage インスタンスの BlendOutTime パラメータを上書きします。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Ability|Animation", Meta = (AdvancedDisplay = "OverrideBlendOutTime"))
	void MontageStopForAllMeshes(float OverrideBlendOutTime = -1.0f);

	// End of `Animation Support for multiple USkeletalMeshComponents on the AvatarActor`
	// ----------------------------------------------------------------------------------------------------------------
#endif
};
