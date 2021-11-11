// (C) Sentya Anko 2021


#include "Weapons/GHOWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

#include "Characters/Components/GHOAbilitySystemComponent.h"
//#include "Characters/Abilities/GHOAbilitySystemGlobals.h"
#include "Characters/Abilities/GHOGameplayAbility.h"
#include "Characters/TargetActors/GHOGATA_LineTrace.h"
#include "Characters/TargetActors/GHOGATA_SphereTrace.h"
#include "Characters/GHOHeroCharacterBase.h"
#include "FunctionLibrary/GHOBlueprintFunctionLibrary.h"
#include "Player/GHOPlayerController.h"
#include "Settings/GHOCollisions.h"


// Sets default values
AGHOWeapon::AGHOWeapon()
{
	// Set this actor to never tick
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	/*
	by GASShooter
		Set this to a value that's appropriate for your game
	和訳
		ゲームに適した値を設定してください。
	*/
	NetUpdateFrequency = 100.0f;
	bSpawnWithCollision = true;
	PrimaryClipAmmo = 0;
	MaxPrimaryClipAmmo = 0;
	SecondaryClipAmmo = 0;
	MaxSecondaryClipAmmo = 0;
	bInfiniteAmmo = false;
	PrimaryAmmoType = FGameplayTag::RequestGameplayTag(FName("Weapon.Ammo.None"));
	SecondaryAmmoType = FGameplayTag::RequestGameplayTag(FName("Weapon.Ammo.None"));

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(FName("CollisionComponent"));
	CollisionComp->InitCapsuleSize(40.0f, 50.0f);
	CollisionComp->SetCollisionObjectType(COLLISION_PICKUP);
	/*
	by GASShooter
		Manually enable when in pickup mode
	和訳
		ピックアップモード時に手動で設定。
	*/
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComp;

#if 0
	//TODO: 1st / 3rd person view
	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("WeaponMesh1P"));
	WeaponMesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh1P->CastShadow = false;
	WeaponMesh1P->SetVisibility(false, true);
	WeaponMesh1P->SetupAttachment(CollisionComp);
	WeaponMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	WeaponMesh3PickupRelativeLocation = FVector(0.0f, -25.0f, 0.0f);

	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(FName("WeaponMesh3P"));
	WeaponMesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh3P->SetupAttachment(CollisionComp);
	WeaponMesh3P->SetRelativeLocation(WeaponMesh3PickupRelativeLocation);
	WeaponMesh3P->CastShadow = true;
	WeaponMesh3P->SetVisibility(true, true);
	WeaponMesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
#else
	WeaponMeshickupRelativeLocation = FVector(0.0f, -25.0f, 0.0f);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("WeaponMesh3P"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(CollisionComp);
	WeaponMesh->SetRelativeLocation(WeaponMeshickupRelativeLocation);
	WeaponMesh->CastShadow = true;
	WeaponMesh->SetVisibility(true, true);
	WeaponMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
#endif

	WeaponPrimaryInstantAbilityTag = FGameplayTag::RequestGameplayTag("Ability.Weapon.Primary.Instant");
	WeaponSecondaryInstantAbilityTag = FGameplayTag::RequestGameplayTag("Ability.Weapon.Secondary.Instant");
	WeaponAlternateInstantAbilityTag = FGameplayTag::RequestGameplayTag("Ability.Weapon.Alternate.Instant");
	WeaponIsFiringTag = FGameplayTag::RequestGameplayTag("Weapon.IsFiring");

	FireMode = FGameplayTag::RequestGameplayTag("Weapon.FireMode.None");
	StatusText = DefaultStatusText;

	RestrictedPickupTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
	RestrictedPickupTags.AddTag(FGameplayTag::RequestGameplayTag("State.KnockedDown"));
}

