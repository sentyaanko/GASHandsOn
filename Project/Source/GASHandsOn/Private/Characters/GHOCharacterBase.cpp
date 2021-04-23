// (C) Sentya Anko 2021


#include "Characters/GHOCharacterBase.h"

// Sets default values
AGHOCharacterBase::AGHOCharacterBase()
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
}

