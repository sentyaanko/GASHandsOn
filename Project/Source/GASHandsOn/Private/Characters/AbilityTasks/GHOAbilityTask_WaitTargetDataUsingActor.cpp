// (C) Sentya Anko 2021

#include "Characters/AbilityTasks/GHOAbilityTask_WaitTargetDataUsingActor.h"
#include "Characters/TargetActors/GHOGATA_Trace.h"


/**
解説
	UAbilityTask_WaitTargetData を元にしているため、
	コードのつくりが似ている箇所が多々あります。

	基本的には bCreateKeyIfNotValidForMorePredicting が絡むところ以外は同じです。
 */

UGHOAbilityTask_WaitTargetDataUsingActor* UGHOAbilityTask_WaitTargetDataUsingActor::WaitTargetDataWithReusableActor(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, AGameplayAbilityTargetActor* InTargetActor, bool bCreateKeyIfNotValidForMorePredicting)
{
	/*
	by Epic
		Register for task list here, providing a given FName as a key
	和訳
		与えられた FName をキーとして、ここでタスクリストに登録します。
	*/
	UGHOAbilityTask_WaitTargetDataUsingActor* MyObj = NewAbilityTask<UGHOAbilityTask_WaitTargetDataUsingActor>(OwningAbility, TaskInstanceName);
	MyObj->Initialize(InTargetActor, ConfirmationType, bCreateKeyIfNotValidForMorePredicting);
	return MyObj;
}

void UGHOAbilityTask_WaitTargetDataUsingActor::Initialize(AGameplayAbilityTargetActor* InTargetActor, TEnumAsByte<EGameplayTargetingConfirmation::Type> InConfirmationType, bool bInCreateKeyIfNotValidForMorePredicting)
{
	TargetActor = InTargetActor;
	ConfirmationType = InConfirmationType;
	bCreateKeyIfNotValidForMorePredicting = bInCreateKeyIfNotValidForMorePredicting;
}

void UGHOAbilityTask_WaitTargetDataUsingActor::Activate()
{
/*
解説
	この関数は構成が結構違う。

	主に IsPendingKill() の呼び出し、 RegisterTargetDataCallbacks() の呼び出しなど。
*/
	if (IsPendingKill())
	{
		return;
	}

	if (Ability && TargetActor)
	{
		InitializeTargetActor();
		RegisterTargetDataCallbacks();
		FinalizeTargetActor();
	}
	else
	{
		EndTask();
	}
}

void UGHOAbilityTask_WaitTargetDataUsingActor::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	check(AbilitySystemComponent);

	FGameplayAbilityTargetDataHandle MutableData = Data;
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	/*
	by Epic
		Call into the TargetActor to sanitize/verify the data. 
		If this returns false, we are rejecting the replicated target data and will treat this as a cancel.
		
		This can also be used for bandwidth optimizations. 
		OnReplicatedTargetDataReceived could do an actual trace/check/whatever server side and use that data. 
		So rather than having the client send that data explicitly, the client is basically just sending a 'confirm' and the server is now going to do the work in OnReplicatedTargetDataReceived.
	和訳
		データをサニタイズ/ベリファイするために TargetActor を呼び出します。
		false が返った場合、レプリケーションされたデータを拒否し、キャンセルとして扱います。
		
		これは帯域幅の最適化にも使用できます。
		OnReplicatedTargetDataReceived はサーバー側でトレース/チェック/その他の処理を行い、そのデータを使用することができます。
		つまり、クライアントがそのデータを明示的に送信するのではなく、基本的にクライアントは「確認」を送信するだけで、サーバーは OnReplicatedTargetDataReceived で作業を行うことになります。
	*/
	if (TargetActor && !TargetActor->OnReplicatedTargetDataReceived(MutableData))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			Cancelled.Broadcast(MutableData);
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ValidData.Broadcast(MutableData);
		}
	}

	if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
	{
		EndTask();
	}
}

void UGHOAbilityTask_WaitTargetDataUsingActor::OnTargetDataReplicatedCancelledCallback()
{
	check(AbilitySystemComponent);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	EndTask();
}