/*
by GASShooter
	Called when the game starts or when spawned
和訳
	ゲーム開始時又はスポーン時に呼び出される。
*/
void AGHOWeapon::BeginPlay()
{
	ResetWeapon();

	if (!OwningCharacter && bSpawnWithCollision)
	{
		/*
		by GASShooter
			Spawned into the world without an owner, enable collision as we are in pickup mode
		和訳
			所有者がいない状態でワールドにスポーンしました、コリジョンを有効にしてピックアップモードにします。
		*/
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	Super::BeginPlay();
}

UAbilitySystemComponent* AGHOWeapon::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

#if 0
//TODO: 1st / 3rd person view
USkeletalMeshComponent* AGHOWeapon::GetWeaponMesh1P() const
{
	return WeaponMesh1P;
}

USkeletalMeshComponent* AGHOWeapon::GetWeaponMesh3P() const
{
	return WeaponMesh3P;
}
#else
USkeletalMeshComponent* AGHOWeapon::GetWeaponMesh() const
{
	return WeaponMesh;
}
#endif

void AGHOWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGHOWeapon, OwningCharacter, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGHOWeapon, PrimaryClipAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGHOWeapon, MaxPrimaryClipAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGHOWeapon, SecondaryClipAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AGHOWeapon, MaxSecondaryClipAmmo, COND_OwnerOnly);
}

void AGHOWeapon::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	DOREPLIFETIME_ACTIVE_OVERRIDE(AGHOWeapon, PrimaryClipAmmo, (IsValid(AbilitySystemComponent) && !AbilitySystemComponent->HasMatchingGameplayTag(WeaponIsFiringTag)));
	DOREPLIFETIME_ACTIVE_OVERRIDE(AGHOWeapon, SecondaryClipAmmo, (IsValid(AbilitySystemComponent) && !AbilitySystemComponent->HasMatchingGameplayTag(WeaponIsFiringTag)));
}

