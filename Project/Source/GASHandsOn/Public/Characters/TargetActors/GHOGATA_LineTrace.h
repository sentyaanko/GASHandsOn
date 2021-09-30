// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Characters/TargetActors/GHOGATA_Trace.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "Engine/CollisionProfile.h"
#include "WorldCollision.h"
#include "GHOGATA_LineTrace.generated.h"

/**
by GASShooter
	Reusable, configurable line trace TargetActor.
	Meant to be used with GHOAT_WaitTargetDataUsingActor instead of the default WaitTargetData AbilityTask as the default one will destroy the TargetActor.
和訳
	再利用可能で設定可能なライントレース TargetActor 。
	デフォルトの WaitTargetData アビリティタスクでは TargetActor が破壊されてしまうので、 GHOAT_WaitTargetDataUsingActor と一緒に使うことを想定しています。
 */
UCLASS()
class GASHANDSON_API AGHOGATA_LineTrace : public AGHOGATA_Trace
{
	GENERATED_BODY()

public:
	AGHOGATA_LineTrace();

	/**
	by GASShooter
		Configure the TargetActor for use. 
		This TargetActor could be used in multiple abilities and there's no guarantee what state it will be in. 
		You will need to make sure that only one ability is using this TargetActor at a time.
		
		@param InStartLocation Location to trace from.
		@param InAimingTag Optional. Predicted GameplayTag for aiming. Only used if we modify spread while aiming. If used, must set InAimingRemovalTag also.
		@param InAimingRemovalTag Optional. Predicted GameplayTag for aiming removal. Only used if we modify spread while aiming. If used, must set InAimingTag also.
		@param InTraceProfile Collision profile to use for tracing.
		@param InFilter Hit Actors must pass this filter to be returned in the TargetData.
		@param InReticleClass Reticle that will appear on top of acquired targets. Reticles will be spawned/despawned as targets are acquired/lost.
		@param InReticleParams Parameters for world reticle. Usage of these parameters is dependent on the reticle.
		@param bInIgnoreBlockingHits Ignore blocking collision hits in the trace. Useful if you want to target through walls.
		@param bInShouldProduceTargetDataOnServer If set, this TargetActor will produce TargetData on the Server in addition to the client and the client will just send a generic "Confirm" event to the server.
				If false, the client will send the TargetData to the Server. This is handled by the WaitTargetDataUsingActor AbilityTask.
		@param bInUsePersistentHitResults Should HitResults persist while targeting? HitResults are cleared on Confirm/Cancel or when new HitResults take their place.
		@param bInDebug When true, this TargetActor will show debug lines of the trace and hit results.
		@param bInTraceAffectsAimPitch Does the trace affect the aiming pitch?
		@param bInTraceFromPlayerViewPoint Should we trace from the player ViewPoint instead of the StartLocation? 
				The TargetData HitResults will still have the StartLocation for the TraceStart. 
				This is useful for FPS where we want to trace from the player ViewPoint but draw the bullet tracer from the weapon muzzle.
				TODO: AI Controllers should fall back to muzzle location. Not implemented yet.
		@param bInUseAImingSpreadMod Should we modify spread based on if we're aiming? If true, must set InAimingTag and InAimingRemovalTag.
		@param InMaxRange Max range for this trace.
		@param InBaseSpread Base targeting spread in degrees.
		@param InAimingSpreadMod Optional. Multiplicative modifier to spread if aiming.
		@param InTargetingSpreadIncrement Amount spread increments from continuous targeting in degrees.
		@param InTargetingSpreadMax Maximum amount of spread for continuous targeting in degrees.
		@param InMaxHitResultsPerTrace Max hit results that a trace can return. < 1 just returns the trace end point.
		@param InNumberOfTraces Number of traces to perform. Intended to be used with BaseSpread for multi-shot weapons like shotguns. 
				Not intended to be used with PersistentHitsResults. If using PersistentHitResults, NumberOfTraces is hardcoded to 1. 
				You will need to add support for this in your project if you need it.
	和訳
		TargetActor を使用するための設定を行います。
		この TargetActor は複数のアビリティで使われるかもしれませんが、その際にどのような状態になるかは保証されていません。
		一度に 1 つのアビリティだけがこの TargetActor を使用していることを確認する必要があります。

		@param InStartLocation トレースの開始する場所。
		@param InAimingTag 任意です。 Predicted （予測）される エイム（時に付与される） GameplayTag 。エイムしている間散らばり具合を変更する場合にのみ使用する。使用する場合は、 InAimingRemovalTag も設定しなければならない。
		@param InAimingRemovalTag 任意です。 Predicted （予測）される エイムをやめる（時に付与される） GameplayTag 。エイムしている間散らばり具合を変更する場合にのみ使用する。使用する場合は、 InAimingTag も設定しなければならない。
		@param InTraceProfile トレースに使用するコリジョンプロファイル。
		@param InFilter TargetData に入れられて返される Hit Actors が通過している必要があるフィルター。
		@param InReticleClass 取得したターゲットの上に表示されるレティクルです。レティクルは取得/喪失に応じて生成/破棄されます。
		@param InReticleParams ワールドレティクルのパラメータです。これらのパラメータの使い方は、レティクルに依存します。
		@param bInIgnoreBlockingHits トレース内のブロッキングコリジョンの命中を無視します。壁越しにターゲットを狙う場合に便利です。
		@param bInShouldProduceTargetDataOnServer true の場合、この TargetActor はクライアントに加えてサーバーでも TargetData  を生成し、クライアントは一般的な「 Confirm 」イベントをサーバーに送信するだけになります。
				false の場合、クライアントは TargetData をサーバーに送ります。これは WaitTargetDataUsingActor AbilityTask で処理されます。
		@param bInUsePersistentHitResults HitResults はターゲッティング中も持続させるべきか？ HitResults は Confirm/Cancel の際、または新しい HitResults に置き換わる時にクリアされます。
		@param bInDebug true の場合、この TargetActor はトレースとヒット結果のデバッグラインを表示します。
		@param bInTraceAffectsAimPitch トレースはエイムのピッチに影響するか？
		@param bInTraceFromPlayerViewPoint StartLocation ではなくプレイヤーのビューポイントからトレースすべきか？
				TargetData の HitResults には TraceStart の StartLocation が残っています。
				これは、 FPS において、プレイヤーのビューポイントからトレースしたいが、銃口からの弾丸のトレーサーの描画もしたい場合に有効です。
				TODO: AI コントローラーは、銃口の位置にフォールバックする必要があります。まだ実装されていません。
		@param bInUseAImingSpreadMod エイム中かどうかで弾の散らばり具合を変更すべきか？ true の場合、 InAimingTag と InAimingRemovalTag を指定しなければならない。
		@param InMaxRange このトレースの最大範囲。
		@param InBaseSpread 基本のターゲッティング時の弾の散らばり具合の角度。
		@param InAimingSpreadMod 任意です。エイム中は弾の散らばり具合に乗算修正を行います。
		@param InTargetingSpreadIncrement ターゲティングを継続した際の弾の散らばり具合の角度の増分。
		@param InTargetingSpreadMax ターゲティングを継続した際の弾の散らばり具合の角度の最大値。
		@param InMaxHitResultsPerTrace トレースが返すことができる最大のヒット結果。 1 より小さい場合は、トレースの終点を返します。
		@param InNumberOfTraces 実行するトレース数。ショットガンのようなマルチショット武器の BaseSpread との併用を意図しています。
				PersistentHitsResults と一緒に使用することはできません。 PersistentHitResults を使用する場合、 NumberOfTraces は 1 にハードコードされます。
				あなたが必要ならば、あなたのプロジェクトで、これのサポートを追加する必要があります。
	*/
	UFUNCTION(BlueprintCallable)
	void Configure(
		UPARAM(DisplayName = "Start Location") const FGameplayAbilityTargetingLocationInfo& InStartLocation,
		UPARAM(DisplayName = "Aiming Tag") FGameplayTag InAimingTag,
		UPARAM(DisplayName = "Aiming Removal Tag") FGameplayTag InAimingRemovalTag,
		UPARAM(DisplayName = "Trace Profile") FCollisionProfileName InTraceProfile,
		UPARAM(DisplayName = "Filter") FGameplayTargetDataFilterHandle InFilter,
		UPARAM(DisplayName = "Reticle Class") TSubclassOf<AGameplayAbilityWorldReticle> InReticleClass,
		UPARAM(DisplayName = "Reticle Params") FWorldReticleParameters InReticleParams,
		UPARAM(DisplayName = "Ignore Blocking Hits") bool bInIgnoreBlockingHits = false,
		UPARAM(DisplayName = "Should Produce Target Data on Server") bool bInShouldProduceTargetDataOnServer = false,
		UPARAM(DisplayName = "Use Persistent Hit Results") bool bInUsePersistentHitResults = false,
		UPARAM(DisplayName = "Debug") bool bInDebug = false,
		UPARAM(DisplayName = "Trace Affects Aim Pitch") bool bInTraceAffectsAimPitch = true,
		UPARAM(DisplayName = "Trace From Player ViewPoint") bool bInTraceFromPlayerViewPoint = false,
		UPARAM(DisplayName = "Use Aiming Spread Mod") bool bInUseAimingSpreadMod = false,
		UPARAM(DisplayName = "Max Range") float InMaxRange = 999999.0f,
		UPARAM(DisplayName = "Base Targeting Spread") float InBaseSpread = 0.0f,
		UPARAM(DisplayName = "Aiming Spread Mod") float InAimingSpreadMod = 0.0f,
		UPARAM(DisplayName = "Targeting Spread Increment") float InTargetingSpreadIncrement = 0.0f,
		UPARAM(DisplayName = "Targeting Spread Max") float InTargetingSpreadMax = 0.0f,
		UPARAM(DisplayName = "Max Hit Results Per Trace") int32 InMaxHitResultsPerTrace = 1,
		UPARAM(DisplayName = "Number of Traces") int32 InNumberOfTraces = 1
	);

protected:

	virtual void DoTrace(TArray<FHitResult>& HitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params) override;
	virtual void ShowDebugTrace(TArray<FHitResult>& HitResults, EDrawDebugTrace::Type DrawDebugType, float Duration = 2.0f) override;

#if ENABLE_DRAW_DEBUG
	//by GASShooter
	//	Util for drawing result of multi line trace from KismetTraceUtils.h
	//和訳
	//	KismetTraceUtils.h から、マルチライントレースの結果を描画するためのユーティリティ。
	void DrawDebugLineTraceMulti(const UWorld* World, const FVector& Start, const FVector& End, EDrawDebugTrace::Type DrawDebugType, bool bHit, const TArray<FHitResult>& OutHits, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime);
#endif // ENABLE_DRAW_DEBUG
};
