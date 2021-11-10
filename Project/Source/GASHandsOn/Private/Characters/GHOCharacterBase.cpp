// (C) Sentya Anko 2021


#include "Characters/GHOCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/Components/GHOAbilitySystemComponent.h"
#include "Characters/Components/GHOCharacterMovementComponent.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "Characters/AttributeSets/GHOAttributeSetBase.h"


AGHOCharacterBase::AGHOCharacterBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UGHOCharacterMovementComponent>(TEXT("CharMoveComp")))
{
	PrimaryActorTick.bCanEverTick = false;

	bAlwaysRelevant = true;
}

UAbilitySystemComponent* AGHOCharacterBase::GetAbilitySystemComponent() const
{
	unimplemented();
	return nullptr;
}

UGHOAttributeSetBase* AGHOCharacterBase::GetAttributeSet()
{
	unimplemented();
	return nullptr;
}
const UGHOAttributeSetBase* AGHOCharacterBase::GetAttributeSet()const
{
	unimplemented();
	return nullptr;
}

void AGHOCharacterBase::Die()
{
	/*
	by GASDocumentation
		Only runs on Server
	和訳
		サーバー上のみで動作
	解説
		この関数は GASShooter では呼び出されていません。
		Hero しか存在しておらず、 Hero ではこの関数を使わないためです。
	*/
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnCharacterDied.Broadcast(this);

	if (UGHOAbilitySystemComponent* AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AbilitySystemComponent->Die();

		//解説
		//	DeathSound の代わりに Local で GameplayCue を再生するための仕組み。
		//	ただし、 Minion でしか使用されないメンバを追加する必要があり、独自に追加実装するにしては設計が微妙です。
		//	仕様次第なので、コメントアウトしておきます。
		//if (DeathSoundTag.IsValid())
		//{
		//	FGameplayCueParameters GCParameters;
		//	GCParameters.Location = GetActorLocation();
		//	AbilitySystemComponent->ExecuteGameplayCueLocal(DeathSoundTag, GCParameters);
		//}
	}

	////TODO replace with a locally executed GameplayCue
	//解説
	//	この Die() 関数は前述のとおり、 Minion でしか呼び出されていません。
	//	Hero 側の実装ではすでに ローカル GameplayCue を利用しています。
	//if (DeathSound)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	//}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}
void AGHOCharacterBase::FinishDying()
{
	Destroy();
}

FTransform AGHOCharacterBase::GetProjectileTransform(float Range)const
{
	return GetTransform();
}

int32 AGHOCharacterBase::GetAbilityLevel(EGHOAbilityInputID AbilityID) const
{
	return 1;
}

bool AGHOCharacterBase::IsAlive() const
{
	if (const auto attr = GetAttributeSet())
	{
		return attr->IsAlive();
	}
	return false;
}

int32 AGHOCharacterBase::GetCharacterLevel() const
{
	//TODO
	//return 1;
	if (const auto attr = GetAttributeSet())
	{
		return attr->GetCharacterLevel();
	}
	return 1;
}

float AGHOCharacterBase::GetHealth() const
{
	if (const auto attr = GetAttributeSet())
	{
		return attr->GetHealth();
	}
	return 0.0f;
}

float AGHOCharacterBase::GetMaxHealth() const
{
	if (const auto attr = GetAttributeSet())
	{
		return attr->GetHealthMax();
	}
	return 1.0f;
}

float AGHOCharacterBase::GetMana() const
{
	if (const auto attr = GetAttributeSet())
	{
		return attr->GetMana();
	}
	return 0.0f;
}

float AGHOCharacterBase::GetMaxMana() const
{
	if (const auto attr = GetAttributeSet())
	{
		return attr->GetManaMax();
	}
	return 1.0f;
}

float AGHOCharacterBase::GetStamina() const
{
	if (const auto attr = GetAttributeSet())
	{
		return attr->GetStamina();
	}
	return 0.0f;
}

float AGHOCharacterBase::GetMaxStamina() const
{
	if (const auto attr = GetAttributeSet())
	{
		return attr->GetStaminaMax();
	}
	return 1.0f;
}

float AGHOCharacterBase::GetShield() const
{
	if (const auto attr = GetAttributeSet())
	{
		return attr->GetShield();
	}
	return 0.0f;
}

float AGHOCharacterBase::GetMaxShield() const
{
	if (const auto attr = GetAttributeSet())
	{
		return attr->GetShieldMax();
	}
	return 1.0f;
}

bool AGHOCharacterBase::IsMovable()const
{
	if (const auto attr = GetAttributeSet())
	{
		if (!attr->IsAlive())
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	if (const auto asc = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		if (asc->IsStun())
		{
			return false;
		}
		//by GASShooter
		//	Don't move while interacting or being interacted on (revived)
		//和訳
		//	インタラクト中またはインタラクション（蘇生）をされている間は動かない
		if (asc->IsInteractingBeforeRemoval())
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool AGHOCharacterBase::IsKnockedDown()const
{
	if (const auto asc = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		return asc->IsKnockedDown();
	}
	return false;
}

void AGHOCharacterBase::InitializeAttributes()
{
	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}
	if (UGHOAbilitySystemComponent* AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AbilitySystemComponent->InitializeAttributes(this);
	}
}

void AGHOCharacterBase::AddStartupEffects()
{
	/*
	by GASDocumentation
		for server
	和訳
		サーバー用
	*/
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (UGHOAbilitySystemComponent* AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AbilitySystemComponent->AddStartupEffects(this);
	}
}

void AGHOCharacterBase::AddCharacterAbilities()
{
	/*
	by GASDocumentation
		for server
		Grant abilities, but only on the server
	和訳
		サーバー用
		アビリティの付与、ただしサーバー上のみ
	*/
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (UGHOAbilitySystemComponent* AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AbilitySystemComponent->AddCharacterAbilities(this);
	}
}

void AGHOCharacterBase::RemoveCharacterAbilities()
{
	/*
	by GASDocumentation
		for server
	和訳
		サーバー用
	*/
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (UGHOAbilitySystemComponent* AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AbilitySystemComponent->RemoveCharacterAbilities(this);
	}
}