void UGHOAbilityTask_WaitTargetDataUsingActor::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data)
{
/*
解説
	この関数は構成がちょっとだけ違う。

	主に ScopedPrediction コンストラクタの第に引数。
*/
	check(AbilitySystemComponent);
	if (!Ability)
	{
		return;
	}

	FScopedPredictionWindow	ScopedPrediction(AbilitySystemComponent,
		ShouldReplicateDataToServer() && (bCreateKeyIfNotValidForMorePredicting && !AbilitySystemComponent->ScopedPredictionKey.IsValidForMorePrediction()));

	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (IsPredictingClient())
	{
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			/*
			by Epic
				Fixme: where would this be useful?
			和訳
				Fixme: これはどこで役に立つのでしょうか？
			*/
			FGameplayTag ApplicationTag;
			AbilitySystemComponent->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), Data, ApplicationTag, AbilitySystemComponent->ScopedPredictionKey);
		}
		else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			/*
			by Epic
				We aren't going to send the target data, but we will send a generic confirmed message.
			和訳
				ターゲットデータを送らないが、一般的な確認メッセージを送る。
			*/
			AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data);
	}

	if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
	{
		EndTask();
	}
}

void UGHOAbilityTask_WaitTargetDataUsingActor::OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	check(AbilitySystemComponent);

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent, IsPredictingClient());

	if (IsPredictingClient())
	{
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			AbilitySystemComponent->ServerSetReplicatedTargetDataCancelled(GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
		else
		{
			/*
			by Epic
				We aren't going to send the target data, but we will send a generic confirmed message.
			和訳
				ターゲットデータを送らないが、一般的な確認メッセージを送る。
			*/
			AbilitySystemComponent->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericCancel, GetAbilitySpecHandle(), GetActivationPredictionKey(), AbilitySystemComponent->ScopedPredictionKey);
		}
	}
	Cancelled.Broadcast(Data);
	EndTask();
}

void UGHOAbilityTask_WaitTargetDataUsingActor::ExternalConfirm(bool bEndTask)
{
	check(AbilitySystemComponent);
	if (TargetActor)
	{
		if (TargetActor->ShouldProduceTargetData())
		{
			TargetActor->ConfirmTargetingAndContinue();
		}
	}
	Super::ExternalConfirm(bEndTask);
}

void UGHOAbilityTask_WaitTargetDataUsingActor::ExternalCancel()
{
	check(AbilitySystemComponent);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	Super::ExternalCancel();
}

void UGHOAbilityTask_WaitTargetDataUsingActor::InitializeTargetActor() const
{
	check(TargetActor);
	check(Ability);

	TargetActor->MasterPC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	/*
	by Epic
		If we spawned the target actor, always register the callbacks for when the data is ready.
	和訳
		ターゲットアクターをスポーンした場合は、データの準備ができた時のコールバックを常に登録しておきます。
	*/
	TargetActor->TargetDataReadyDelegate.AddUObject(const_cast<UGHOAbilityTask_WaitTargetDataUsingActor*>(this), &UGHOAbilityTask_WaitTargetDataUsingActor::OnTargetDataReadyCallback);
	TargetActor->CanceledDelegate.AddUObject(const_cast<UGHOAbilityTask_WaitTargetDataUsingActor*>(this), &UGHOAbilityTask_WaitTargetDataUsingActor::OnTargetDataCancelledCallback);
}

void UGHOAbilityTask_WaitTargetDataUsingActor::FinalizeTargetActor() const
{
/*
解説
	この関数は構成が結構違う。

	主に引数とターゲットアクターの扱いが異なる。
*/
	check(TargetActor);
	check(Ability);

	TargetActor->StartTargeting(Ability);

	if (TargetActor->ShouldProduceTargetData())
	{
		/*
		by Epic
			If instant confirm, then stop targeting immediately.
			Note this is kind of bad: we should be able to just call a static func on the CDO to do this. 
			But then we wouldn't get to set ExposeOnSpawnParameters.
		和訳
			即座に確認された場合は、直ちにターゲティングを停止します。
			Note これはちょっとまずいです : これを行うためには CDO の静的な関数を呼び出すクトができるはずです。
			しかし、そうすると ExposeOnSpawnParameters を設定することができなくなります。
		*/
		if (ConfirmationType == EGameplayTargetingConfirmation::Instant)
		{
			TargetActor->ConfirmTargeting();
		}
		else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			/*
			by Epic
				Bind to the Cancel/Confirm Delegates (called from local confirm or from repped confirm)
			和訳
				キャンセル/確認デリゲートへのバインド（ローカル確認または repped 確認からの呼び出し）
			*/
			TargetActor->BindToConfirmCancelInputs();
		}
	}
}

