// (C) Sentya Anko 2021


#include "Characters/GHOASCActorBase.h"
#include "Characters/Components/GHOAbilitySystemComponent.h"


AGHOASCActorBase::AGHOASCActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	/*
	by GASShooter
		Create ability system component, and set it to be explicitly replicated
	和訳
		AbilitySystemComponent を作成し、レプリケーションするように明示的に設定する。
	*/
	AbilitySystemComponent = CreateDefaultSubobject<UGHOAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	/*
	by GASShooter
		Minimal mode means GameplayEffects are not replicated to anyone. 
		Only GameplayTags and Attributes are replicated to clients.
	和訳
		Minimal モードとは GameplayEffects が誰にも複製されないことを意味する。
		GameplayTags と Attribute だけがクライアントに複製される。
	*/
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

UAbilitySystemComponent* AGHOASCActorBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGHOASCActorBase::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
