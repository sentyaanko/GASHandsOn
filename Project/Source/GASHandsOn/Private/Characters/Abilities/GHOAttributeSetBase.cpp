// (C) Sentya Anko 2021


#include "Characters/Abilities/GHOAttributeSetBase.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Characters/GHOCharacterBase.h"
#include "Player/GHOPlayerController.h"
#include "Net/UnrealNetwork.h"


UGHOAttributeSetBase::UGHOAttributeSetBase()
{
}

void UGHOAttributeSetBase::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, HealthMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, ManaMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, ManaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, StaminaMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGHOAttributeSetBase, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UGHOAttributeSetBase::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		PostGameplayEffectExecute_Damage(Data);
	}// Damage
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		/*
		by GASDocumentation
			Handle other health changes.
			Health loss should go through Damage.
		和訳
			他の Health の変更を扱う。
			Health の減少は Damage を経由する必要があります。
		*/
		/*
		解説
			これを実行することで、レプリケーションが発生します。
			実行しない場合、例えば Health が HealthRegen の効果でクライアントだけ HealthMax を突破した状態になります。
		*/
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetHealthMax()));
	} // Health
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		/*
		by GASDocumentation
			Handle mana changes.
		和訳
			Mana の変更を扱う。
		*/
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetManaMax()));
	} // Mana
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		/*
		by GASDocumentation
			Handle stamina changes.
		和訳
			Stamina の変更を扱う。
		*/
		// 
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetStaminaMax()));
	}
}

void UGHOAttributeSetBase::PostGameplayEffectExecute_Damage(const struct FGameplayEffectModCallbackData& Data)
{
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	//const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	//FGameplayTagContainer SpecAssetTags;
	//Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	/*
	by GASDocumentation
		Get the Target actor, which should be our owner
	和訳
		owner であるはずのターゲットアクターを取得します。
	*/
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AGHOCharacterBase* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AGHOCharacterBase>(TargetActor);
	}

	/*
	by GASDocumentation
		Get the Source actor
	和訳
		ソースアクターを取得します。
	*/
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	AGHOCharacterBase* SourceCharacter = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		/*
		by GASDocumentation
			Use the controller to find the source pawn
		和訳
			コントローラーを使用してソースのポーンを見つけます。
		*/
		if (SourceController)
		{
			SourceCharacter = Cast<AGHOCharacterBase>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<AGHOCharacterBase>(SourceActor);
		}

		/*
		by GASDocumentation
			Set the causer actor based on context if it's set
		和訳
			因果関係のあるアクターが設定されていれば、それに基づいて設定します。
		*/
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	/*
	by GASDocumentation
		Store a local copy of the amount of damage done and clear the damage attribute
	和訳
		ダメージ量のローカルコピーを保存し、ダメージアトリビュートをクリアする。
	*/
	const float LocalDamageDone = GetDamage();
	SetDamage(0.f);

	if (LocalDamageDone > 0.0f)
	{
		/*
		by GASDocumentation
			If character was alive before damage is added, handle damage
			This prevents damage being added to dead things and replaying death animations
		和訳
			ダメージが追加される前にキャラクターが生きていた場合、ダメージ処理をする。
			これにより死んだものにダメージが追加され、死亡アニメーションがリプレイされることを防ぎます。
		*/
		const bool WasAlive = TargetCharacter? TargetCharacter->IsAlive(): true;

		if (!TargetCharacter->IsAlive())
		{
			//UE_LOG(LogTemp, Warning, TEXT("%s() %s is NOT alive when receiving damage"), TEXT(__FUNCTION__), *TargetCharacter->GetName());
		}

		/*
		by GASDocumentation
			Apply the health change and then clamp it
		和訳
			Health の変化を適用してからクランプする。
		*/
		const float NewHealth = GetHealth() - LocalDamageDone;
		SetHealth(FMath::Clamp(NewHealth, 0.0f, GetHealthMax()));
	}
}

void UGHOAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	/*
	by GASDocumentation
		This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	和訳
		アトリビュートが変更する度に呼び出されるので、 health/mana の最大値が変更される場合は現在の合計値をスケーリングさせて一致させます。
	*/
	Super::PreAttributeChange(Attribute, NewValue);

	/*
	by GASDocumentation
		If a Max value changes, adjust current to keep Current % of Current to Max
		GetHealthMaxAttribute comes from the Macros defined at the top of the header
	和訳
		Max 値が変更された場合、 Current の Max に対する % を保つように Current を調整する。
		GetHealthMaxAttribute() はヘッダの先頭で定義されたマクロから来ています。
	*/
	if (Attribute == GetHealthMaxAttribute())
	{
		AdjustAttributeForMaxChange(Health, HealthMax, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetManaMaxAttribute())
	{
		AdjustAttributeForMaxChange(Mana, ManaMax, NewValue, GetManaAttribute());
	}
	else if (Attribute == GetStaminaMaxAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, StaminaMax, NewValue, GetStaminaAttribute());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		/*
		by GASDocumentation
			Cannot slow less than 150 units/s and cannot boost more than 1000 units/s
		和訳
			150 units/s 以下の減速と 1000 units/s 以上の加速を制限する。
		*/
		NewValue = FMath::Clamp<float>(NewValue, 150.f, 1000.f);
	}
	/*
	解説
		ここでクランプを行わない場合、以下のような処理の流れとなります。
			* サーバーサイドでは変更が行われた後に GetLifetimeReplicatedProps でクランプ（つまりは２回目の変更）が行われる
			* クライアントサイドではクランプ後の値のみ受け取る
		要は、サーバーサイドでは AGHOPlayerState が所持するデリゲートなどで変更通知が２回来るということです。
		[「GASDocumentation」の「4.4.6 PostGameplayEffectExecute()」](https://github.com/tranek/GASDocumentation#concepts-as-postgameplayeffectexecute) [(和訳)](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#concepts-as-postgameplayeffectexecute) も参照。
		GASDocumentation ではここでのクランプを行っていません。
		GASHandsOn では以下を理由に、クランプするようにしています。
			* ゲームルール的にクランプしても問題ないこと
			* サーバーサイドのデリゲートが一時的にでも想定した上限を超えること
				* 例えば Health が最大のときでも HealthRegen の効果で一秒ごとに HealthMax を超える。この変更はデリゲートにも通知が届く。
	*/
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetHealthMax());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetManaMax());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, GetStaminaMax());
	}
}

void UGHOAttributeSetBase::InitializeAttributesOnSpawned()
{
	// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
	SetHealth(GetHealthMax());
	SetMana(GetManaMax());
	SetStamina(GetStaminaMax());
}

bool UGHOAttributeSetBase::IsAlive()const
{
	return GetHealth() > 0.f;
}

FGHOHUDWidgetParameter UGHOAttributeSetBase::GetHUDParameter()const
{
	return FGHOHUDWidgetParameter
	(
		GetCharacterLevel(),
		GetMoveSpeed(),
		GetHealth(),
		GetHealthMax(),
		GetHealthRegenRate(),
		GetMana(),
		GetManaMax(),
		GetManaRegenRate(),
		GetStamina(),
		GetStaminaMax(),
		GetStaminaRegenRate()
	);
}

void UGHOAttributeSetBase::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UGHOAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Health, OldHealth);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldHealth.GetCurrentValue(), Health.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_HealthMax(const FGameplayAttributeData& OldHealthMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, HealthMax, OldHealthMax);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldHealthMax.GetCurrentValue(), HealthMax.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, HealthRegenRate, OldHealthRegenRate);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldHealthRegenRate.GetCurrentValue(), HealthRegenRate.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Mana, OldMana);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldMana.GetCurrentValue(), Mana.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_ManaMax(const FGameplayAttributeData& OldManaMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, ManaMax, OldManaMax);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldManaMax.GetCurrentValue(), ManaMax.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, ManaRegenRate, OldManaRegenRate);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldManaRegenRate.GetCurrentValue(), ManaRegenRate.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, Stamina, OldStamina);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldStamina.GetCurrentValue(), Stamina.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_StaminaMax(const FGameplayAttributeData& OldStaminaMax)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, StaminaMax, OldStaminaMax);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldStaminaMax.GetCurrentValue(), StaminaMax.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, StaminaRegenRate, OldStaminaRegenRate);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldStaminaRegenRate.GetCurrentValue(), StaminaRegenRate.GetCurrentValue());
}

void UGHOAttributeSetBase::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGHOAttributeSetBase, MoveSpeed, OldMoveSpeed);

	UE_LOG(LogTemp, Warning, TEXT("%s() %f -> %f"), *FString(__FUNCTION__), OldMoveSpeed.GetCurrentValue(), MoveSpeed.GetCurrentValue());
}

