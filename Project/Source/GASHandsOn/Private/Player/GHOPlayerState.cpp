// (C) Sentya Anko 2021


#include "Player/GHOPlayerState.h"
#include "Characters/Components/GHOAbilitySystemComponent.h"
#include "Characters/AttributeSets/GHOAttributeSetBase.h"
#include "Characters/AttributeSets/GHOAttributeSetAmmo.h"
#include "Characters/GHOHeroCharacterBase.h"
#include "Player/GHOPlayerController.h"
#include "UI/GHOHUDWidget.h"
#include "UI/GHOFloatingStatusBarWidget.h"
#include "Settings/GHOGameplayTags.h"


AGHOPlayerState::AGHOPlayerState()
{
	/*
	by GASDocumentation
		Create ability system component, and set it to be explicitly replicated
	和訳
		AbilitySystemComponent を作成し、明示的にレプリケーションされるように設定します。
	*/
	AbilitySystemComponent = CreateDefaultSubobject< UGHOAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	/*
	by GASDocumentation
		Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies.
		If another GHOPlayerState (Hero) receives a GE, we won't be told about it by the Server.
		Attributes, GameplayTags, and GameplayCues will still replicate to us.
	和訳
		混合モードとは GameplayEffect をシミュレートさせた時、プロキシにレプリケーションするのではなく、 GameplayEffect を自分自身にレプリケーションさせることを意味します。
		他の GHOPlayerState （ヒーロー） が GameplayEffect を受け取った時、サーバーからは通知されません。
		Attributes, GameplayTags, GameplayCues は引き続きレプリケーションされます。
	*/
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	/*
	by GASDocumentation
		Create the attribute set, this replicates by default
		Adding it as a subobject of the owning actor of an AbilitySystemComponent
		automatically registers the AttributeSet with the AbilitySystemComponent
	和訳
		AttributeSet を作成します。これはデフォルトでレプリケーションされます。
		AbilitySystemComponent の Owning Actor のサブオブジェクトとして追加します。
		AttributeSet を AbilitySystemComponent に自動的に登録します。
	*/
	AttributeSetBase = CreateDefaultSubobject< UGHOAttributeSetBase>(TEXT("AttributeSetBase"));

	AttributeSetAmmo = CreateDefaultSubobject<UGHOAttributeSetAmmo>(TEXT("AmmoAttributeSet"));

	/*
	by GASDocumentation
		Set PlayerState's NetUpdateFrequency to the same as the Character.
		Default is very low for PlayerStates and introduces perceived lag in the ability system.
		100 is probably way too high for a shipping game, you can adjust to fit your needs.
	和訳
		PlayerState の NetUpdateFrequency を Character と同じに設定します。
		PlayerStates のデフォルト値は非常に低く、 AbilitySystem に知覚可能なラグをもたらします。
		100 はおそらく出荷ゲームには高すぎるので、ニーズに合わせて調整することが出来ます。
	*/
	NetUpdateFrequency = 100.f;
}

void AGHOPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AGHOPlayerState::HealthChanged);
		HealthMaxChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthMaxAttribute()).AddUObject(this, &AGHOPlayerState::HealthMaxChanged);
		HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthRegenRateAttribute()).AddUObject(this, &AGHOPlayerState::HealthRegenRateChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &AGHOPlayerState::ManaChanged);
		ManaMaxChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaMaxAttribute()).AddUObject(this, &AGHOPlayerState::ManaMaxChanged);
		ManaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaRegenRateAttribute()).AddUObject(this, &AGHOPlayerState::ManaRegenRateChanged);
		StaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetStaminaAttribute()).AddUObject(this, &AGHOPlayerState::StaminaChanged);
		StaminaMaxChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetStaminaMaxAttribute()).AddUObject(this, &AGHOPlayerState::StaminaMaxChanged);
		StaminaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetStaminaRegenRateAttribute()).AddUObject(this, &AGHOPlayerState::StaminaRegenRateChanged);
		ShieldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetShieldAttribute()).AddUObject(this, &AGHOPlayerState::ShieldChanged);
		ShieldMaxChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetShieldMaxAttribute()).AddUObject(this, &AGHOPlayerState::ShieldMaxChanged);
		ShieldRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetShieldRegenRateAttribute()).AddUObject(this, &AGHOPlayerState::ShieldRegenRateChanged);
		XPChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetXPAttribute()).AddUObject(this, &AGHOPlayerState::XPChanged);
		GoldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetGoldAttribute()).AddUObject(this, &AGHOPlayerState::GoldChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetCharacterLevelAttribute()).AddUObject(this, &AGHOPlayerState::CharacterLevelChanged);

		// Tag change callbacks
		StunChangedDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(TAG_State_CrowdControl_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AGHOPlayerState::StunTagChanged);

		// Tag change callbacks
		KnockedDownChangedDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(TAG_State_KnockedDown, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AGHOPlayerState::KnockDownTagChanged);
	}
}