void UGHOAbilityTask_WaitTargetDataUsingActor::RegisterTargetDataCallbacks()
{
	if (!ensure(IsPendingKill() == false))
	{
		return;
	}

	check(Ability);

	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	const bool bShouldProduceTargetDataOnServer = TargetActor->ShouldProduceTargetDataOnServer;

	/*
	by Epic
		If not locally controlled (server for remote client), see if TargetData was already sent else register callback for when it does get here.
	和訳
		ローカルに制御されていない場合（リモートクライアントのサーバー）、 TargetData がすでに送信されたかを確認し、そうでない場合はここに届いた時のコールバックを登録します。
	*/
	if (!bIsLocallyControlled)
	{
		/*
		by Epic
			Register with the TargetData callbacks if we are expecting client to send them
		和訳
			クライアントからの送信を期待している場合は、 TargetData コールバックに登録します。
		*/
		if (!bShouldProduceTargetDataOnServer)
		{
			FGameplayAbilitySpecHandle	SpecHandle = GetAbilitySpecHandle();
			FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

			/*
			by Epic
				Since multifire is supported, we still need to hook up the callbacks
			和訳
				multifire がサポートされているので、コールバックをフックアップする必要があります。
			*/
			AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UGHOAbilityTask_WaitTargetDataUsingActor::OnTargetDataReplicatedCallback);
			AbilitySystemComponent->AbilityTargetDataCancelledDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UGHOAbilityTask_WaitTargetDataUsingActor::OnTargetDataReplicatedCancelledCallback);

			AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

			SetWaitingOnRemotePlayerData();
		}
	}
}

void UGHOAbilityTask_WaitTargetDataUsingActor::OnDestroy(bool AbilityEnded)
{
/*
解説
	この関数は構成が結構違う。

	主にターゲットアクターの破棄の方法が異なる。
*/
	if (TargetActor)
	{
		AGHOGATA_Trace* TraceTargetActor = Cast<AGHOGATA_Trace>(TargetActor);
		if (TraceTargetActor)
		{
			TraceTargetActor->StopTargeting();
		}
		else
		{
			/*
			by GASShooter
				TargetActor doesn't have a StopTargeting function
			和訳
				TargetActor が StopTargeting 関数を持っていない
			*/
			TargetActor->SetActorTickEnabled(false);

			/*
			by GASShooter
				Clear added callbacks
			和訳
				追加されたコールバックをクリアする
			*/
			TargetActor->TargetDataReadyDelegate.RemoveAll(this);
			TargetActor->CanceledDelegate.RemoveAll(this);

			AbilitySystemComponent->GenericLocalConfirmCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::ConfirmTargeting);
			AbilitySystemComponent->GenericLocalCancelCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::CancelTargeting);
			TargetActor->GenericDelegateBoundASC = nullptr;
		}
	}

	Super::OnDestroy(AbilityEnded);
}

bool UGHOAbilityTask_WaitTargetDataUsingActor::ShouldReplicateDataToServer() const
{
	if (!Ability || !TargetActor)
	{
		return false;
	}

	/*
	by Epic
		Send TargetData to the server IFF we are the client and this isn't a GameplayTargetActor that can produce data on the server	
	和訳
		我々がクライアントであり、サーバー上でデータ生成できる GameplayTargetActor ではない場合、サーバーに TargetData を送信する
	*/
	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (!Info->IsNetAuthority() && !TargetActor->ShouldProduceTargetDataOnServer)
	{
		return true;
	}

	return false;
}
