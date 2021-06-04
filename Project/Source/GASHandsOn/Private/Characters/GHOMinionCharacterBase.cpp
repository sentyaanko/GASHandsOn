// (C) Sentya Anko 2021


#include "Characters/GHOMinionCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Components/GHOAbilitySystemComponent.h"
#include "UI/GHOFloatingStatusBarWidget.h"
#include "Characters/AttributeSets/GHOAttributeSetBase.h"
#include "Settings/GHODefaultClasses.h"
#include "Settings/GHOGameplayTags.h"

AGHOMinionCharacterBase::AGHOMinionCharacterBase(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	/*
	by GASDocumentation
		Create ability system component, and set it to be explicitly replicated
	和訳
		AbilitySystemComponent を作成し、それを明示的に複製するように設定します。
	*/
	AbilitySystemComponent = CreateDefaultSubobject<UGHOAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	/*
	by GASDocumentation
		Minimal Mode means that no GameplayEffects will replicate.
		They will only live on the Server.
		Attributes, GameplayTags, and GameplayCues will still replicate to us.
	和訳
		Minimal モードとは、 GameplayEffect が複製されないことを意味します。
		GameplayEffect はサーバー上にしか存在しません。
		Attributes と GameplayTags と GameplayCues はレプリケーションされます。
	*/
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	/*
	by GASDocumentation
		Create the attribute set, this replicates by default
		Adding it as a subobject of the owning actor of an AbilitySystemComponent
		automatically registers the AttributeSet with the AbilitySystemComponent
	和訳
		AttributeSet の作成、これはデフォルトで複製されます。
		AbilitySystemComponent のオーナーであるアクターのサブオブジェクトとして追加します。
		AbilitySet を AbilitySystemComponent に自動的に登録します。
	*/
	AttributeSetBase = CreateDefaultSubobject<UGHOAttributeSetBase>(TEXT("AttributeSetBase"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
	UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));

	UIFloatingStatusBarClass = FGHODefaultClasses::GetFloatingStatusBarForMinionClass();
}

void AGHOMinionCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
		AddStartupEffects();
		AddCharacterAbilities();

		/*
		by GASDocumentation
			Setup FloatingStatusBar UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
		和訳
			Locally Owned Player のみ FloatingStatusBar UI を設定し、 AI や サーバーのコピーの PlayerController には設定しない。
		*/
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC && PC->IsLocalPlayerController())
		{
			if (UIFloatingStatusBarClass)
			{
				UIFloatingStatusBar = CreateWidget<UGHOFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
				if (UIFloatingStatusBar && UIFloatingStatusBarComponent)
				{
					UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

					/*
					by GASDocumentation
						Setup the floating status bar
					和訳
						floating status bar の設定
					*/
					UIFloatingStatusBar->SetHealthPercentage(AttributeSetBase->GetHealth() / AttributeSetBase->GetHealthMax());

					UIFloatingStatusBar->SetCharacterName(CharacterName);
				}
			}
		}

		/*
		by GASDocumentation
			Attribute change callbacks
		和訳
			Attribute の変更コールバック
		*/
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AGHOMinionCharacterBase::HealthChanged);

		/*
		by GASDocumentation
			Tag change callbacks
		和訳
			タグの変更コールバック
		*/
		StunChangedDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName(TAG_State_CrowdControl_Stun)), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AGHOMinionCharacterBase::StunTagChanged);
	}
}

UAbilitySystemComponent* AGHOMinionCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UGHOAttributeSetBase* AGHOMinionCharacterBase::GetAttributeSet()
{
	return AttributeSetBase;
}

const UGHOAttributeSetBase* AGHOMinionCharacterBase::GetAttributeSet()const
{
	return AttributeSetBase;
}

void AGHOMinionCharacterBase::HealthChanged(const FOnAttributeChangeData & Data)
{
	const float Health = Data.NewValue;

	/*
	by GASDocumentation
		Update floating status bar
	和訳
		floating status bar の更新
	*/
	if (UIFloatingStatusBar)
	{
		UIFloatingStatusBar->SetHealthPercentage(Health / AttributeSetBase->GetHealthMax());
	}

	/*
	by GASDocumentation
		If the minion died, handle death
	和訳
		ミニオンが死亡した場合、死亡処理する。
	*/
	if (!IsAlive() && !AbilitySystemComponent->IsDead())
	{
		Die();
	}
}

void AGHOMinionCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		AbilitySystemComponent->CancelAbilitiesByStun();
	}
}