UAbilitySystemComponent* AGHOPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UGHOAttributeSetBase* AGHOPlayerState::GetAttributeSetBase()const
{
	return AttributeSetBase;
}

UGHOAttributeSetAmmo* AGHOPlayerState::GetAttributeSetAmmo()const
{
	return AttributeSetAmmo;
}


void AGHOPlayerState::ShowAbilityConfirmCancelText(bool bShowText)
{
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->ShowAbilityConfirmCancelText(bShowText);
		}
	}
}

void AGHOPlayerState::ShowInteractionPrompt(float InteractionDuration)
{
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->ShowInteractionPrompt(InteractionDuration);
		}
	}
}

void AGHOPlayerState::HideInteractionPrompt()
{
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->HideInteractionPrompt();
		}
	}
}

void AGHOPlayerState::StartInteractionTimer(float InteractionDuration)
{
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->StartInteractionTimer(InteractionDuration);
		}
	}
}

void AGHOPlayerState::StopInteractionTimer()
{
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->StopInteractionTimer();
		}
	}
}

int AGHOPlayerState::GetReservedAmmoWithTag(const FGameplayTag& tag)
{
	FGameplayAttribute Attribute = AttributeSetAmmo->GetReserveAmmoAttributeFromTag(tag);
	return Attribute.IsValid() ? AbilitySystemComponent->GetNumericAttribute(Attribute): 0;
}

void AGHOPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	const float Health = Data.NewValue;
	const float HealthMax = AttributeSetBase ? AttributeSetBase->GetHealthMax(): 1.f;

	if (AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(GetPawn()))
	{
		if (UGHOFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar())
		{
			HeroFloatingStatusBar->SetHealthPercentage(Health / HealthMax);
		}

		if ((Health <= 0.f) && !AbilitySystemComponent->IsDead())
		{
#if 0
			// GASDocumentation
			Hero->Die();
#else
			// GASShotter
			if (!AbilitySystemComponent->IsKnockedDown())
			{
				Hero->KnockDown();
			}
			else
			{
				Hero->FinishDying();
			}
#endif
		}
	}
}

void AGHOPlayerState::HealthMaxChanged(const FOnAttributeChangeData& Data)
{
	const float Health = AttributeSetBase ? AttributeSetBase->GetHealth() : 0.f;
	const float HealthMax = Data.NewValue;

	if (AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(GetPawn()))
	{
		if (UGHOFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar())
		{
			HeroFloatingStatusBar->SetHealthPercentage(Health / HealthMax);
		}
	}
	// Update the HUD
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->SetHealthMax(HealthMax);
		}
	}
}

void AGHOPlayerState::HealthRegenRateChanged(const FOnAttributeChangeData & Data)
{
	const float HealthRegenRate = Data.NewValue;

	// Update the HUD
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->SetHealthRegenRate(HealthRegenRate);
		}
	}
}

void AGHOPlayerState::ManaChanged(const FOnAttributeChangeData & Data)
{
	const float Mana = Data.NewValue;
	const float ManaMax = AttributeSetBase ? AttributeSetBase->GetManaMax() : 1.f;

	// Update floating status bar
	if (AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(GetPawn()))
	{
		if (UGHOFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar())
		{
			HeroFloatingStatusBar->SetManaPercentage(Mana / ManaMax);
		}
	}

	/*
	by GASDocumentation
		Update the HUD
		Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
	和訳
		HUD の更新
		AsyncTaskAttributeChanged ノードを使用して UI 自体で処理する。
	*/
}

