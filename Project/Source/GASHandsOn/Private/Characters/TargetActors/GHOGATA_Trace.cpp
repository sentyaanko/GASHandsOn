// (C) Sentya Anko 2021


#include "Characters/TargetActors/GHOGATA_Trace.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "GameplayAbilitySpec.h"

AGHOGATA_Trace::AGHOGATA_Trace()
{
	bDestroyOnConfirmation = false;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
	MaxHitResultsPerTrace = 1;
	NumberOfTraces = 1;
	bIgnoreBlockingHits = false;
	bTraceAffectsAimPitch = true;
	bTraceFromPlayerViewPoint = false;
	MaxRange = 999999.0f;
	bUseAimingSpreadMod = false;
	BaseSpread = 0.0f;
	AimingSpreadMod = 0.0f;
	TargetingSpreadIncrement = 0.0f;
	TargetingSpreadMax = 0.0f;
	CurrentTargetingSpread = 0.0f;
	bUsePersistentHitResults = false;
}

void AGHOGATA_Trace::ResetSpread()
{
	bUseAimingSpreadMod = false;
	BaseSpread = 0.0f;
	AimingSpreadMod = 0.0f;
	TargetingSpreadIncrement = 0.0f;
	TargetingSpreadMax = 0.0f;
	CurrentTargetingSpread = 0.0f;
}

float AGHOGATA_Trace::GetCurrentSpread() const
{
	float FinalSpread = BaseSpread + CurrentTargetingSpread;

	if (bUseAimingSpreadMod && AimingTag.IsValid() && AimingRemovalTag.IsValid())
	{
		UAbilitySystemComponent* ASC = OwningAbility->GetCurrentActorInfo()->AbilitySystemComponent.Get();
		if (ASC && (ASC->GetTagCount(AimingTag) > ASC->GetTagCount(AimingRemovalTag)))
		{
			FinalSpread *= AimingSpreadMod;
		}
	}

	return FinalSpread;
}

void AGHOGATA_Trace::SetStartLocation(const FGameplayAbilityTargetingLocationInfo& InStartLocation)
{
	StartLocation = InStartLocation;
}

void AGHOGATA_Trace::SetShouldProduceTargetDataOnServer(bool bInShouldProduceTargetDataOnServer)
{
	ShouldProduceTargetDataOnServer = bInShouldProduceTargetDataOnServer;
}

void AGHOGATA_Trace::SetDestroyOnConfirmation(bool bInDestroyOnConfirmation)
{
	bDestroyOnConfirmation = bDestroyOnConfirmation;
}

void AGHOGATA_Trace::StartTargeting(UGameplayAbility* Ability)
{
	//by GASShooter
	//	Don't call to Super because we can have more than one Reticle
	//和訳
	//	レティクルを複数持てるので、基底クラスの関数を呼び出さないでください。

	SetActorTickEnabled(true);

	OwningAbility = Ability;
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();

	//by GASShooter
	//	This is a lazy way of emptying and repopulating the ReticleActors.
	//	We could come up with a solution that reuses them.
	//和訳
	//	これは ReticleActors を空にして再配置するための怠惰な方法です。
	//	これを再利用する方法を考えてみましょう。
	DestroyReticleActors();

	if (ReticleClass)
	{
		for (int32 i = 0; i < MaxHitResultsPerTrace * NumberOfTraces; i++)
		{
			SpawnReticleActor(GetActorLocation(), GetActorRotation());
		}
	}

	if (bUsePersistentHitResults)
	{
		PersistentHitResults.Empty();
	}
}

void AGHOGATA_Trace::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (SourceActor)
	{
		TArray<FHitResult> HitResults = PerformTrace(SourceActor);
		FGameplayAbilityTargetDataHandle Handle = MakeTargetData(HitResults);
		TargetDataReadyDelegate.Broadcast(Handle);

#if ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			ShowDebugTrace(HitResults, EDrawDebugTrace::Type::ForDuration, 2.0f);
		}
#endif
	}

	if (bUsePersistentHitResults)
	{
		PersistentHitResults.Empty();
	}
}

