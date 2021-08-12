// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "CollisionQueryParams.h"
//#include "DrawDebugHelpers.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WorldCollision.h"
#include "GHOGATA_Trace.generated.h"

/**
by GASShooter
	Reusable, configurable trace TargetActor. Subclass with your own trace shapes.
	Meant to be used with GSAT_WaitTargetDataUsingActor instead of the default WaitTargetData AbilityTask as the default one will destroy the TargetActor.
和訳
	再利用可能で設定可能なトレース TargetActor 。独自のトレース形状を持つサブクラスがあります。
	デフォルトの WaitTargetData アビリティタスクでは TargetActor が破壊されてしまうので、 GHOAT_WaitTargetDataUsingActor と一緒に使うことを想定しています。
 */
UCLASS()
class GASHANDSON_API AGHOGATA_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	AGHOGATA_Trace();

	//by GASShooter
	//	Base targeting spread (degrees)
	//和訳
	//	基本となる散らばり具合（角度）
	UPROPERTY(BlueprintReadWrite, Category = "Accuracy")
	float BaseSpread;

	//by GASShooter
	//	Aiming spread modifier
	//和訳
	//	エイム時の散らばり具合の変化値
	UPROPERTY(BlueprintReadWrite, Category = "Accuracy")
	float AimingSpreadMod;

	//by GASShooter
	//	Continuous targeting: spread increment
	//和訳
	//	連射時 : 散らばり具合の加算値
	UPROPERTY(BlueprintReadWrite, Category = "Accuracy")
	float TargetingSpreadIncrement;

	//by GASShooter
	//	Continuous targeting: max increment
	//和訳
	//	連射時 : 最大の加算値
	UPROPERTY(BlueprintReadWrite, Category = "Accuracy")
	float TargetingSpreadMax;

	//by GASShooter
	//	Current spread from continuous targeting
	//和訳
	//	連射による現在の散らばり具合
	float CurrentTargetingSpread;

	UPROPERTY(BlueprintReadWrite, Category = "Accuracy")
	bool bUseAimingSpreadMod;

	UPROPERTY(BlueprintReadWrite, Category = "Accuracy")
	FGameplayTag AimingTag;

	UPROPERTY(BlueprintReadWrite, Category = "Accuracy")
	FGameplayTag AimingRemovalTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	float MaxRange;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, config, meta = (ExposeOnSpawn = true), Category = "Trace")
	FCollisionProfileName TraceProfile;

	//by GASShooter
	//	Does the trace affect the aiming pitch
	//和訳
	//	トレースがエイミングピッチに影響するか
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	bool bTraceAffectsAimPitch;

	//by GASShooter
	//	Maximum hit results to return per trace. 0 just returns the trace end point.
	//和訳
	//	トレース毎に返すヒット結果の最大値。 0 はトレースの終点を返すだけです。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	int32 MaxHitResultsPerTrace;

	//by GASShooter
	//	Number of traces to perform at one time. Single bullet weapons like rilfes will only do one trace.
	//	Multi-bullet weapons like shotguns can do multiple traces. Not intended to be used with PersistentHits.
	//和訳
	//	一度に実行するトレースの数。ライフルのように単弾の武器は一度のトレースしか行いません。
	// ショットガンのような多弾頭の武器は複数のトレースを行うことができます。 PersistentHits と一緒に使うことはできません。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	int32 NumberOfTraces;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	bool bIgnoreBlockingHits;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	bool bTraceFromPlayerViewPoint;

	//by GASShooter
	//	HitResults will persist until Confirmation/Cancellation or until a new HitResult takes its place
	//和訳
	//	（true の場合、） HitResults が、 Confirmation/Cancellation が行われるか、新しい HitResult が作成されるまで存続します。
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Trace")
	bool bUsePersistentHitResults;

	UFUNCTION(BlueprintCallable)
	virtual void ResetSpread();

	virtual float GetCurrentSpread() const;

	//by GASShooter
	//	Expose to Blueprint
	//和訳
	//	Blueprint に公開します。
	UFUNCTION(BlueprintCallable)
	void SetStartLocation(const FGameplayAbilityTargetingLocationInfo& InStartLocation);

	//by GASShooter
	//	Expose to Blueprint
	//和訳
	//	Blueprint に公開します。
	UFUNCTION(BlueprintCallable)
	virtual void SetShouldProduceTargetDataOnServer(bool bInShouldProduceTargetDataOnServer);

	//by GASShooter
	//	Expose to Blueprint
	//和訳
	//	Blueprint に公開します。
	UFUNCTION(BlueprintCallable)
	void SetDestroyOnConfirmation(bool bInDestroyOnConfirmation = false);

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

	virtual void CancelTargeting() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	//by GASShooter
	//	Traces as normal, but will manually filter all hit actors
	//和訳
	//	トレースは通常通り行われ、全てのヒットしたアクターのフィルターを手動で行えます。
	virtual void LineTraceWithFilter(TArray<FHitResult>& OutHitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params);

	virtual void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, FVector& OutTraceEnd, bool bIgnorePitch = false);

	virtual bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition);

	virtual void StopTargeting();

protected:
	//by GASShooter
	//	Trace End point, useful for debug drawing
	//和訳
	//	デバッグ描画に便利な、トレースの終了点。
	FVector CurrentTraceEnd;
	
	TArray<TWeakObjectPtr<AGameplayAbilityWorldReticle>> ReticleActors;
	TArray<FHitResult> PersistentHitResults;

	virtual FGameplayAbilityTargetDataHandle MakeTargetData(const TArray<FHitResult>& HitResults) const;
	virtual TArray<FHitResult> PerformTrace(AActor* InSourceActor);

	virtual void DoTrace(TArray<FHitResult>& HitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params) PURE_VIRTUAL(AGHOGATA_Trace, return;);
	virtual void ShowDebugTrace(TArray<FHitResult>& HitResults, EDrawDebugTrace::Type DrawDebugType, float Duration = 2.0f) PURE_VIRTUAL(AGHOGATA_Trace, return;);

	virtual AGameplayAbilityWorldReticle* SpawnReticleActor(FVector Location, FRotator Rotation);
	virtual void DestroyReticleActors();
};
