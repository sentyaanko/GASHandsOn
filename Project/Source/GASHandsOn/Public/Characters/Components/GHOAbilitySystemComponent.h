// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GHOAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UGHOAbilitySystemComponent*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);

#if 0 //for multiple USkeletalMeshComponents on the AvatarActor
class USkeletalMeshComponent;

/**
by Epic
	Data about montages that were played locally (all montages in case of server. predictive montages in case of client). Never replicated directly.
和訳
	ローカルで再生されたモンタージュのデータ（サーバーの場合は全モンタージュ、クライアントの場合は predictive （予測）モンタージュ ）。直接複製されることはありません。
解説
	Engine\Plugins\Runtime\GameplayAbilities\Source\GameplayAbilities\Public\Abilities\GameplayAbilityTypes.h
	で定義されている FGameplayAbilityLocalAnimMontage を拡張するための構造体。
*/
USTRUCT()
struct GASHANDSON_API FGameplayAbilityLocalAnimMontageForMesh
{
	GENERATED_BODY();

public:
	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	UPROPERTY()
	FGameplayAbilityLocalAnimMontage LocalMontageInfo;

	FGameplayAbilityLocalAnimMontageForMesh() : Mesh(nullptr), LocalMontageInfo()
	{
	}

	FGameplayAbilityLocalAnimMontageForMesh(USkeletalMeshComponent* InMesh)
		: Mesh(InMesh), LocalMontageInfo()
	{
	}

	FGameplayAbilityLocalAnimMontageForMesh(USkeletalMeshComponent* InMesh, FGameplayAbilityLocalAnimMontage& InLocalMontageInfo)
		: Mesh(InMesh), LocalMontageInfo(InLocalMontageInfo)
	{
	}
};

/**
by Epic
	Data about montages that is replicated to simulated clients.
和訳
	simulated clients にレプリケーションされる、モンタージュに関するデータ。
解説
	Engine\Plugins\Runtime\GameplayAbilities\Source\GameplayAbilities\Public\Abilities\GameplayAbilityTypes.h
	で定義されている FGameplayAbilityRepAnimMontage を拡張するための構造体。
*/
USTRUCT()
struct GASHANDSON_API FGameplayAbilityRepAnimMontageForMesh
{
	GENERATED_BODY();

public:
	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	UPROPERTY()
	FGameplayAbilityRepAnimMontage RepMontageInfo;

	FGameplayAbilityRepAnimMontageForMesh() : Mesh(nullptr), RepMontageInfo()
	{
	}

	FGameplayAbilityRepAnimMontageForMesh(USkeletalMeshComponent* InMesh)
		: Mesh(InMesh), RepMontageInfo()
	{
	}
};
#endif

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

#if 0 //for multiple USkeletalMeshComponents on the AvatarActor
	// UObject interface
public:
	/*
	by Epic
		Returns properties that are replicated for the lifetime of the actor channel
	和訳
		アクターチャンネルのライフタイム中にレプリケーションされるプロパティを返します。
	*/
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;

	// End of UObject interface

	// UActorComponent interface
public:
	/**
	by Epic
		Function called every frame on this ActorComponent. 
		Override this function to implement custom logic to be executed every frame.
		Only executes if the component is registered, and also PrimaryComponentTick.bCanEverTick must be set to true.
	
		@param DeltaTime - The time since the last tick.
		@param TickType - The kind of tick this is, for example, are we paused, or 'simulating' in the editor
		@param ThisTickFunction - Internal tick function struct that caused this to run
	和訳
		この ActorComponent でフレームごとに呼び出される関数です。
		この関数をオーバーライドして、フレームごとに実行されるカスタムロジックを実装します。
		コンポーネントが登録されていて、 PrimaryComponentTick.bCanEverTick が true に設定されている場合のみ実行されます。

		@param DeltaTime - 最後の tick からの時間。
		@param TickType - これがどのような種類の tick なのか。たとえば、一時停止しているのか、エディタでシミュレートしているか、など。
		@param ThisTickFunction - これを実行する原因となった内部の tick function 構造体。
	*/
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	// End of UActorComponent interface


	// UGameplayTasksComponent interface
