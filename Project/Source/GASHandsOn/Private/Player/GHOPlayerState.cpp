// (C) Sentya Anko 2021


#include "Player/GHOPlayerState.h"
#include "Characters/Abilities/GHOAbilitySystemComponent.h"
#include "Characters/Abilities/GHOAttributeSetBase.h"
#include "Characters/GHOHeroCharacterBase.h"
#include "Player/GHOPlayerController.h"


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
		混合モードとは GameplayEffect をシミュレートさせた時、プロ棋士にレプリケーションするのではなく、 GameplayEffect を自分自身にレプリケーションさせることを意味します。
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

	/*
	by GASDocumentation
		PlayerState の NetUpdateFrequency を Character と同じに設定します。
		PlayerStates のデフォルト値は非常に低く、 AbilitySystem に知覚可能なラグをもたらします。
		100 はおそらく出荷ゲームには高すぎるので、ニーズに合わせて調整することが出来ます。
	和訳
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
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &AGHOPlayerState::MaxHealthChanged);
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

void AGHOPlayerState::HealthChanged(const struct FOnAttributeChangeData& Data)
{
	const float Health = Data.NewValue;

	if (AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(GetPawn()))
	{
		// TODO:Update floating status bar

		if ((Health <= 0.f) && !AbilitySystemComponent->IsDead())
		{
			Hero->Die();
		}
	}
}

void AGHOPlayerState::MaxHealthChanged(const struct FOnAttributeChangeData& Data)
{
	float MaxHealth = Data.NewValue;

	if (AGHOHeroCharacterBase* Hero = Cast<AGHOHeroCharacterBase>(GetPawn()))
	{
		// TODO: Update floating status bar
	}
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetOwner()))
	{
		// TODO:Update the HUD
	}
}