void AGHOWeapon::SetOwningCharacter(AGHOHeroCharacterBase* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
	if (OwningCharacter)
	{
		/*
		by GASShooter
			Called when added to inventory
		和訳
			インベントリに追加された時に呼び出される。
		*/
		AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());
		SetOwner(InOwningCharacter);
		AttachToComponent(OwningCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (OwningCharacter->GetCurrentWeapon() != this)
		{
#if 0
			//TODO: 1st / 3rd person view
			WeaponMesh3P->CastShadow = false;
			WeaponMesh3P->SetVisibility(true, true);
			WeaponMesh3P->SetVisibility(false, true);
#else
			WeaponMesh->CastShadow = false;
			WeaponMesh->SetVisibility(true, true);
			WeaponMesh->SetVisibility(false, true);
#endif
		}
	}
	else
	{
		AbilitySystemComponent = nullptr;
		SetOwner(nullptr);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void AGHOWeapon::SetSpawnWithCollision(bool bInSpawnWithCollision)
{
	bSpawnWithCollision = bInSpawnWithCollision;
}

void AGHOWeapon::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	if (!IsPendingKill() && !OwningCharacter)
	{
		PickUpOnTouch(Cast<AGHOHeroCharacterBase>(Other));
	}
}

void AGHOWeapon::Equip()
{
	if (!OwningCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s OwningCharacter is nullptr"), *FString(__FUNCTION__), *GetName());
		return;
	}

	FName AttachPoint = OwningCharacter->GetWeaponAttachPoint();

#if 0
	//TODO: 1st / 3rd person view
	if (WeaponMesh1P)
	{
		WeaponMesh1P->AttachToComponent(OwningCharacter->GetFirstPersonMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
		WeaponMesh1P->SetRelativeLocation(WeaponMesh1PEquippedRelativeLocation);
		WeaponMesh1P->SetRelativeRotation(FRotator(0, 0, -90.0f));

		if (OwningCharacter->IsInFirstPersonPerspective())
		{
			WeaponMesh1P->SetVisibility(true, true);
		}
		else
		{
			WeaponMesh1P->SetVisibility(false, true);
		}
	}

	if (WeaponMesh3P)
	{
		WeaponMesh3P->AttachToComponent(OwningCharacter->GetThirdPersonMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
		WeaponMesh3P->SetRelativeLocation(WeaponMesh3PEquippedRelativeLocation);
		WeaponMesh3P->SetRelativeRotation(FRotator(0, 0, -90.0f));
		WeaponMesh3P->CastShadow = true;
		WeaponMesh3P->bCastHiddenShadow = true;

		if (OwningCharacter->IsInFirstPersonPerspective())
		{
			/*
			by GASShooter
				Without this, the weapon's 3p shadow doesn't show
			和訳
				これを行わないと、三人称視点の武器の影が表示されない
			*/
			WeaponMesh3P->SetVisibility(true, true);
			WeaponMesh3P->SetVisibility(false, true);
		}
		else
		{
			WeaponMesh3P->SetVisibility(true, true);
		}
	}
#else
	if (WeaponMesh)
	{
		WeaponMesh->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
		WeaponMesh->SetRelativeLocation(WeaponMeshEquippedRelativeLocation);
//		WeaponMesh->SetRelativeRotation(FRotator(0, 0, -90.0f));
		WeaponMesh->CastShadow = true;
		WeaponMesh->bCastHiddenShadow = true;

		//if (OwningCharacter->IsInFirstPersonPerspective())
		//{
		//	WeaponMesh->SetVisibility(true, true); // Without this, the weapon's 3p shadow doesn't show
		//	WeaponMesh->SetVisibility(false, true);
		//}
		//else
		//{
			WeaponMesh->SetVisibility(true, true);
		//}
	}
#endif
}

void AGHOWeapon::UnEquip()
{
	if (OwningCharacter == nullptr)
	{
		return;
	}

	/*
	by GASShooter
		Necessary to detach so that when toggling perspective all meshes attached won't become visible.
	和訳
		パースペクティブを切り替えた時に、接続されている全てのメッシュが表示されないように、デタッチする必要があります。
	*/

#if 0
	//TODO: 1st / 3rd person view
	WeaponMesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh1P->SetVisibility(false, true);

	WeaponMesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh3P->CastShadow = false;
	WeaponMesh3P->bCastHiddenShadow = false;
	/*
	by GASShooter
		Without this, the unequipped weapon's 3p shadow hangs around
	和訳
		これがないと、装備されていない武器の 3P シャドウがかかってしまう。
	*/
	WeaponMesh3P->SetVisibility(true, true);
	WeaponMesh3P->SetVisibility(false, true);
#else
	WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh->CastShadow = false;
	WeaponMesh->bCastHiddenShadow = false;
	/*
	by GASShooter
		Without this, the unequipped weapon's shadow hangs around
	和訳
		これがないと、装備されていない武器のシャドウがかかってしまう。
	*/
	WeaponMesh->SetVisibility(true, true);
	WeaponMesh->SetVisibility(false, true);
#endif
}

void AGHOWeapon::AddAbilities()
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetAbilitySystemComponent())
	{
		return;
	}

	UGHOAbilitySystemComponent* ASC = Cast<UGHOAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s Role: %s ASC is null"), *FString(__FUNCTION__), *GetName(), GET_ACTOR_ROLE_FSTRING(OwningCharacter));
		return;
	}

	/*
	by GASShooter
		Grant abilities, but only on the server	
	和訳
		アビリティを付与する、ただしサーバーのみ。
	*/
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (TSubclassOf<UGHOGameplayAbility>& Ability : Abilities)
	{
		AbilitySpecHandles.Add(ASC->GiveAbility(
			FGameplayAbilitySpec(Ability, GetAbilityLevel(Ability.GetDefaultObject()->AbilityID), static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), this)));
	}
}

void AGHOWeapon::RemoveAbilities()
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetAbilitySystemComponent())
	{
		return;
	}

	UGHOAbilitySystemComponent* ASC = Cast<UGHOAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	if (!ASC)
	{
		return;
	}

	/*
	by GASShooter
		Remove abilities, but only on the server
	和訳
		アビリティを削除する、ただしサーバーのみ。
	*/
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (FGameplayAbilitySpecHandle& SpecHandle : AbilitySpecHandles)
	{
		ASC->ClearAbility(SpecHandle);
	}
}