public:
	/*
	by Epic
		retrieves information whether this component should be ticking taken current activity into consideration
	和訳
		現在のアクティビティを考慮して、このコンポーネントが Tick するべきかどうかの情報を取得します。
	*/
	virtual bool GetShouldTick() const override;

	// End of UGameplayTasksComponent interface
#endif
	
	// UAbilitySystemComponent interface
public:
	/** Called to handle ability bind input */
	virtual void AbilityLocalInputPressed(int32 InputID)override;

#if 0 //for multiple USkeletalMeshComponents on the AvatarActor
	/*
	by Epic
		Initialized the Abilities' ActorInfo - the structure that holds information about who we are acting on and who controls us.
			OwnerActor is the actor that logically owns this component.
			AvatarActor is what physical actor in the world we are acting on. Usually a Pawn but it could be a Tower, Building, Turret, etc, may be the same as Owner
	和訳
		現在のアクティビティを考慮して、このコンポーネントが Tick するべきかどうかの情報を取得します。
	*/
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
#endif

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
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Abilities", Meta = (DisplayName = "AddLooseGameplayTag"))
	void K2_AddLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count = 1);

	/**
	by GASShooter
		Exposes AddLooseGameplayTags to Blueprint. These tags are *not* replicated.
	和訳
		AddLooseGameplayTags をブループリントに公開します。このタグは複製*されません*。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Abilities", Meta = (DisplayName = "AddLooseGameplayTags"))
	void K2_AddLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);

	/**
	by GASShooter
		Exposes RemoveLooseGameplayTag to Blueprint. This tag is *not* replicated.
	和訳
		RemoveLooseGameplayTag をブループリントに公開します。このタグは複製*されません*。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Abilities", Meta = (DisplayName = "RemoveLooseGameplayTag"))
	void K2_RemoveLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count = 1);

	/**
	by GASShooter
		Exposes RemoveLooseGameplayTags to Blueprint. These tags are *not* replicated.
	和訳
		RemoveLooseGameplayTags をブループリントに公開します。このタグは複製*されません*。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Abilities", Meta = (DisplayName = "RemoveLooseGameplayTags"))
	void K2_RemoveLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);

	/*
	解説
		ローカルで GameplayCue を再生するための口金。
		GameplayEffect による GameplayCue を使用する代わりにこちらを使用することで、 GameplayEffect の同期のための RPC が発生しなくなり、帯域に優しくなる。
		同期されないため、利用する際はサウンドやパーティクルなど装飾用のものに制限すること。
		詳しくは [「GASDocumentation」の「4.8.3 Local Gameplay Cues」](https://github.com/tranek/GASDocumentation#concepts-gc-local) [（和訳）](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#concepts-gc-local) を参照。
	*/
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
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
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GameplayEffects", Meta = (DisplayName = "ApplyGameplayEffectToSelfWithPrediction"))
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
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|GameplayEffects", Meta = (DisplayName = "ApplyGameplayEffectToTargetWithPrediction"))
	FActiveGameplayEffectHandle BP_ApplyGameplayEffectToTargetWithPrediction(TSubclassOf<UGameplayEffect> GameplayEffectClass, UAbilitySystemComponent* Target, float Level, FGameplayEffectContextHandle Context);

	/*
	by GASShooter
		Exposes GetTagCount to Blueprint
	和訳
		Blueprint に GetTagCount を公開
	解説
		GASShooter では ScriptName も指定していますが、ドキュメントを見る限り ScriptName は列挙型用のメタデータ指定子なので、外しています。
		//オリジナル
		//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|Abilities", Meta = (DisplayName = "GetTagCount", ScriptName = "GetTagCount"))
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|Abilities", Meta = (DisplayName = "GetTagCount"))
	int32 K2_GetTagCount(FGameplayTag TagToCheck) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|Abilities")
	bool IsInteractingBeforeRemoval() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GASHandsOn|Abilities")
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
	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Abilities")
	/*virtual*/ bool BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately);


	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|Abilities", meta = (DisplayName = "CurrentMontageJumpToSection"))
	void K2_CurrentMontageJumpToSection(FName SectionName);

	FGameplayAbilitySpec* FindAbilitySpecFromClassWithAllowInheritedTypeFlag(TSubclassOf<UGameplayAbility> InAbilityClass, bool bAllowInheritedType);