void AGHOGATA_Trace::CancelTargeting()
{
	const FGameplayAbilityActorInfo* ActorInfo = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
	UAbilitySystemComponent* ASC = (ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr);
	if (ASC)
	{
		ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::GenericCancel, OwningAbility->GetCurrentAbilitySpecHandle(), OwningAbility->GetCurrentActivationInfo().GetActivationPredictionKey()).Remove(GenericCancelHandle);
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("AGameplayAbilityTargetActor::CancelTargeting called with null ASC! Actor %s"), *GetName());
	}

	CanceledDelegate.Broadcast(FGameplayAbilityTargetDataHandle());

	SetActorTickEnabled(false);

	if (bUsePersistentHitResults)
	{
		PersistentHitResults.Empty();
	}
}

void AGHOGATA_Trace::BeginPlay()
{
	Super::BeginPlay();

	//by GASShooter
	//	Start with Tick disabled. We'll enable it in StartTargeting() and disable it again in StopTargeting().
	//	For instant confirmations, tick will never happen because we StartTargeting(), ConfirmTargeting(), and immediately StopTargeting().
	//和訳
	//	Tick を無効にして開始します。 StartTargeting() で有効にして、 StopTargeting() で再び無効にします。
	//	instant confirmation の場合、 Tick は発生しません。なぜなら、 StartTargeting() 、 ConfirmTargeting() 、そしてすぐに StopTargeting() を呼び出すからです。
	SetActorTickEnabled(false);
}

void AGHOGATA_Trace::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyReticleActors();

	Super::EndPlay(EndPlayReason);
}

void AGHOGATA_Trace::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TArray<FHitResult> HitResults;
	if (bDebug || bUsePersistentHitResults)
	{
		//by GASShooter
		//	Only need to trace on Tick if we're showing debug or if we use persistent hit results, otherwise we just use the confirmation trace
		//和訳
		//	Tick でトレースが必要なのは、デバグ表示、又は命中結果を持続的に使用する場合で、それ以外の場合は確認用のトレースを使用します。
		HitResults = PerformTrace(SourceActor);
	}

#if ENABLE_DRAW_DEBUG
	if (SourceActor && bDebug)
	{
		ShowDebugTrace(HitResults, EDrawDebugTrace::Type::ForOneFrame);
	}
#endif
}

void AGHOGATA_Trace::LineTraceWithFilter(TArray<FHitResult>& OutHitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params)
{
	check(World);

	TArray<FHitResult> HitResults;
	World->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);

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

void AGHOGATA_Trace::AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, FVector& OutTraceEnd, bool bIgnorePitch)
{
	//by GASShooter
	//	Server and launching client only
	//和訳
	//	サーバーと起動クライアントのみ
	if (!OwningAbility)
	{
		return;
	}

	//by GASShooter
	//	Default values in case of AI Controller
	//和訳
	//	AI コントローラーの場合のデフォルト値
	FVector ViewStart = TraceStart;
	FRotator ViewRot = StartLocation.GetTargetingTransform().GetRotation().Rotator();

	if (MasterPC)
	{
		MasterPC->GetPlayerViewPoint(ViewStart, ViewRot);
	}

	const FVector ViewDir = ViewRot.Vector();
	FVector ViewEnd = ViewStart + (ViewDir * MaxRange);

	ClipCameraRayToAbilityRange(ViewStart, ViewDir, TraceStart, MaxRange, ViewEnd);

	//by GASShooter
	//	Use first hit
	//和訳
	//	最初のヒットを使用する
	TArray<FHitResult> HitResults;
	LineTraceWithFilter(HitResults, InSourceActor->GetWorld(), Filter, ViewStart, ViewEnd, TraceProfile.Name, Params);

	CurrentTargetingSpread = FMath::Min(TargetingSpreadMax, CurrentTargetingSpread + TargetingSpreadIncrement);

	const bool bUseTraceResult = HitResults.Num() > 0 && (FVector::DistSquared(TraceStart, HitResults[0].Location) <= (MaxRange * MaxRange));

	const FVector AdjustedEnd = (bUseTraceResult) ? HitResults[0].Location : ViewEnd;

	FVector AdjustedAimDir = (AdjustedEnd - TraceStart).GetSafeNormal();
	if (AdjustedAimDir.IsZero())
	{
		AdjustedAimDir = ViewDir;
	}

	if (!bTraceAffectsAimPitch && bUseTraceResult)
	{
		FVector OriginalAimDir = (ViewEnd - TraceStart).GetSafeNormal();

		if (!OriginalAimDir.IsZero())
		{
			//by GASShooter
			//	Convert to angles and use original pitch
			//和訳
			//	角度に変換し、元のピッチを使用します。
			const FRotator OriginalAimRot = OriginalAimDir.Rotation();

			FRotator AdjustedAimRot = AdjustedAimDir.Rotation();
			AdjustedAimRot.Pitch = OriginalAimRot.Pitch;

			AdjustedAimDir = AdjustedAimRot.Vector();
		}
	}

	const float CurrentSpread = GetCurrentSpread();

	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const FVector ShootDir = WeaponRandomStream.VRandCone(AdjustedAimDir, ConeHalfAngle, ConeHalfAngle);

	OutTraceEnd = TraceStart + (ShootDir * MaxRange);
}

