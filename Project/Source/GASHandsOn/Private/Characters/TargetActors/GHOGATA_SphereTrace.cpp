// (C) Sentya Anko 2021


#include "Characters/TargetActors/GHOGATA_SphereTrace.h"

/**
by GASShooter
	Reusable, configurable sphere trace TargetActor.
	Meant to be used with GSAT_WaitTargetDataUsingActor instead of the default WaitTargetData AbilityTask as the default one will destroy the TargetActor.
和訳
	再利用可能で設定可能なスフィアトレース TargetActor 。
	デフォルトの WaitTargetData アビリティタスクでは TargetActor が破壊されてしまうので、 GHOAT_WaitTargetDataUsingActor と一緒に使うことを想定しています。
 */

AGHOGATA_SphereTrace::AGHOGATA_SphereTrace()
{
	TraceSphereRadius = 100.0f;
}

void AGHOGATA_SphereTrace::Configure(
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
	float InTraceSphereRadius,
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
	NumberOfTraces = bInUsePersistentHitResults ? 1 : InNumberOfTraces;
	bIgnoreBlockingHits = bInIgnoreBlockingHits;
	bTraceFromPlayerViewPoint = bInTraceFromPlayerViewPoint;
	bUsePersistentHitResults = bInUsePersistentHitResults;

	//AGHOGATA_SphereTrace
	TraceSphereRadius = InTraceSphereRadius;
}

void AGHOGATA_SphereTrace::SphereTraceWithFilter(TArray<FHitResult>& OutHitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, float Radius, FName ProfileName, const FCollisionQueryParams Params)
{
	check(World);

	TArray<FHitResult> HitResults;
	World->SweepMultiByProfile(HitResults, Start, End, FQuat::Identity, ProfileName, FCollisionShape::MakeSphere(Radius), Params);

	TArray<FHitResult> FilteredHitResults;

	//by GASShooter
	//	Start param could be player ViewPoint. We want HitResult to always display the StartLocation.
	//和訳
	//	パラメータ Start はプレイヤーのビューポイントになります。 HitResult が常に StartLocation を示させたい。
	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();

	for (int32 HitIdx = 0; HitIdx < HitResults.Num(); ++HitIdx)
	{
		FHitResult& Hit = HitResults[HitIdx];

		if (!Hit.Actor.IsValid() || FilterHandle.FilterPassesForActor(Hit.Actor))
		{
			Hit.TraceStart = TraceStart;
			Hit.TraceEnd = End;

			FilteredHitResults.Add(Hit);
		}
	}

	OutHitResults = FilteredHitResults;

	return;
}

void AGHOGATA_SphereTrace::DoTrace(TArray<FHitResult>& HitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params)
{
	SphereTraceWithFilter(HitResults, World, FilterHandle, Start, End, TraceSphereRadius, ProfileName, Params);
}

void AGHOGATA_SphereTrace::ShowDebugTrace(TArray<FHitResult>& HitResults, EDrawDebugTrace::Type DrawDebugType, float Duration)
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

		DrawDebugSphereTraceMulti(GetWorld(), ViewStart, TraceEnd, TraceSphereRadius, DrawDebugType, true, HitResults, FLinearColor::Green, FLinearColor::Red, Duration);
	}
#endif
}

#if ENABLE_DRAW_DEBUG
// Copied from KismetTraceUtils.cpp
//解説
//	KismetTraceUtils.cpp で定義されている同名の関数の実装と全く同じ。（ DrawDebugPoint() の第三引数が直値かマクロかの違いはある）
//	公開されていないため、コードを複製しているものと思われる。
void AGHOGATA_SphereTrace::DrawDebugSweptSphere(const UWorld* InWorld, FVector const& Start, FVector const& End, float Radius, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority)
{
	FVector const TraceVec = End - Start;
	float const Dist = TraceVec.Size();

	FVector const Center = Start + TraceVec * 0.5f;
	float const HalfHeight = (Dist * 0.5f) + Radius;

	FQuat const CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	::DrawDebugCapsule(InWorld, Center, HalfHeight, Radius, CapsuleRot, Color, bPersistentLines, LifeTime, DepthPriority);
}

void AGHOGATA_SphereTrace::DrawDebugSphereTraceMulti(const UWorld* World, const FVector& Start, const FVector& End, float Radius, EDrawDebugTrace::Type DrawDebugType, bool bHit, const TArray<FHitResult>& OutHits, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		if (bHit && OutHits.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = OutHits.Last().Location;
			DrawDebugSweptSphere(World, Start, BlockingHitPoint, Radius, TraceColor.ToFColor(true), bPersistent, LifeTime);
			DrawDebugSweptSphere(World, BlockingHitPoint, End, Radius, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			DrawDebugSweptSphere(World, Start, End, Radius, TraceColor.ToFColor(true), bPersistent, LifeTime);
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
