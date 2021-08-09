// (C) Sentya Anko 2021


#include "Characters/AbilityTasks/GHOAbilityTask_WaitInteractableTarget.h"
#include "DrawDebugHelpers.h"
#include "Characters/Interfaces/GHOInteractable.h"
#include "Settings/GHOCollisions.h"
#include "Characters/GHOHeroCharacterBase.h"

UGHOAbilityTask_WaitInteractableTarget::UGHOAbilityTask_WaitInteractableTarget()
{
	bTraceAffectsAimPitch = true;
}

UGHOAbilityTask_WaitInteractableTarget* UGHOAbilityTask_WaitInteractableTarget::WaitForInteractableTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, FCollisionProfileName TraceProfile, float MaxRange, float TimerPeriod, bool bShowDebug)
{
	if (!OwningAbility)
		return nullptr;
	//by GASShooter
	//	Register for task list here, providing a given FName as a key
	//和訳
	//	与えられた FName をキーとしてここでタスクリストに登録します。
	UGHOAbilityTask_WaitInteractableTarget* MyObj = NewAbilityTask<UGHOAbilityTask_WaitInteractableTarget>(OwningAbility, TaskInstanceName);
	MyObj->Initialize(OwningAbility, TaskInstanceName, TraceProfile, MaxRange, TimerPeriod, bShowDebug);
	return MyObj;
}

void UGHOAbilityTask_WaitInteractableTarget::Initialize(UGameplayAbility* InOwningAbility, FName InTaskInstanceName, FCollisionProfileName InTraceProfile, float InMaxRange, float InTimerPeriod, bool bInShowDebug)
{
	TraceProfile = InTraceProfile;
	MaxRange = InMaxRange;
	TimerPeriod = InTimerPeriod;
	bShowDebug = bInShowDebug;

	AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(InOwningAbility->GetCurrentActorInfo()->AvatarActor);

#if 0 // TODO: first person view
	StartLocation1P = FGameplayAbilityTargetingLocationInfo();
	StartLocation1P.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	StartLocation1P.SourceComponent = Hero->GetFirstPersonMesh();
	StartLocation1P.SourceSocketName = "WeaponPoint";
	StartLocation1P.SourceAbility = InOwningAbility;

	StartLocation3P = FGameplayAbilityTargetingLocationInfo();
	StartLocation3P.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	StartLocation3P.SourceComponent = Hero->GetThirdPersonMesh();
	StartLocation3P.SourceSocketName = "Head";
	StartLocation3P.SourceAbility = InOwningAbility;
#else
	StartLocation3P = FGameplayAbilityTargetingLocationInfo();
	StartLocation3P.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
	StartLocation3P.SourceComponent = Hero->GetMesh();
	StartLocation3P.SourceSocketName = "Head";
	StartLocation3P.SourceAbility = InOwningAbility;
#endif
}

void UGHOAbilityTask_WaitInteractableTarget::Activate()
{
	UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(TraceTimerHandle, this, &UGHOAbilityTask_WaitInteractableTarget::PerformTrace, TimerPeriod, true);
}

void UGHOAbilityTask_WaitInteractableTarget::OnDestroy(bool AbilityEnded)
{
	UWorld* World = GetWorld();
	World->GetTimerManager().ClearTimer(TraceTimerHandle);

	Super::OnDestroy(AbilityEnded);
}

void UGHOAbilityTask_WaitInteractableTarget::LineTrace(FHitResult& OutHitResult, const UWorld* World, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params, bool bLookForInteractableActor) const
{
	check(World);

	TArray<FHitResult> HitResults;
	World->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);

	OutHitResult.TraceStart = Start;
	OutHitResult.TraceEnd = End;

	for (int32 HitIdx = 0; HitIdx < HitResults.Num(); ++HitIdx)
	{
		const FHitResult& Hit = HitResults[HitIdx];

		if (!Hit.Actor.IsValid() || Hit.Actor != Ability->GetCurrentActorInfo()->AvatarActor.Get())
		{
			//by GASShooter
			//	If bLookForInteractableActor is false, we're looking for an endpoint to trace to
			//和訳
			//	bLookForInteractableActor が false の場合、トレースするエンドポイントを探します。
			if (bLookForInteractableActor && Hit.Actor.IsValid())
			{
				//by GASShooter
				//	bLookForInteractableActor is true, hit component must overlap COLLISION_INTERACTABLE trace channel
				//	This is so that a big Actor like a computer can have a small interactable button.
				//和訳
				//	bLookForInteractableActor が true の場合、ヒットコンポーネントは COLLISION_INTERACTABLE トレースチャンネルと重ならなくてはならない！
				//	これはコンピュータのような大きなアクターに小さなインタラクト可能なボタンをもたせるためです。
				//	This is so that a big Actor like a computer can have a small interactable button.
				if (Hit.Component.IsValid() && Hit.Component.Get()->GetCollisionResponseToChannel(COLLISION_INTERACTABLE)
					== ECollisionResponse::ECR_Overlap)
				{
					//by GASShooter
					//	Component/Actor must be available to interact
					//和訳
					//	コンポーネント/アクターがインタラクト可能であること
					bool bIsInteractable = Hit.Actor.Get()->Implements<UGHOInteractable>();

					if (bIsInteractable && IGHOInteractable::Execute_IsAvailableForInteraction(Hit.Actor.Get(), Hit.Component.Get()))
					{
						OutHitResult = Hit;
						//by GASShooter
						//	treat it as a blocking hit
						//和訳
						//	ブロッキングヒットとして扱う
						OutHitResult.bBlockingHit = true;
						return;
					}
				}

				OutHitResult.TraceEnd = Hit.Location;
				//by GASShooter
				//	False means it isn't valid to interact with
				//和訳
				//	False はインタラクトが有効でないことを意味します。
				OutHitResult.bBlockingHit = false;
				return;
			}

			//by GASShooter
			//	This is for the first line trace to get an end point to trace to
			//	!Hit.Actor.IsValid() implies we didn't hit anything so return the endpoint as a blocking hit
			//	Or if we hit something else
			//和訳
			//	これは「最初のライントレースでエンドポイントを取得し、 !Hit.Actor.IsValid() にトレースした」ときのためのもので、それは何もヒットしていないことを意味するので、エンドポイントをブロックヒットとして返します。
			//	あるいは何か他のものにヒットした
			OutHitResult = Hit;
			//by GASShooter
			//	treat it as a blocking hit
			//和訳
			//	ブロッキングヒットとして扱う
			OutHitResult.bBlockingHit = true;
			return;
		}
	}
}