int32 AGHOWeapon::GetAbilityLevel(EGHOAbilityInputID AbilityID)
{
	/*
	by GASShooter
		All abilities for now are level 1
	和訳
		全てのアビリティは今のところ、レベル 1 です。
	*/
	return 1;
}

void AGHOWeapon::ResetWeapon()
{
	FireMode = DefaultFireMode;
	StatusText = DefaultStatusText;
}

void AGHOWeapon::RPCNetMulticastOnDropped_Implementation(FVector NewLocation)
{
	SetOwningCharacter(nullptr);
	ResetWeapon();

	SetActorLocation(NewLocation);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

#if 0
	//TODO: 1st / 3rd person view
	if (WeaponMesh1P)
	{
		WeaponMesh1P->AttachToComponent(CollisionComp, FAttachmentTransformRules::SnapToTargetIncludingScale);
		WeaponMesh1P->SetVisibility(false, true);
	}

	if (WeaponMesh3P)
	{
		WeaponMesh3P->AttachToComponent(CollisionComp, FAttachmentTransformRules::SnapToTargetIncludingScale);
		WeaponMesh3P->SetRelativeLocation(WeaponMesh3PickupRelativeLocation);
		WeaponMesh3P->CastShadow = true;
		WeaponMesh3P->SetVisibility(true, true);
	}
#else
	if (WeaponMesh)
	{
		WeaponMesh->AttachToComponent(CollisionComp, FAttachmentTransformRules::SnapToTargetIncludingScale);
		WeaponMesh->SetRelativeLocation(WeaponMeshickupRelativeLocation);
		WeaponMesh->CastShadow = true;
		WeaponMesh->SetVisibility(true, true);
	}
#endif
}

bool AGHOWeapon::RPCNetMulticastOnDropped_Validate(FVector NewLocation)
{
	return true;
}

int32 AGHOWeapon::GetPrimaryClipAmmo() const
{
	return PrimaryClipAmmo;
}

int32 AGHOWeapon::GetMaxPrimaryClipAmmo() const
{
	return MaxPrimaryClipAmmo;
}

int32 AGHOWeapon::GetSecondaryClipAmmo() const
{
	return SecondaryClipAmmo;
}

int32 AGHOWeapon::GetMaxSecondaryClipAmmo() const
{
	return MaxSecondaryClipAmmo;
}

void AGHOWeapon::SetPrimaryClipAmmo(int32 NewPrimaryClipAmmo)
{
	int32 OldPrimaryClipAmmo = PrimaryClipAmmo;
	PrimaryClipAmmo = NewPrimaryClipAmmo;
	OnPrimaryClipAmmoChanged.Broadcast(OldPrimaryClipAmmo, PrimaryClipAmmo);
}

void AGHOWeapon::SetMaxPrimaryClipAmmo(int32 NewMaxPrimaryClipAmmo)
{
	int32 OldMaxPrimaryClipAmmo = MaxPrimaryClipAmmo;
	MaxPrimaryClipAmmo = NewMaxPrimaryClipAmmo;
	OnMaxPrimaryClipAmmoChanged.Broadcast(OldMaxPrimaryClipAmmo, MaxPrimaryClipAmmo);
}

void AGHOWeapon::SetSecondaryClipAmmo(int32 NewSecondaryClipAmmo)
{
	int32 OldSecondaryClipAmmo = SecondaryClipAmmo;
	SecondaryClipAmmo = NewSecondaryClipAmmo;
	OnSecondaryClipAmmoChanged.Broadcast(OldSecondaryClipAmmo, SecondaryClipAmmo);
}

void AGHOWeapon::SetMaxSecondaryClipAmmo(int32 NewMaxSecondaryClipAmmo)
{
	int32 OldMaxSecondaryClipAmmo = MaxSecondaryClipAmmo;
	MaxSecondaryClipAmmo = NewMaxSecondaryClipAmmo;
	OnMaxSecondaryClipAmmoChanged.Broadcast(OldMaxSecondaryClipAmmo, MaxSecondaryClipAmmo);
}