void AGHOPlayerState::ManaMaxChanged(const FOnAttributeChangeData & Data)
{
	const float Mana= AttributeSetBase ? AttributeSetBase->GetMana() : 0.f;
	const float ManaMax = Data.NewValue;

	// Update floating status bar
	if (AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(GetPawn()))
	{
		if (UGHOFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar())
		{
			HeroFloatingStatusBar->SetManaPercentage(Mana / ManaMax);
		}
	}

	// Update the HUD
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->SetManaMax(ManaMax);
		}
	}
}

void AGHOPlayerState::ManaRegenRateChanged(const FOnAttributeChangeData & Data)
{
	float ManaRegenRate = Data.NewValue;

	// Update the HUD
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->SetManaRegenRate(ManaRegenRate);
		}
	}
}

void AGHOPlayerState::StaminaChanged(const FOnAttributeChangeData& Data)
{
	const float Stamina = Data.NewValue;

	/*
	by GASDocumentation
		Update the HUD
		Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
	和訳
		HUD の更新
		AsyncTaskAttributeChanged ノードを使用して UI 自体で処理する。
	*/
}

void AGHOPlayerState::StaminaMaxChanged(const FOnAttributeChangeData& Data)
{
	const float StaminaMax = Data.NewValue;

	// Update the HUD
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->SetStaminaMax(StaminaMax);
		}
	}
}

void AGHOPlayerState::StaminaRegenRateChanged(const FOnAttributeChangeData& Data)
{
	const float StaminaRegenRate = Data.NewValue;

	// Update the HUD
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->SetStaminaRegenRate(StaminaRegenRate);
		}
	}
}

void AGHOPlayerState::ShieldChanged(const FOnAttributeChangeData & Data)
{
	const float Shield = Data.NewValue;
	const float ShieldMax = AttributeSetBase ? AttributeSetBase->GetShieldMax() : 1.f;

	// Update floating status bar
	if (AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(GetPawn()))
	{
		if (UGHOFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar())
		{
			HeroFloatingStatusBar->SetShieldPercentage(Shield / ShieldMax);
		}
	}

	/*
	by GASDocumentation
		Update the HUD
		Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
	和訳
		HUD の更新
		AsyncTaskAttributeChanged ノードを使用して UI 自体で処理する。
	*/
}

void AGHOPlayerState::ShieldMaxChanged(const FOnAttributeChangeData & Data)
{
	const float Shield = AttributeSetBase ? AttributeSetBase->GetShield() : 0.f;
	const float ShieldMax = Data.NewValue;

	// Update floating status bar
	if (AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(GetPawn()))
	{
		if (UGHOFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar())
		{
			HeroFloatingStatusBar->SetShieldPercentage(Shield / ShieldMax);
		}
	}

	// Update the HUD
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->SetShieldMax(ShieldMax);
		}
	}
}

void AGHOPlayerState::ShieldRegenRateChanged(const FOnAttributeChangeData & Data)
{
	float ShieldRegenRate = Data.NewValue;

	// Update the HUD
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->SetShieldRegenRate(ShieldRegenRate);
		}
	}
}

void AGHOPlayerState::CharacterLevelChanged(const FOnAttributeChangeData & Data)
{
	float CharacterLevel = Data.NewValue;

	// Update the HUD
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->SetHeroLevel(CharacterLevel);
		}
	}
}

void AGHOPlayerState::XPChanged(const FOnAttributeChangeData & Data)
{
	float XP = Data.NewValue;

	// Update the HUD
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->SetXP(XP);
		}
	}
}

void AGHOPlayerState::GoldChanged(const FOnAttributeChangeData & Data)
{
	float Gold = Data.NewValue;

	// Update the HUD
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		if (UGHOHUDWidget* HUD = PC->GetHUD())
		{
			HUD->SetGold(Gold);
		}
	}
}

void AGHOPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		AbilitySystemComponent->CancelAbilitiesByStun();
	}
}

void AGHOPlayerState::KnockDownTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(GetPawn()))
	{
		if (IsValid(Hero))
		{
			if (NewCount > 0)
			{
				Hero->PlayKnockDownEffects();
			}
			else if (NewCount < 1 && !AbilitySystemComponent->IsDead())
			{
				Hero->PlayReviveEffects();
			}
		}
	}
}