void UGHOAbilityTask_WaitInteractableTarget::AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, FVector& OutTraceEnd, bool bIgnorePitch) const
{
	//by GASShooter
	//	Server and launching client only
	//和訳
	//	サーバーと起動クライアントのみ
	if (!Ability)
	{
		return;
	}

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	//by GASShooter
	//	Default to TraceStart if no PlayerController
	//和訳
	//	PlayerController がない場合はデフォルトで TraceStart になります。
	FVector ViewStart = TraceStart;
	FRotator ViewRot(0.0f);
	if (PC)
	{
		PC->GetPlayerViewPoint(ViewStart, ViewRot);
	}

	const FVector ViewDir = ViewRot.Vector();
	FVector ViewEnd = ViewStart + (ViewDir * MaxRange);

	ClipCameraRayToAbilityRange(ViewStart, ViewDir, TraceStart, MaxRange, ViewEnd);

	FHitResult HitResult;
	LineTrace(HitResult, InSourceActor->GetWorld(), ViewStart, ViewEnd, TraceProfile.Name, Params, false);

	const bool bUseTraceResult = HitResult.bBlockingHit && (FVector::DistSquared(TraceStart, HitResult.Location) <= (MaxRange * MaxRange));

	const FVector AdjustedEnd = (bUseTraceResult) ? HitResult.Location : ViewEnd;

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
			//	角度に変換して元のピッチを使用
			const FRotator OriginalAimRot = OriginalAimDir.Rotation();

			FRotator AdjustedAimRot = AdjustedAimDir.Rotation();
			AdjustedAimRot.Pitch = OriginalAimRot.Pitch;

			AdjustedAimDir = AdjustedAimRot.Vector();
		}
	}

	OutTraceEnd = TraceStart + (AdjustedAimDir * MaxRange);
}

bool UGHOAbilityTask_WaitInteractableTarget::ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition) const
{
	FVector CameraToCenter = AbilityCenter - CameraLocation;
	const float DotToCenter = FVector::DotProduct(CameraToCenter, CameraDirection);
	//by GASShooter
	//	If this fails, we're pointed away from the center, but we might be inside the sphere and able to find a good exit point.
	//和訳
	//	これに失敗した場合、中心から離れてはいますが、球体の内側に居て、うまく出口を見つけられるかもしれません。
	if (DotToCenter >= 0)
	{
		const float DistanceSquared = CameraToCenter.SizeSquared() - (DotToCenter * DotToCenter);
		const float RadiusSquared = (AbilityRange * AbilityRange);
		if (DistanceSquared <= RadiusSquared)
		{
			const float DistanceFromCamera = FMath::Sqrt(RadiusSquared - DistanceSquared);
			//by GASShooter
			//	Subtracting instead of adding will get the other intersection point
			//和訳
			//	加算する代わりに減算すると、他の交点が得られます。
			const float DistanceAlongRay = DotToCenter + DistanceFromCamera;
			//by GASShooter
			//	Cam aim point clipped to range sphere
			//和訳
			//	カメラのエイムポイントが範囲の球にクリップされる
			ClippedPosition = CameraLocation + (DistanceAlongRay * CameraDirection);
			return true;
		}
	}
	return false;
}