TSubclassOf<UGHOHUDReticle> AGHOWeapon::GetPrimaryHUDReticleClass() const
{
	return PrimaryHUDReticleClass;
}

bool AGHOWeapon::HasInfiniteAmmo() const
{
	return bInfiniteAmmo;
}

#if 0
//TODO: 1st / 3rd person view
UAnimMontage* AGHOWeapon::GetEquip1PMontage() const
{
	return Equip1PMontage;
}

UAnimMontage* AGHOWeapon::GetEquip3PMontage() const
{
	return Equip3PMontage;
}
#else
UAnimMontage* AGHOWeapon::GetEquipMontage() const
{
	return EquipMontage;
}
#endif

USoundCue* AGHOWeapon::GetPickupSound() const
{
	return PickupSound;
}

FText AGHOWeapon::GetDefaultStatusText() const
{
	return DefaultStatusText;
}

AGHOGATA_LineTrace* AGHOWeapon::GetLineTraceTargetActor()
{
	if (LineTraceTargetActor)
	{
		return LineTraceTargetActor;
	}

	LineTraceTargetActor = GetWorld()->SpawnActor<AGHOGATA_LineTrace>();
	LineTraceTargetActor->SetOwner(this);
	return LineTraceTargetActor;
}

AGHOGATA_SphereTrace* AGHOWeapon::GetSphereTraceTargetActor()
{
	if (SphereTraceTargetActor)
	{
		return SphereTraceTargetActor;
	}

	SphereTraceTargetActor = GetWorld()->SpawnActor<AGHOGATA_SphereTrace>();
	SphereTraceTargetActor->SetOwner(this);
	return SphereTraceTargetActor;
}

void AGHOWeapon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (LineTraceTargetActor)
	{
		LineTraceTargetActor->Destroy();
	}

	if (SphereTraceTargetActor)
	{
		SphereTraceTargetActor->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

void AGHOWeapon::PickUpOnTouch(AGHOHeroCharacterBase* InCharacter)
{
	if (!InCharacter || !InCharacter->IsAlive() || !InCharacter->GetAbilitySystemComponent() || InCharacter->GetAbilitySystemComponent()->HasAnyMatchingGameplayTags(RestrictedPickupTags))
	{
		return;
	}

#if 0
	//TODO: 1st / 3rd person view
	if (InCharacter->AddWeaponToInventory(this, true) && OwningCharacter->IsInFirstPersonPerspective())
	{
		WeaponMesh3P->CastShadow = false;
		WeaponMesh3P->SetVisibility(true, true);
		WeaponMesh3P->SetVisibility(false, true);
	}
#else
	if (InCharacter->AddWeaponToInventory(this, true) /*&& OwningCharacter->IsInFirstPersonPerspective()*/ )
	{
		//WeaponMesh->CastShadow = false;
		//WeaponMesh->SetVisibility(true, true);
		//WeaponMesh->SetVisibility(false, true);
	}
#endif
}

void AGHOWeapon::OnRep_PrimaryClipAmmo(int32 OldPrimaryClipAmmo)
{
	OnPrimaryClipAmmoChanged.Broadcast(OldPrimaryClipAmmo, PrimaryClipAmmo);
}

void AGHOWeapon::OnRep_MaxPrimaryClipAmmo(int32 OldMaxPrimaryClipAmmo)
{
	OnMaxPrimaryClipAmmoChanged.Broadcast(OldMaxPrimaryClipAmmo, MaxPrimaryClipAmmo);
}

void AGHOWeapon::OnRep_SecondaryClipAmmo(int32 OldSecondaryClipAmmo)
{
	OnSecondaryClipAmmoChanged.Broadcast(OldSecondaryClipAmmo, SecondaryClipAmmo);
}

void AGHOWeapon::OnRep_MaxSecondaryClipAmmo(int32 OldMaxSecondaryClipAmmo)
{
	OnMaxSecondaryClipAmmoChanged.Broadcast(OldMaxSecondaryClipAmmo, MaxSecondaryClipAmmo);
}

