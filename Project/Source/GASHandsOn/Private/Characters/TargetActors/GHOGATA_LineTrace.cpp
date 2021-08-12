// (C) Sentya Anko 2021


#include "Characters/TargetActors/GHOGATA_LineTrace.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"

/**
by GASShooter
	Reusable, configurable line trace TargetActor.
	Meant to be used with GSAT_WaitTargetDataUsingActor instead of the default WaitTargetData AbilityTask as the default one will destroy the TargetActor.
和訳
	再利用可能で設定可能なライントレース TargetActor 。
	デフォルトの WaitTargetData アビリティタスクでは TargetActor が破壊されてしまうので、 GHOAT_WaitTargetDataUsingActor と一緒に使うことを想定しています。
 */

AGHOGATA_LineTrace::AGHOGATA_LineTrace()
{
}

void AGHOGATA_LineTrace::Configure(
	const FGameplayAbilityTargetingLocationInfo& InStartLocation,
	FGameplayTag InAimingTag,
	FGameplayTag InAimingRemovalTag,
	FCollisionProfileName InTraceProfile,
	FGameplayTargetDataFilterHandle InFilter,
	TSubclassOf<AGameplayAbilityWorldReticle> InReticleClass,
	FWorldReticleParameters InReticleParams,
	bool bInIgnoreBlockingHits,
	bool bInShouldProduceTargetDataOnServer,
	bool bInUsePersistentHitResults,
	bool bInDebug,
	bool bInTraceAffectsAimPitch,
	bool bInTraceFromPlayerViewPoint,
	bool bInUseAimingSpreadMod,
	float InMaxRange,
	float InBaseSpread,
	float InAimingSpreadMod,
	float InTargetingSpreadIncrement,
	float InTargetingSpreadMax,
	int32 InMaxHitResultsPerTrace,
	int32 InNumberOfTraces)
{
	//AGameplayAbilityTargetActor
	ShouldProduceTargetDataOnServer = bInShouldProduceTargetDataOnServer;
	StartLocation = InStartLocation;
	ReticleParams = InReticleParams;
	ReticleClass = InReticleClass;
	Filter = InFilter;
	bDebug = bInDebug;

	//AGHOGATA_Trace
	BaseSpread = InBaseSpread;
	AimingSpreadMod = InAimingSpreadMod;
	TargetingSpreadIncrement = InTargetingSpreadIncrement;
	TargetingSpreadMax = InTargetingSpreadMax;
	bUseAimingSpreadMod = bInUseAimingSpreadMod;
	AimingTag = InAimingTag;
	AimingRemovalTag = InAimingRemovalTag;
	MaxRange = InMaxRange;
	TraceProfile = InTraceProfile;
	bTraceAffectsAimPitch = bInTraceAffectsAimPitch;
	MaxHitResultsPerTrace = InMaxHitResultsPerTrace;
	NumberOfTraces = bInUsePersistentHitResults? 1: InNumberOfTraces;
	bIgnoreBlockingHits = bInIgnoreBlockingHits;
	bTraceFromPlayerViewPoint = bInTraceFromPlayerViewPoint;
	bUsePersistentHitResults = bInUsePersistentHitResults;
}

void AGHOGATA_LineTrace::DoTrace(TArray<FHitResult>& HitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params)
{
	LineTraceWithFilter(HitResults, World, FilterHandle, Start, End, ProfileName, Params);
}

void AGHOGATA_LineTrace::ShowDebugTrace(TArray<FHitResult>& HitResults, EDrawDebugTrace::Type DrawDebugType, float Duration)
{
#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		FVector ViewStart = StartLocation.GetTargetingTransform().GetLocation();
		FRotator ViewRot;
		if (MasterPC && bTraceFromPlayerViewPoint)
		{
			MasterPC->GetPlayerViewPoint(ViewStart, ViewRot);
		}

		FVector TraceEnd = HitResults[0].TraceEnd;
		if (NumberOfTraces > 1 || bUsePersistentHitResults)
		{
			TraceEnd = CurrentTraceEnd;
		}

		DrawDebugLineTraceMulti(GetWorld(), ViewStart, TraceEnd, DrawDebugType, true, HitResults, FLinearColor::Green, FLinearColor::Red, Duration);
	}
#endif
}

#if ENABLE_DRAW_DEBUG
// Copied from KismetTraceUtils.cpp
//解説
//	KismetTraceUtils.cpp で定義されている同名の関数の実装と全く同じ。（ DrawDebugPoint() の第三引数が直値かマクロかの違いはある）
//	公開されていないため、コードを複製しているものと思われる。
void AGHOGATA_LineTrace::DrawDebugLineTraceMulti(const UWorld* World, const FVector& Start, const FVector& End, EDrawDebugTrace::Type DrawDebugType, bool bHit, const TArray<FHitResult>& OutHits, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		// @fixme, draw line with thickness = 2.f?
		if (bHit && OutHits.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = OutHits.Last().ImpactPoint;
			::DrawDebugLine(World, Start, BlockingHitPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, BlockingHitPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}

		// draw hits
		for (int32 HitIdx = 0; HitIdx < OutHits.Num(); ++HitIdx)
		{
			FHitResult const& Hit = OutHits[HitIdx];
			::DrawDebugPoint(World, Hit.ImpactPoint, 16.0f, (Hit.bBlockingHit ? TraceColor.ToFColor(true) : TraceHitColor.ToFColor(true)), bPersistent, LifeTime);
		}
	}
}
#endif // ENABLE_DRAW_DEBUG