#if 0 //for multiple USkeletalMeshComponents on the AvatarActor
	// ----------------------------------------------------------------------------------------------------------------
	/**
	by GASShooter
		AnimMontage Support for multiple USkeletalMeshComponents on the AvatarActor.
		Only one ability can be animating at a time though?
	和訳
		AvatarActor で複数の USkeletalMeshComponents を持つものの AnimMontage のサポートです。
		一度に一つのアビリティだけにアニメートできます？

	解説
		これらの関数群は UAbilitySystemComponent で定義されている、名前に ForMesh がつかない関数群を拡張したものです。
		全てを使用しているわけではないですが、関連している機能としてほぼ一式実装されています。
		機能の違いから、(StopAllCurrentMontages / GetCurrentMontages)が追加され、(SetMontageRepAnimPositionMethod)が実装されていません。
		関数の説明コメントもほぼ Epic のもののままです（複数形に直されているぐらい）。
	*/
	/**
	by Epic
		Plays a montage and handles replication and prediction based on passed in ability/activation info
	和訳
		モンタージュを再生し、渡されたアビリティ/アクティベーション情報に基づいてレプリケーションや prediction （予測）を処理します。
	*/
	virtual float PlayMontageForMesh(UGameplayAbility* AnimatingAbility, class USkeletalMeshComponent* InMesh, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None, bool bReplicateMontage = true);

	/**
	by Epic
		Plays a montage without updating replication/prediction structures. Used by simulated proxies when replication tells them to play a montage.
	和訳
		レプリケーション/ prediction （予測）構造体を更新せずにモンタージュを再生します。レプリケーションがモンタージュの再生を指示したときに、 simulated proxies が使用します。
	*/
	virtual float PlayMontageSimulatedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None);

	/**
	by Epic
		Stops whatever montage is currently playing. Expectation is caller should only be stopping it if they are the current animating ability (or have good reason not to check)
	和訳
		現在再生しているモンタージュがなんであれ停止します。期待されるのは、呼び出し側が現在のアビリティのアニメーションを行っている場合にのみ停止させることです。（又は確認しない正当な理由がある場合）
	*/
	virtual void CurrentMontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime = -1.0f);

	/**
	by GASShooter
		Stops all montages currently playing
	和訳
		再生中のモンタージュを全て停止します。
	*/
	virtual void StopAllCurrentMontages(float OverrideBlendOutTime = -1.0f);

	/**
	by Epic
		Stops current montage if it's the one given as the Montage param
	和訳
		現在のモンタージュが Montage パラメータとして渡されたものだった場合、停止します。
	*/
	virtual void StopMontageIfCurrentForMesh(USkeletalMeshComponent* InMesh, const UAnimMontage& Montage, float OverrideBlendOutTime = -1.0f);

	/**
	by Epic
		Clear the animating ability that is passed in, if it's still currently animating
	和訳
		渡されたアビリティのアニメーションがまだアニメーションしている場合はクリアする。
	*/
	virtual void ClearAnimatingAbilityForAllMeshes(UGameplayAbility* Ability);

	/**
	by Epic
		Jumps current montage to given section. Expectation is caller should only be stopping it if they are the current animating ability (or have good reason not to check)
	和訳
		現在のモンタージュを指定されたセクションにジャンプします。期待されるのは、呼び出し側が現在のアビリティのアニメーションを行っている場合にのみ停止させることです。（又は確認しない正当な理由がある場合）
	*/
	virtual void CurrentMontageJumpToSectionForMesh(USkeletalMeshComponent* InMesh, FName SectionName);

	/**
	by Epic
		Sets current montages next section name. Expectation is caller should only be stopping it if they are the current animating ability (or have good reason not to check)
	和訳
		現在のモンタージュの次のセクション名を設定します。期待されるのは、呼び出し側が現在のアビリティのアニメーションを行っている場合にのみ停止させることです。（又は確認しない正当な理由がある場合）
	*/
	virtual void CurrentMontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, FName FromSectionName, FName ToSectionName);

	/**
	by Epic
		Sets current montage's play rate
	和訳
		現在のモンタージュの再生速度を設定します。
	*/
	virtual void CurrentMontageSetPlayRateForMesh(USkeletalMeshComponent* InMesh, float InPlayRate);

	/**
	by Epic
		Returns true if the passed in ability is the current animating ability
	和訳
		渡されたアビリティが現在のアニメーションのアビリティであれば ture を返します。
	*/
	bool IsAnimatingAbilityForAnyMesh(UGameplayAbility* Ability) const;

	/**
	by Epic
		Returns the current animating ability
	和訳
		現在のアニメーションのアビリティを返します。
	*/
	UGameplayAbility* GetAnimatingAbilityFromAnyMesh();

	/**
	by GASShooter
		Returns montages that are currently playing
	和訳
		現在再生中のモンタージュを返します。
	*/
	TArray<UAnimMontage*> GetCurrentMontages() const;

	/**
	by Epic
		Returns the montage that is playing for the mesh
	和訳
		メッシュに再生されているモンタージュを返します。
	*/
	UAnimMontage* GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh);

	/**
	by Epic
		Get SectionID of currently playing AnimMontage
	和訳
		現在再生中の AnimMontage の SectionID を返します。
	*/
	int32 GetCurrentMontageSectionIDForMesh(USkeletalMeshComponent* InMesh);

	/**
	by Epic
		Get SectionName of currently playing AnimMontage
	和訳
		現在再生中の AnimMontage の SectionName を返します。
	*/
	FName GetCurrentMontageSectionNameForMesh(USkeletalMeshComponent* InMesh);

	/**
	by Epic
		Get length in time of current section
	和訳
		現在のセクションの時間の長さを取得します。
	*/
	float GetCurrentMontageSectionLengthForMesh(USkeletalMeshComponent* InMesh);

	/**
	by Epic
		Returns amount of time left in current section
	和訳
		現在のセクションの残り時間を返します。
	*/
	float GetCurrentMontageSectionTimeLeftForMesh(USkeletalMeshComponent* InMesh);

