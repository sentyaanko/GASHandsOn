// (C) Sentya Anko 2021


#include "Characters/GHOCharacterBase.h"
//#include "AbilitySystemComponent.h"
#include "Characters/Abilities/GHOAbilitySystemComponent.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "Character/GHOCharacterMovementComponent.h"


// Sets default values
AGHOCharacterBase::AGHOCharacterBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UGHOCharacterMovementComponent>(TEXT("CharMoveComp")))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

void AGHOCharacterBase::Die()
{
	// Only runs on Server
	RemoveCharacterAbilities();

	//todo:We need to complete this function when we implement health.
}

void AGHOCharacterBase::InitializeAttributes()
{

}

void AGHOCharacterBase::AddStartupEffects()
{
	//for server
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

int32 AGHOCharacterBase::GetAbilityLevel(EGHOAbilityInputID AbilityID) const
{
	return 1;
}