bool AGHOGATA_Trace::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition)
{
	FVector CameraToCenter = AbilityCenter - CameraLocation;
	float DotToCenter = FVector::DotProduct(CameraToCenter, CameraDirection);
	//by GASShooter
	//	If this fails, we're pointed away from the center, but we might be inside the sphere and able to find a good exit point.
	//和訳
	//	false の場合、中心から離れた方向を向いていますが、球の内側に居て、適切な出口店を見つけることができる可能性があります。
	if (DotToCenter >= 0)
	{
		float DistanceSquared = CameraToCenter.SizeSquared() - (DotToCenter * DotToCenter);
		float RadiusSquared = (AbilityRange * AbilityRange);
		if (DistanceSquared <= RadiusSquared)
		{
			float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
		
			//by GASShooter
			//	Subtracting instead of adding will get the other intersection point
			//和訳
			//	加算の代わりに減算すると、他の交点が取得されます。
			float DistanceAlongRay = DotToCenter + DistanceFromCamera;
			
			//by GASShooter
			//	Cam aim point clipped to range sphere
			//和訳
			//	範囲旧にクリップされたカメラのエイムポイント
			ClippedPosition = CameraLocation + (DistanceAlongRay * CameraDirection);
			return true;
		}
	}
	return false;
}

void AGHOGATA_Trace::StopTargeting()
{
	SetActorTickEnabled(false);

	DestroyReticleActors();

	//by GASShooter
	//	Clear added callbacks
	//和訳
	//	追加されたコールバックをクリアします。
	TargetDataReadyDelegate.Clear();
	CanceledDelegate.Clear();

	if (GenericDelegateBoundASC)
	{
		GenericDelegateBoundASC->GenericLocalConfirmCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::ConfirmTargeting);
		GenericDelegateBoundASC->GenericLocalCancelCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::CancelTargeting);
		GenericDelegateBoundASC = nullptr;
	}
}

FGameplayAbilityTargetDataHandle AGHOGATA_Trace::MakeTargetData(const TArray<FHitResult>& HitResults) const
{
	FGameplayAbilityTargetDataHandle ReturnDataHandle;

	for (int32 i = 0; i < HitResults.Num(); i++)
	{
		//by GASShooter
		//	Note: These are cleaned up by the FGameplayAbilityTargetDataHandle (via an internal TSharedPtr)
		//和訳
		//	注：これらは (TSharedPtr の内部を通して )FGameplayAbilityTargetDataHandle によってクリーンアップされます。
		FGameplayAbilityTargetData_SingleTargetHit* ReturnData = new FGameplayAbilityTargetData_SingleTargetHit();
		ReturnData->HitResult = HitResults[i];
		ReturnDataHandle.Add(ReturnData);
	}

	return ReturnDataHandle;
}