protected:
	/**
	by Epic
		Set if montage rep happens while we don't have the animinstance associated with us yet
	和訳
		まだ AnimInstance が関連付けられていない状態でモンタージュ再生が行われた場合設定されます。
	*/
	UPROPERTY()
	bool bPendingMontageRepForMesh;

	/**
	by Epic
		Data structure for montages that were instigated locally (everything if server, predictive if client. replicated if simulated proxy)
	by GASShooter
		Will be max one element per skeletal mesh on the AvatarActor
	和訳
		ローカルでインスタンスされたモンタージュのデータ構造（サーバーなら everything 、クライアントなら predictive （予測）。 simulated proxy の場合はレプリケーションされます。）
		AvatarActor のスケルタルメッシュ毎に最大一要素となります。
	*/
	UPROPERTY()
	TArray<FGameplayAbilityLocalAnimMontageForMesh> LocalAnimMontageInfoForMeshes;

	/**
	by Epic
		Data structure for replicating montage info to simulated clients
	by GASShooter
		Will be max one element per skeletal mesh on the AvatarActor
	和訳
		もんた＾樹情報をシミュレートされたクライアントに複製するためのデータ構造。
		AvatarActor のスケルタルメッシュ毎に最大一要素となります。
	*/
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAnimMontageForMesh)
	TArray<FGameplayAbilityRepAnimMontageForMesh> RepAnimMontageInfoForMeshes;

	/**
	by GASShooter
		Finds the existing FGameplayAbilityLocalAnimMontageForMesh for the mesh or creates one if it doesn't exist
	和訳
		メッシュの対となる既存の FGameplayAbilityLocalAnimMontageForMesh を検索します。存在しない場合は作成します。
	*/
	FGameplayAbilityLocalAnimMontageForMesh& GetLocalAnimMontageInfoForMesh(USkeletalMeshComponent* InMesh);
	/**
	by GASShooter
		Finds the existing FGameplayAbilityRepAnimMontageForMesh for the mesh or creates one if it doesn't exist
	和訳
		メッシュの対となる既存の FGameplayAbilityRepAnimMontageForMeshを検索します。存在しない場合は作成します。
	*/
	FGameplayAbilityRepAnimMontageForMesh& GetGameplayAbilityRepAnimMontageForMesh(USkeletalMeshComponent* InMesh);

	/**
	by Epic
		Called when a prediction key that played a montage is rejected
	和訳
		モンタージュを再生した予測キーが reject されたときに呼び出されます。
	*/
	void OnPredictiveMontageRejectedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* PredictiveMontage);

	/**
	by Epic
		Copy LocalAnimMontageInfo into RepAnimMontageInfo
	和訳
		LocalAnimMontageInfo を RepAnimMontageInfo にコピーします。
	*/
	void AnimMontage_UpdateReplicatedDataForMesh(USkeletalMeshComponent* InMesh);
	void AnimMontage_UpdateReplicatedDataForMesh(FGameplayAbilityRepAnimMontageForMesh& OutRepAnimMontageInfo);

	/**
	by Epic
		Copy over playing flags for duplicate animation data
	和訳
		重複するアニメーションデータの再生フラグを上書きします。
	*/
	void AnimMontage_UpdateForcedPlayFlagsForMesh(FGameplayAbilityRepAnimMontageForMesh& OutRepAnimMontageInfo);

	UFUNCTION()
	/*virtual*/ void OnRep_ReplicatedAnimMontageForMesh();

	/**
	by Epic
		Returns true if we are ready to handle replicated montage information
	和訳
		レプリケーションされたモンタージュ情報を扱う準備ができていれば true を返します。
	*/
	virtual bool IsReadyForReplicatedMontageForMesh();

	/**
	by Epic
		RPC function called from CurrentMontageSetNextSectionName, replicates to other clients
	和訳
		CurrentMontageSetNextSectionName から呼び出される RPC 関数で、他のクライアントにレプリケーションする。
	*/
	UFUNCTION(Reliable, Server, WithValidation)
	void RPCServerCurrentMontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName);
	void RPCServerCurrentMontageSetNextSectionNameForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName);
	bool RPCServerCurrentMontageSetNextSectionNameForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName);

	/**
	by Epic
		RPC function called from CurrentMontageJumpToSection, replicates to other clients
	和訳
		CurrentMontageJumpToSection から呼び出される RPC 関数で、他のクライアントにレプリケーションする。
	*/
	UFUNCTION(Reliable, Server, WithValidation)
	void RPCServerCurrentMontageJumpToSectionNameForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName);
	void RPCServerCurrentMontageJumpToSectionNameForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName);
	bool RPCServerCurrentMontageJumpToSectionNameForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName);

	/**
	by Epic
		RPC function called from CurrentMontageSetPlayRate, replicates to other clients
	和訳
		CurrentMontageSetPlayRate から呼び出される RPC 関数で、他のクライアントにレプリケーションする。
	*/
	UFUNCTION(Reliable, Server, WithValidation)
	void RPCServerCurrentMontageSetPlayRateForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float InPlayRate);
	void RPCServerCurrentMontageSetPlayRateForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float InPlayRate);
	bool RPCServerCurrentMontageSetPlayRateForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float InPlayRate);

	// End of `AnimMontage Support for multiple USkeletalMeshComponents on the AvatarActor.`
	// ----------------------------------------------------------------------------------------------------------------
#endif

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
