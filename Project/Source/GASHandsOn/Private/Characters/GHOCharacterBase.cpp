// (C) Sentya Anko 2021


#include "Characters/GHOCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "AbilitySystemComponent.h"
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

//// Called when the game starts or when spawned
//void AGHOCharacterBase::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}

//// Called every frame
//void AGHOCharacterBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

//// Called to bind functionality to input
//void AGHOCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

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
	// Only runs on Server
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnCharacterDied.Broadcast(this);

	if (UGHOAbilitySystemComponent* AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AbilitySystemComponent->Die();
	}

	//TODO: We need to complete this function when we implement DeathMontage.
	//if (DeathMontage)
	//{
	//	PlayAnimMontage(DeathMontage);
	//}
	//else
	//{
		FinishDying();
	//}
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
	}
	else
	{
		return false;
	}
	return true;
}

void AGHOCharacterBase::InitializeAttributes()
{
#if 0
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
#else
	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}
	if (UGHOAbilitySystemComponent* AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AbilitySystemComponent->InitializeAttributes(this);
	}

#endif
}

void AGHOCharacterBase::AddStartupEffects()
{
	//for server
#if 0
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->StartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->StartupEffectsApplied = true;
#else
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (UGHOAbilitySystemComponent* AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AbilitySystemComponent->AddStartupEffects(this);
	}

#endif
}

void AGHOCharacterBase::AddCharacterAbilities()
{
	//for server
#if 0
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();

	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UGHOGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	bCharacterAbilitiesGiven = true;
#else
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (UGHOAbilitySystemComponent* AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AbilitySystemComponent->AddCharacterAbilities(this);
	}

#endif
}

void AGHOCharacterBase::RemoveCharacterAbilities()
{
	//for server
#if 0
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || !bCharacterAbilitiesGiven)
	{
		return;
	}

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	bCharacterAbilitiesGiven = false;
#else
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (UGHOAbilitySystemComponent* AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AbilitySystemComponent->RemoveCharacterAbilities(this);
	}
#endif
}