void UGHOAbilityTask_WaitInteractableTarget::PerformTrace()
{
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;

	AActor* SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	if (!SourceActor)
	{
		// Hero is dead
		//UE_LOG(LogTemp, Error, TEXT("%s %s SourceActor was null"), *FString(__FUNCTION__), *UGSBlueprintFunctionLibrary::GetPlayerEditorWindowRole(GetWorld()));
		return;
	}

	ActorsToIgnore.Add(SourceActor);

#if 0 // TODO: first person view
	//by GASShooter
	//	Check player's perspective, could be 1P or 3P
	//和訳
	//	プレイヤーの始点を確認する、1P または 3P の可能性がある。
	AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(SourceActor);
	if (Hero->IsInFirstPersonPerspective())
	{
		StartLocation = StartLocation1P;
	}
	else
	{
		StartLocation = StartLocation3P;
	}
#else
	StartLocation = StartLocation3P;
#endif

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AGameplayAbilityTargetActor_SingleLineTrace), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActors(ActorsToIgnore);

	//by GASShooter
	//	Calculate TraceEnd
	//和訳
	//	TraceEnd を計算します。
	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();
	FVector TraceEnd;
	//by GASShooter
	//	Effective on server and launching client only
	//和訳
	//	サーバーと起動クライアントのみで有効
	AimWithPlayerController(SourceActor, Params, TraceStart, TraceEnd);

	// ------------------------------------------------------

	FHitResult ReturnHitResult;
	LineTrace(ReturnHitResult, GetWorld(), TraceStart, TraceEnd, TraceProfile.Name, Params, true);

	//by GASShooter
	//	Default to end of trace line if we don't hit a valid, available Interactable Actor
	//	bBlockingHit = valid, available Interactable Actor
	//和訳
	//	有効で利用可能なインタラクト可能なアクターにヒットしなかった場合、デフォルトではトレースラインの終わりになります。
	//	bBlockingHit = valid で、利用可能なインタラクト可能なアクターです。
	if (!ReturnHitResult.bBlockingHit)
	{
		//by GASShooter
		//	No valid, available Interactable Actor
		//和訳
		//	有効で利用可能なインタラクト可能なアクターがありません。

		ReturnHitResult.Location = TraceEnd;
		if (TargetData.Num() > 0 && TargetData.Get(0)->GetHitResult()->Actor.Get())
		{
			//by GASShooter
			//	Previous trace had a valid Interactable Actor, now we don't have one
			//	Broadcast last valid target
			//和訳
			//	以前のトレースには有効なインタラクト可能なアクターがありましたが、現在ではいません。
			//	最後の有効なターゲットをブロードキャストします。
			LostInteractableTarget.Broadcast(TargetData);
		}

		TargetData = MakeTargetData(ReturnHitResult);
	}
	else
	{
		//by GASShooter
		//	Valid, available Interactable Actor
		//和訳
		//	有効で利用可能なインタラクト可能なアクターがあります。

		bool bBroadcastNewTarget = true;

		if (TargetData.Num() > 0)
		{
			const AActor* OldTarget = TargetData.Get(0)->GetHitResult()->Actor.Get();

			if (OldTarget == ReturnHitResult.Actor.Get())
			{
				//by GASShooter
				//	Old target is the same as the new target, don't broadcast the target
				//和訳
				//	古いターゲットは新しいターゲットと同じなので、ターゲットをブロードキャストしません。
				bBroadcastNewTarget = false;
			}
			else if (OldTarget)
			{
				//by GASShooter
				//	Old target exists and is different from the new target
				//	Broadcast last valid target
				//和訳
				//	古いターゲットが存在し、新しいターゲットとは異なります。
				//	最後の有効なターゲットをブロードキャストします。
				LostInteractableTarget.Broadcast(TargetData);
			}
		}

		if (bBroadcastNewTarget)
		{
			//by GASShooter
			//	Broadcast new valid target
			//和訳
			//	新しい有効なターゲットをブロードキャストします。
			TargetData = MakeTargetData(ReturnHitResult);
			FoundNewInteractableTarget.Broadcast(TargetData);
		}
	}

#if ENABLE_DRAW_DEBUG
	if (bShowDebug)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, TimerPeriod);

		if (ReturnHitResult.bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), ReturnHitResult.Location, 20.0f, 16, FColor::Red, false, TimerPeriod);
		}
		else
		{
			DrawDebugSphere(GetWorld(), ReturnHitResult.TraceEnd, 20.0f, 16, FColor::Green, false, TimerPeriod);
		}
	}
#endif // ENABLE_DRAW_DEBUG
}

FGameplayAbilityTargetDataHandle UGHOAbilityTask_WaitInteractableTarget::MakeTargetData(const FHitResult& HitResult) const
{
	/**
	by GASShooter
		Note : This will be cleaned up by the FGameplayAbilityTargetDataHandle(via an internal TSharedPtr)
	和訳
		注：これは FGameplayAbilityTargetDataHandle によって（ TSharedPtr を介して）クリーンアップされます。
	*/
	return StartLocation.MakeTargetDataHandleFromHitResult(Ability, HitResult);
}