TArray<FHitResult> AGHOGATA_Trace::PerformTrace(AActor* InSourceActor)
{
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(InSourceActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AGSGATA_LineTrace), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);
	Params.bIgnoreBlocks = bIgnoreBlockingHits;

	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;

	if (MasterPC)
	{
		FVector ViewStart;
		FRotator ViewRot;
		MasterPC->GetPlayerViewPoint(ViewStart, ViewRot);

		TraceStart = bTraceFromPlayerViewPoint ? ViewStart : TraceStart;
	}

	if (bUsePersistentHitResults)
	{
		//by GASShooter
		//	Clear any blocking hit results, invalid Actors, or actors out of range
		//	TODO Check for visibility if we add AIPerceptionComponent in the future
		//和訳
		//	ブロックされたヒット結果、無効なアクター、又は範囲外のアクターをクリアします。
		//	TODO: 将来的に AIPerceptionComponent を追加した場合の視認性のチェック
		for (int32 i = PersistentHitResults.Num() - 1; i >= 0; i--)
		{
			FHitResult& HitResult = PersistentHitResults[i];

			if (HitResult.bBlockingHit || !HitResult.Actor.IsValid() || FVector::DistSquared(TraceStart, HitResult.Actor.Get()->GetActorLocation()) > (MaxRange * MaxRange))
			{
				PersistentHitResults.RemoveAt(i);
			}
		}
	}

	TArray<FHitResult> ReturnHitResults;

	for (int32 TraceIndex = 0; TraceIndex < NumberOfTraces; TraceIndex++)
	{
		//by GASShooter
		//	Effective on server and launching client only
		//和訳
		//	サーバーと起動クライアントのみで有効
		AimWithPlayerController(InSourceActor, Params, TraceStart, TraceEnd);

		// ------------------------------------------------------

		SetActorLocationAndRotation(TraceEnd, SourceActor->GetActorRotation());

		CurrentTraceEnd = TraceEnd;

		TArray<FHitResult> TraceHitResults;
		DoTrace(TraceHitResults, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);

		for (int32 j = TraceHitResults.Num() - 1; j >= 0; j--)
		{
			if (MaxHitResultsPerTrace >= 0 && j + 1 > MaxHitResultsPerTrace)
			{
				//by GASShooter
				//	Trim to MaxHitResultsPerTrace
				//和訳
				//	MaxHitResultsPerTrace へトリム
				TraceHitResults.RemoveAt(j);
				continue;
			}

			FHitResult& HitResult = TraceHitResults[j];

			//by GASShooter
			//	Reminder: if bUsePersistentHitResults, Number of Traces = 1
			//和訳
			//	リマインダー : bUsePersistentHitResults が true  の場合、 Traces の値は 1
			if (bUsePersistentHitResults)
			{
				//by GASShooter
				//	This is looping backwards so that further objects from player are added first to the queue.
				//	This results in closer actors taking precedence as the further actors will get bumped out of the TArray.
				//和訳
				//	これは逆方向にループしています。これにより、プレイヤーから遠いオブジェクトからキューに追加されます。
				//	この結果は、近い位置に居るアクターが優先され、遠い位置に居るアクターは TArray からはじき出されることになります。
				//解説
				//	訳、あってる？
				//		TraceHitResults は近い順（多分）
				//		ループを逆回ししているので遠い順に処理される。
				//		PersistentHitResults に要素を足す際に MaxHitResultsPerTrace 以上の要素がある場合は最初の様子を remove している。
				//		そのため、 PersistentHitResults には MaxHitResultsPerTrace 以下の数の hit actor が遠い順に入っている。
				if (HitResult.Actor.IsValid() && (!HitResult.bBlockingHit || PersistentHitResults.Num() < 1))
				{
					bool bActorAlreadyInPersistentHits = false;

					//by GASShooter
					//	Make sure PersistentHitResults doesn't have this hit actor already
					//和訳
					//	PersistentHitResults にこの hit actor が既にないことを確認
					for (int32 k = 0; k < PersistentHitResults.Num(); k++)
					{
						FHitResult& PersistentHitResult = PersistentHitResults[k];

						if (PersistentHitResult.Actor.Get() == HitResult.Actor.Get())
						{
							bActorAlreadyInPersistentHits = true;
							break;
						}
					}

					if (bActorAlreadyInPersistentHits)
					{
						continue;
					}

					if (PersistentHitResults.Num() >= MaxHitResultsPerTrace)
					{
						//by GASShooter
						//	Treat PersistentHitResults like a queue, remove first element
						//和訳
						//	PersistentHitResults  をキューのように扱い、最初の要素を削除する。
						PersistentHitResults.RemoveAt(0);
					}

					PersistentHitResults.Add(HitResult);
				}
			}
			else
			{
				//by GASShooter
				//	ReticleActors for PersistentHitResults are handled later
				//和訳
				//	PersistentHitResults 用の  ReticleActors は後で処理されます。
				int32 ReticleIndex = TraceIndex * MaxHitResultsPerTrace + j;
				if (ReticleIndex < ReticleActors.Num())
				{
					if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActors[ReticleIndex].Get())
					{
						const bool bHitActor = HitResult.Actor != nullptr;

						if (bHitActor && !HitResult.bBlockingHit)
						{
							LocalReticleActor->SetActorHiddenInGame(false);

							const FVector ReticleLocation = (bHitActor && LocalReticleActor->bSnapToTargetedActor) ? HitResult.Actor->GetActorLocation() : HitResult.Location;

							LocalReticleActor->SetActorLocation(ReticleLocation);
							LocalReticleActor->SetIsTargetAnActor(bHitActor);
						}
						else
						{
							LocalReticleActor->SetActorHiddenInGame(true);
						}
					}
				}
			}
		} // for TraceHitResults

		if (!bUsePersistentHitResults)
		{
			if (TraceHitResults.Num() < ReticleActors.Num())
			{
				//by GASShooter
				//	We have less hit results than ReticleActors, hide the extra ones
				//和訳
				//	ReticleActors に比べて hit が少ないので、余分なものを隠します。
				for (int32 j = TraceHitResults.Num(); j < ReticleActors.Num(); j++)
				{
					if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActors[j].Get())
					{
						LocalReticleActor->SetIsTargetAnActor(false);
						LocalReticleActor->SetActorHiddenInGame(true);
					}
				}
			}
		}

		if (TraceHitResults.Num() < 1)
		{
			//by GASShooter
			//	If there were no hits, add a default HitResult at the end of the trace
			//和訳
			//	ヒットしなかった場合は、トレースの最後にデフォルトの HitResult を追加します。
			FHitResult HitResult;
			//by GASShooter
			//	Start param could be player ViewPoint. We want HitResult to always display the StartLocation.
			//和訳
			//	パラメータ Start はプレイヤーのビューポイントになります。 HitResult が常に StartLocation を示させたい。
			HitResult.TraceStart = StartLocation.GetTargetingTransform().GetLocation();
			HitResult.TraceEnd = TraceEnd;
			HitResult.Location = TraceEnd;
			HitResult.ImpactPoint = TraceEnd;
			TraceHitResults.Add(HitResult);

			if (bUsePersistentHitResults && PersistentHitResults.Num() < 1)
			{
				PersistentHitResults.Add(HitResult);
			}
		}

		ReturnHitResults.Append(TraceHitResults);
	} // for NumberOfTraces

	//by GASShooter
	//	Reminder: if bUsePersistentHitResults, Number of Traces = 1
	//和訳
	//	リマインダー : bUsePersistentHitResults が true  の場合、 Traces の値は 1
	if (bUsePersistentHitResults && MaxHitResultsPerTrace > 0)
	{
		//by GASShooter
		//	Handle ReticleActors
		//和訳
		//	ReticleActors を処理します。
		for (int32 PersistentHitResultIndex = 0; PersistentHitResultIndex < PersistentHitResults.Num(); PersistentHitResultIndex++)
		{
			FHitResult& HitResult = PersistentHitResults[PersistentHitResultIndex];

			//by GASShooter
			//	Update TraceStart because old persistent HitResults will have their original TraceStart and the player could have moved since then
			//和訳
			//	TraceStart を更新します。なぜなら古い persistent HitResults は初期の TraceStart を所持しており、プレイヤーがそのトゴ動いている可能性があるためです。
			HitResult.TraceStart = StartLocation.GetTargetingTransform().GetLocation();

			if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActors[PersistentHitResultIndex].Get())
			{
				const bool bHitActor = HitResult.Actor != nullptr;

				if (bHitActor && !HitResult.bBlockingHit)
				{
					LocalReticleActor->SetActorHiddenInGame(false);

					const FVector ReticleLocation = (bHitActor && LocalReticleActor->bSnapToTargetedActor) ? HitResult.Actor->GetActorLocation() : HitResult.Location;

					LocalReticleActor->SetActorLocation(ReticleLocation);
					LocalReticleActor->SetIsTargetAnActor(bHitActor);
				}
				else
				{
					LocalReticleActor->SetActorHiddenInGame(true);
				}
			}
		}

		if (PersistentHitResults.Num() < ReticleActors.Num())
		{
			//by GASShooter
			//	We have less hit results than ReticleActors, hide the extra ones
			//和訳
			//	ReticleActors に比べて hit が少ないので、余分なものを隠します。
			for (int32 PersistentHitResultIndex = PersistentHitResults.Num(); PersistentHitResultIndex < ReticleActors.Num(); PersistentHitResultIndex++)
			{
				if (AGameplayAbilityWorldReticle* LocalReticleActor = ReticleActors[PersistentHitResultIndex].Get())
				{
					LocalReticleActor->SetIsTargetAnActor(false);
					LocalReticleActor->SetActorHiddenInGame(true);
				}
			}
		}

		return PersistentHitResults;
	}

	return ReturnHitResults;
}

