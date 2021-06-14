// (C) Sentya Anko 2021


#include "Items/Pickups/GHOPickup.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "Settings/GHOCollisions.h"
#include "Settings/GHOGameplayTags.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "Characters/AttributeSets/GHOAttributeSetBase.h"
#include "Characters/GHOCharacterBase.h"


/*
解説
	概要
		拾えるものを扱うクラス。
		一度拾ったら終わりのものと、拾った後一定時間でまた拾えるようになるものがあり、両方扱う。
	動作モード（リスポーンするか、一回取ったら終わりか）
		bCanRespawn
			true の場合
				キャラクターが取得後も、時間経過後に再取得が可能になる。
				仕組み的にはリスポーンしているわけではなく、表示しているオブジェクトを一旦消し、
				RespawnTime 経過後にもとにもどしているだけである。
				消えている間も、アクター自体は生きていて、コリジョンなどもそのままである。
			false の場合
				キャラクターが取得後、アクターが破棄される。
	サーバーとクライアント
		レプリケーション対象
			GetLifetimeReplicatedProps() の実装を見れば分かる通り、以下の2つのメンバーのみ。
				bIsActive
				PickedUpBy
		同期の方法
			Overlap イベントの NotifyActorBeginOverlap() 
				サーバー・クライアント両方で呼び出されるが、関数内の処理はまず Authority (Server) かの確認を行う。
				サーバー側はここで各種判定処理と変数（ bIsActive と PickedUpBy も含む）の更新を行う。
				クライアント側は最初の判定で弾かれ、後続の処理は行わない。
			bIsActive のレプリケーション通知の OnRep_IsActive()
				クライアント側で呼び出される。
				状態が切り替わった（取得され消滅した、リスポーンした）際の見た目や音の処理をここから呼び出す想定となっている。
				具体的な処理は Blueprint 側でも実装可能になっている。
				この実装では以下のような切り分けで処理している（が、そうしなければならないということでは無い）。
					C++ 側
						bIsActive が false になった（取得され消滅した）際に PickedUpBy の RootComponent に対してサウンドをアタッチしている。
					Blueprint 側
						見た目に関する処理を行っている。（表示しているパーティクルの On/Off 等）
		その他のメンバの利用され方は以下の通り
			変数
				両方
					CollisionComp
				レプリケーション
					bIsActive
					PickedUpBy
				Authority のみ使用
					bCanRespawn
					RespawnTime
					RestrictedPickupTags
					AbilityClasses
					EffectClasses
					TimerHandle_RespawnPickup
				Authority 以外（このケースでは要はクライアント側）が使用
					PickupSound
			関数
				両方
					AGHOPickup()
					NotifyActorBeginOverlap()
					OnPickedUp()
					OnPickedUp_Implementation()
					OnRespawned()
					OnRespawned_Implementation()
				Authority のみ使用
					GetLifetimeReplicatedProps()
					PickupOnTouch()
					CanBePickedUp()
					CanBePickedUp_Implementation()
					GivePickupTo()
					RespawnPickup()
				Authority 以外（このケースでは要はクライアント側）が使用
					OnRep_IsActive()
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

	RestrictedPickupTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_State_Dead));
	RestrictedPickupTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_State_KnockedDown));
	//UE_LOG(LogTemp, Error, TEXT("%s Role is %s"), *FString(__FUNCTION__), *UEnum::GetDisplayValueAsText(GetLocalRole()).ToString());
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

bool AGHOPickup::CanBePickedUp_Implementation(AGHOCharacterBase* TestCharacter) const
{
	return bIsActive && TestCharacter && TestCharacter->IsAlive() && !IsPendingKill() && !TestCharacter->GetAbilitySystemComponent()->HasAnyMatchingGameplayTags(RestrictedPickupTags);
}

void AGHOPickup::OnPickedUp_Implementation()
{
	if (PickupSound && PickedUpBy)
	{
		UGameplayStatics::SpawnSoundAttached(PickupSound, PickedUpBy->GetRootComponent());
	}
}

void AGHOPickup::OnRespawned_Implementation()
{
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

	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Pawn's ASC is null."), *FString(__FUNCTION__));
		return;
	}
	const float Level = Pawn->GetCharacterLevel();

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
		if (!bIsActive)
		{
			break;
		}
	}
}

