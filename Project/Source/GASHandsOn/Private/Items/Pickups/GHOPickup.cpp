// (C) Sentya Anko 2021


#include "Items/Pickups/GHOPickup.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
//#include "GameplayAbilitySpec.h"
#include "Sound/SoundCue.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "Characters/AttributeSets/GHOAttributeSetBase.h"
#include "Characters/GHOCharacterBase.h"

#define COLLISION_ABILITY						ECollisionChannel::ECC_GameTraceChannel1
#define COLLISION_PROJECTILE					ECollisionChannel::ECC_GameTraceChannel2
#define COLLISION_ABILITYOVERLAPPROJECTILE		ECollisionChannel::ECC_GameTraceChannel3
#define COLLISION_PICKUP						ECollisionChannel::ECC_GameTraceChannel4
#define COLLISION_INTERACTABLE					ECollisionChannel::ECC_GameTraceChannel5

/*
和訳
*/

// Sets default values
AGHOPickup::AGHOPickup()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bIsActive = true;
	bCanRespawn = true;
	RespawnTime = 5.0f;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(FName("CollisionComp"));
	CollisionComp->InitCapsuleSize(40.0f, 50.0f);
	CollisionComp->SetCollisionObjectType(COLLISION_PICKUP);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComp;

	RestrictedPickupTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
	RestrictedPickupTags.AddTag(FGameplayTag::RequestGameplayTag("State.KnockedDown"));
}

void AGHOPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGHOPickup, bIsActive);
	DOREPLIFETIME(AGHOPickup, PickedUpBy);
}

void AGHOPickup::NotifyActorBeginOverlap(AActor* Other)
{
	if (GetLocalRole() == ROLE_Authority && Other && Other != this)
	{
		PickupOnTouch(Cast<AGHOCharacterBase>(Other));
	}
}

bool AGHOPickup::CanBePickedUp(AGHOCharacterBase* TestCharacter) const
{
	return bIsActive && TestCharacter && TestCharacter->IsAlive() && !IsPendingKill() && !TestCharacter->GetAbilitySystemComponent()->HasAnyMatchingGameplayTags(RestrictedPickupTags) && K2_CanBePickedUp(TestCharacter);
}

bool AGHOPickup::K2_CanBePickedUp_Implementation(AGHOCharacterBase* TestCharacter) const
{
	return true;
}

void AGHOPickup::PickupOnTouch(AGHOCharacterBase* Pawn)
{
	if (CanBePickedUp(Pawn))
	{
		GivePickupTo(Pawn);
		PickedUpBy = Pawn;
		bIsActive = false;
		OnPickedUp();

		if (bCanRespawn && RespawnTime > 0.0f)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_RespawnPickup, this, &AGHOPickup::RespawnPickup, RespawnTime, false);
		}
		else
		{
			Destroy();
		}
	}
}

void AGHOPickup::GivePickupTo(AGHOCharacterBase* Pawn)
{
	UAbilitySystemComponent* ASC = Pawn->GetAbilitySystemComponent();
	//const float Level = Pawn->GetCharacterLevel();
	const float Level = Pawn->GetAttributeSet()->GetCharacterLevel();

	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Pawn's ASC is null."), *FString(__FUNCTION__));
		return;
	}

	for (TSubclassOf<UGHOGameplayAbility> AbilityClass : AbilityClasses)
	{
		if (!AbilityClass)
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1, static_cast<int32>(AbilityClass.GetDefaultObject()->AbilityInputID), this);
		ASC->GiveAbilityAndActivateOnce(AbilitySpec);
	}

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> EffectClass : EffectClasses)
	{
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(EffectClass, Level, EffectContext);

		if (NewHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
		}
	}
}

void AGHOPickup::OnPickedUp()
{
	K2_OnPickedUp();

	if (PickupSound && PickedUpBy)
	{
		UGameplayStatics::SpawnSoundAttached(PickupSound, PickedUpBy->GetRootComponent());
	}
}

void AGHOPickup::RespawnPickup()
{
	bIsActive = true;
	PickedUpBy = NULL;
	OnRespawned();

	TSet<AActor*> OverlappingPawns;
	GetOverlappingActors(OverlappingPawns, AGHOCharacterBase::StaticClass());

	for (AActor* OverlappingPawn : OverlappingPawns)
	{
		PickupOnTouch(CastChecked<AGHOCharacterBase>(OverlappingPawn));
	}
}

void AGHOPickup::OnRespawned()
{
	K2_OnRespawned();
}

void AGHOPickup::OnRep_IsActive()
{
	if (bIsActive)
	{
		OnRespawned();
	}
	else
	{
		OnPickedUp();
	}
}