AGameplayAbilityWorldReticle* AGHOGATA_Trace::SpawnReticleActor(FVector Location, FRotator Rotation)
{
	if (ReticleClass)
	{
		AGameplayAbilityWorldReticle* SpawnedReticleActor = GetWorld()->SpawnActor<AGameplayAbilityWorldReticle>(ReticleClass, Location, Rotation);
		if (SpawnedReticleActor)
		{
			SpawnedReticleActor->InitializeReticle(this, MasterPC, ReticleParams);
			SpawnedReticleActor->SetActorHiddenInGame(true);
			ReticleActors.Add(SpawnedReticleActor);

			//by GASShooter
			//	This is to catch cases of playing on a listen server where we are using a replicated reticle actor.
			//	(In a client controlled player, this would only run on the client and therefor never replicate. 
			//	If it runs on a listen server, the reticle actor may replicate. 
			//	We want consistancy between client/listen server players.
			//	Just saying 'make the reticle actor non replicated' isnt a good answer, 
			//	since we want to mix and match reticle actors and there may be other targeting types that want to replicate the same reticle actor class).
			//和訳
			//	これはリッスンサーバー上でプレイする場合に必要です。そこではレプリケーションされたレティクルアクターを使用します。
			//	（クライアント制御のプレイヤーでは、これはクライアント上のみで実行され、したがってレプリケーションされることはありません。
			//	これがリッスンサーバー上で実行される場合、レティクルアクターはレプリケーションされる可能性があります。
			//	私達はクライアント/リッスンサーバーのプレイヤー間で一貫性を求めています。
			//	「レティクルアクターをレプリケーションされないようにする」というだけでは良い答えではありません。
			//	なぜなら、私達はレティクルアクターを混ぜ合わせたいし、同じレティクルアクタークラスをレプリケーションしたい他のターゲットタイプでがあるかもしれないからです。）
			if (!ShouldProduceTargetDataOnServer)
			{
				SpawnedReticleActor->SetReplicates(false);
			}

			return SpawnedReticleActor;
		}
	}

	return nullptr;
}

void AGHOGATA_Trace::DestroyReticleActors()
{
	for (int32 i = ReticleActors.Num() - 1; i >= 0; i--)
	{
		if (ReticleActors[i].IsValid())
		{
			ReticleActors[i].Get()->Destroy();
		}
	}

	ReticleActors.Empty();
}
