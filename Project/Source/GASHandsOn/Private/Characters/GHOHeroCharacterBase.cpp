// (C) Sentya Anko 2021


#include "Characters/GHOHeroCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Characters/Components/GHOAbilitySystemComponent.h"
#include "Characters/AttributeSets/GHOAttributeSetBase.h"
#include "Characters/AttributeSets/GHOAttributeSetAmmo.h"
#include "Player/GHOPlayerState.h"
#include "Player/GHOPlayerController.h"
#include "Game/GHOGameModeBase.h"
#include "UI/GHOFloatingStatusBarWidget.h"
#include "Settings/GHODefaultClasses.h"
#include "Settings/GHOGameplayTags.h"
#include "Settings/GHOCollisions.h"
#include "AI/GHOHeroAIController.h"
#include "Weapons/GHOWeapon.h"



AGHOHeroCharacterBase::AGHOHeroCharacterBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	DefaultFOV = 90.f;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->FieldOfView = DefaultFOV;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	/*
	by GASDocumentation
		Makes sure that the animations play on the Server so that we can use bone and socket transforms
		to do things like spawning projectiles and other FX.
	和訳
		アニメーションがサーバー上で再生されるようにして、
		ボーンとソケットのトランスフォームを使用して
		発射物やその他の FX をスポーンすることができるようにします。
	*/
#if 0
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));
#else
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));
	GetMesh()->SetCollisionResponseToChannel(COLLISION_INTERACTABLE, ECollisionResponse::ECR_Overlap);
	GetMesh()->bCastHiddenShadow = true;
	GetMesh()->bReceivesDecals = false;
#endif

	UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
	UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));
	
	UIFloatingStatusBarClass = FGHODefaultClasses::GetFloatingStatusBarClass();

	/*
	AIControllerClass
	by Epic
		Default class to use when pawn is controlled by AI.
	和訳
		ポーンが AI によってコントロールされている場合に使用されるデフォルトのクラス。
	*/
	AIControllerClass = AGHOHeroAIController::StaticClass();

	ReviveDuration = 4.0f;


	NoWeaponTag = FGameplayTag::RequestGameplayTag(FName(TAG_Weapon_Equipped_None));
	WeaponChangingDelayReplicationTag = FGameplayTag::RequestGameplayTag(FName(TAG_Ability_Weapon_IsChangingDelayReplication));
	WeaponAmmoTypeNoneTag = FGameplayTag::RequestGameplayTag(FName(TAG_Weapon_Ammo_None));
	WeaponAbilityTag = FGameplayTag::RequestGameplayTag(FName(TAG_Ability_Weapon));

	CurrentWeaponTag = NoWeaponTag;
	Inventory = FGHOHeroInventory();
//	CurrentWeapon = nullptr;
	bChangedWeaponLocally = false;
}

void AGHOHeroCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGHOHeroCharacterBase, Inventory);

	/*
	by GASShooter
		Only replicate CurrentWeapon to simulated clients and manually sync CurrentWeeapon with Owner when we're ready.
		This allows us to predict weapon changing.
	和訳
		CurrentWeeapon を simulated clients にのみレプリケーションし、準備ができたら CurrentWeeapon をオーナーと手動で同期します。
		これにより、武器の変更を predict （予測）することができます。
	*/
	DOREPLIFETIME_CONDITION(AGHOHeroCharacterBase, CurrentWeapon, COND_SimulatedOnly);
}

void AGHOHeroCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StartingCameraBoomTargetArmLength = CameraBoom->TargetArmLength;
	StartingCameraBoomRelativeLocation = CameraBoom->GetRelativeLocation();
	//StartingThirdPersonMeshLocation = GetMesh()->GetRelativeLocation();

	GetWorldTimerManager().SetTimerForNextTick(this, &AGHOHeroCharacterBase::SpawnDefaultInventory);
}

void AGHOHeroCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	/*
	by GASDocumentation
		Only needed for Heroes placed in world and when the player is the Server.
		On respawn, they are set up in PossessedBy.
		When the player a client, the floating status bars are all set up in OnRep_PlayerState.
	和訳
		ワールドに配置されたヒーローと、プレイヤーがサーバーである場合にのみ必要。
		リスポーン時には PossessedBy 経由で設定されます。
		プレイヤーがクライアントの場合、 FloatingStatusBar はすべて OnRep_PlayerState で設定されます。
	*/
	InitializeFloatingStatusBar();

	//不要なはず。なぜ含まれていたか不明。
	//8743b5fa4aecb4b891a8b1272efccd30d2d345df で紛れ込んでいた。
	//PostInitializeComponents();
}

void AGHOHeroCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	/*
	by GASShooter
		Cancel being revived if killed
		//InteractionCanceledDelegate.Broadcast();
	和訳
		殺されたら蘇生をキャンセル
		//InteractionCanceledDelegate.Broadcast();
	*/
#if 0
	Execute_InteractableCancelInteraction(this, GetThirdPersonMesh());
#else
	Execute_InteractableCancelInteraction(this, GetMesh());
#endif

	/*
	by GASShooter
		Clear CurrentWeaponTag on the ASC.
		This happens naturally in UnEquipCurrentWeapon() but that is only called on the server from hero death
		(the OnRep_CurrentWeapon() would have handled it on the client but that is never called due to the hero being marked pending destroy).
		This makes sure the client has it cleared.
	和訳
		ASC の CurrentWeaponTag をクリアする。
		これは UnEquipCurrentWeapon() で自然に発生するが、それはサーバー上のみでヒーローの死亡処理から呼び出される
		（ OnRep_CurrentWeapon() はクライアント上でそれを処理するが、ヒーローが pending destory とマークされているため、それは決して呼び出されない）。
		これにより、クライアントがそれをクリア済みなのを確認します。
	*/
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(CurrentWeaponTag);
		CurrentWeaponTag = NoWeaponTag;
		AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);
	}

	Super::EndPlay(EndPlayReason);
}

//Client only
void AGHOHeroCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AGHOPlayerState* playerState = GetPlayerState<AGHOPlayerState>())
	{
		InitializeAbilitySystemWeakObjects(playerState);

		//client only
		if (true)
		{
			/*
			by GASDocumentation
				Bind player input to the AbilitySystemComponent.
				Also called in SetupPlayerInputComponent because of a potential race condition.
			和訳
				プレイヤーの入力を AbilitySystemComponent にバインドします。
				レースコンディションの可能性があるため、 SetupPlayerInputComponent でも呼び出されます。
			*/
			BindASCInput();
		}

		InitializeAttributes();

		////server only
		//if (true)
		//{
		//	AddStartupEffects();
		//	AddCharacterAbilities();
		//}

		//client only
		if (true)
		{
			if (CurrentWeapon)
			{
				/*
				by GASShooter
					If current weapon repped before PlayerState, set tag on ASC
				和訳
					PlayerState より先に repped （複製）された場合、 ASC にタグを設定します。
				*/
				AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);
				/*
				by GASShooter
					Update owning character and ASC just in case it repped before PlayerState
				和訳
					PlayerState より先に repped （複製）された場合に備えて、 owning character と ASC を更新する。
				*/
				CurrentWeapon->SetOwningCharacter(this);

				if (!PrimaryReserveAmmoChangedDelegateHandle.IsValid())
				{
					PrimaryReserveAmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGHOAttributeSetAmmo::GetReserveAmmoAttributeFromTag(CurrentWeapon->PrimaryAmmoType)).AddUObject(this, &AGHOHeroCharacterBase::CurrentWeaponPrimaryReserveAmmoChanged);
				}
				if (!SecondaryReserveAmmoChangedDelegateHandle.IsValid())
				{
					SecondaryReserveAmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGHOAttributeSetAmmo::GetReserveAmmoAttributeFromTag(CurrentWeapon->SecondaryAmmoType)).AddUObject(this, &AGHOHeroCharacterBase::CurrentWeaponSecondaryReserveAmmoChanged);
				}
			}
		}

		InitializeAfterAbilitySystem();
	}
}

//Server only
void AGHOHeroCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AGHOPlayerState* playerState = GetPlayerState<AGHOPlayerState>())
	{
		InitializeAbilitySystemWeakObjects(playerState);

		////client only
		//if (true)
		//{
		//	//by GASDocumentation
		//	//	Bind player input to the AbilitySystemComponent. 
		//	//	Also called in SetupPlayerInputComponent because of a potential race condition.
		//	//和訳
		//	//	プレイヤーの入力を AbilitySystemComponent にバインドします。
		//	//	レースコンディションの可能性があるため、 SetupPlayerInputComponent でも呼び出されます。
		//	BindASCInput();
		//}

		//server only
		if (true)
		{
			WeaponChangingDelayReplicationTagChangedDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(WeaponChangingDelayReplicationTag)
				.AddUObject(this, &AGHOHeroCharacterBase::WeaponChangingDelayReplicationTagChanged);
		}

		InitializeAttributes();

		//server only
		if(true)
		{
			AddStartupEffects();
			AddCharacterAbilities();
		}

		InitializeAfterAbilitySystem();
	}
}

void AGHOHeroCharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
//	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
//	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGHOHeroCharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGHOHeroCharacterBase::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn", this, &AGHOHeroCharacterBase::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGHOHeroCharacterBase::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AGHOHeroCharacterBase::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGHOHeroCharacterBase::LookUpAtRate);

	// handle touch devices
//	PlayerInputComponent->BindTouch(IE_Pressed, this, &AGHOHeroCharacterBase::TouchStarted);
//	PlayerInputComponent->BindTouch(IE_Released, this, &AGHOHeroCharacterBase::TouchStopped);

	// VR headset functionality
//	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AGHOHeroCharacterBase::OnResetVR);

	// Bind player input to the AbilitySystemComponent. Also called in OnRep_PlayerState because of a potential race condition.
	BindASCInput();
}

UAbilitySystemComponent* AGHOHeroCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

UGHOAttributeSetBase* AGHOHeroCharacterBase::GetAttributeSet()
{
	return AttributeSetBase.Get();
}

const UGHOAttributeSetBase* AGHOHeroCharacterBase::GetAttributeSet()const
{
	return AttributeSetBase.Get();
}

void AGHOHeroCharacterBase::FinishDying()
{
#if 0
	// GASDocumentation
	if (GetLocalRole() == ROLE_Authority)
	{
		if (AGHOGameModeBase* GM = Cast<AGHOGameModeBase>(GetWorld()->GetAuthGameMode()))
		{
			GM->HeroDied(GetController());
		}
	}

	Super::FinishDying();
#else
	/*
	by GASShooter
		AGHOHeroCharacterBase doesn't follow AGHOCharacterBase's pattern of Die->Anim->FinishDying
		because AGHOHeroCharacterBase can be knocked down to either be revived, bleed out, or finished off by an enemy.
	和訳
		AGHOHeroCharacterBase は AGHOCharacterBase である Die->Anim->FinishDying という流れに従いません。
		なぜならば、 AGHOHeroCharacterBase はノックダウンの後、蘇生されるか、出血状態になるか、敵にとどめを刺されるかのいずれかになるからです。
	*/

	if (!HasAuthority())
	{
		return;
	}

	RemoveAllWeaponsFromInventory();

	AbilitySystemComponent->RegisterGameplayTagEvent(WeaponChangingDelayReplicationTag).Remove(WeaponChangingDelayReplicationTagChangedDelegateHandle);

	if (AGHOGameModeBase* GM = Cast<AGHOGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GM->HeroDied(GetController());
	}

	RemoveCharacterAbilities();

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->Down(Cast<UGameplayEffect>(DeathEffect->GetDefaultObject()));
	}

	OnCharacterDied.Broadcast(this);

	Super::FinishDying();
#endif
}

FTransform AGHOHeroCharacterBase::GetProjectileTransform(float Range)const
{
	/*
	解説
		Muzzle の向きをそのまま Projectile の射出方向として利用しています。
		GASDocumentation では Muzzle を始点として、カメラの座標からカメラの向き Range の地点を終点としています。
	*/
	FTransform MuzzleTransform = GetMesh()->GetSocketTransform(FName("Muzzle"));
	//FVector Start = MuzzleTransform.GetLocation();
	//FVector End = GetCameraBoom()->GetComponentLocation() + GetFollowCamera()->GetForwardVector() * Range;
	//FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);
	//MuzzleTransform.SetRotation(Rotation.Quaternion());
	//MuzzleTransform.SetScale3D(FVector(1.0f));
	//UE_LOG(LogTemp, Error, TEXT("%s() Muzzle:%s"), *FString(__FUNCTION__), *MuzzleTransform.ToString());
	return MuzzleTransform;
}

bool AGHOHeroCharacterBase::IsAvailableForInteraction_Implementation(UPrimitiveComponent* InteractionComponent) const
{
	/*
	by GASShooter
		Hero is available to be revived if knocked down and is not already being revived.
		If you want multiple heroes reviving someone to speed it up, you would need to change GA_Interact
		(outside the scope of this sample).
	和訳
		ヒーローはノックダウンされた場合、まだ蘇生されていなければ蘇生可能です。
		複数のヒーローで誰かを蘇生させることでそれをスピードアップさせたい場合は、 GA_Interact を変更する必要があります。
		（このサンプルの範囲外です）
	*/
	if (AbilitySystemComponent.IsValid() && AbilitySystemComponent->IsKnockedDown() && !AbilitySystemComponent->IsInteracting())
	{
		return true;
	}

	return IGHOInteractable::IsAvailableForInteraction_Implementation(InteractionComponent);
}

float AGHOHeroCharacterBase::GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const
{
	if (AbilitySystemComponent.IsValid() && AbilitySystemComponent->IsKnockedDown())
	{
		return ReviveDuration;
	}

	return IGHOInteractable::GetInteractionDuration_Implementation(InteractionComponent);
}

void AGHOHeroCharacterBase::PreInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent)
{
	if (!HasAuthority())
	{
		return;
	}

	if (AbilitySystemComponent.IsValid() && AbilitySystemComponent->IsKnockedDown())
	{
		AbilitySystemComponent->TryActivateRevive();
	}
}

void AGHOHeroCharacterBase::PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent)
{
	if (!HasAuthority())
	{
		return;
	}

	if (AbilitySystemComponent.IsValid() && AbilitySystemComponent->IsKnockedDown())
	{
		AbilitySystemComponent->ApplyGameplayEffectToSelf(Cast<UGameplayEffect>(ReviveEffect->GetDefaultObject()), 1.0f, AbilitySystemComponent->MakeEffectContext());
	}
}

void AGHOHeroCharacterBase::GetPreInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const
{
	if (AbilitySystemComponent.IsValid() && AbilitySystemComponent->IsKnockedDown())
	{
		bShouldSync = true;
		Type = EAbilityTaskNetSyncType::OnlyClientWait;
		return;
	}

	IGHOInteractable::GetPreInteractSyncType_Implementation(bShouldSync, Type, InteractionComponent);
}

void AGHOHeroCharacterBase::CancelInteraction_Implementation(UPrimitiveComponent* InteractionComponent)
{
	if (!HasAuthority())
	{
		return;
	}

	if (AbilitySystemComponent.IsValid() && AbilitySystemComponent->IsKnockedDown())
	{
		AbilitySystemComponent->CancelRevive();
	}
}

FSimpleMulticastDelegate* AGHOHeroCharacterBase::GetTargetCancelInteractionDelegate(UPrimitiveComponent* InteractionComponent)
{
	return &InteractionCanceledDelegate;
}

void AGHOHeroCharacterBase::Turn(float Rate)
{
	if (IsAlive())
	{
		AddControllerYawInput(Rate);
	}
}

void AGHOHeroCharacterBase::LookUp(float Rate)
{
	if (IsAlive())
	{
		AddControllerPitchInput(Rate);
	}
}

void AGHOHeroCharacterBase::TurnAtRate(float Rate)
{
	if (IsAlive())
	{
		const float delta = Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds();
		// calculate delta for this frame from the rate information
		AddControllerYawInput(delta);
	}
}

void AGHOHeroCharacterBase::LookUpAtRate(float Rate)
{
	if (IsAlive())
	{
		const float delta = Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds();
		// calculate delta for this frame from the rate information
		AddControllerPitchInput(delta);
	}
}

void AGHOHeroCharacterBase::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AGHOHeroCharacterBase::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

UGHOFloatingStatusBarWidget* AGHOHeroCharacterBase::GetFloatingStatusBar()
{
	return UIFloatingStatusBar;
}

void AGHOHeroCharacterBase::KnockDown()
{
	if (!HasAuthority())
	{
		return;
	}

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->Down(Cast<UGameplayEffect>(KnockDownEffect->GetDefaultObject()));
	}
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetHealth(AttributeSetBase->GetHealthMax());
		//TODO: shield attribute
		//AttributeSetBase->SetShield(0.0f);
	}
}

void AGHOHeroCharacterBase::PlayKnockDownEffects()
{
	//TODO: 1st / 3rd person view
	///*
	//by GASShooter
	//	Store perspective to restore on Revive
	//和訳
	//	蘇生時に復元する視点を保存する。
	//*/
	//bWasInFirstPersonPerspectiveWhenKnockedDown = IsInFirstPersonPerspective();

	//TODO: 1st / 3rd person view
	//SetPerspective(false);

	/*
	by GASShooter
		Play it here instead of in the ability to skip extra replication data
	和訳
		余分なレプリケーションデータをスキップするために、アビリティではなく、ここで再生する。
	*/
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	if (AbilitySystemComponent.IsValid())
	{
		FGameplayCueParameters GCParameters;
		GCParameters.Location = GetActorLocation();
		AbilitySystemComponent->ExecuteGameplayCueLocal(FGameplayTag::RequestGameplayTag(TAG_GameplayCue_Hero_KnockedDown), GCParameters);
	}
}

void AGHOHeroCharacterBase::PlayReviveEffects()
{
	//TODO: 1st / 3rd person view
	///*
	//by GASShooter
	//	Restore perspective the player had when knocked down
	//和訳
	//	倒された時のプレイヤーの視点を元に戻す
	//*/
	//SetPerspective(bWasInFirstPersonPerspectiveWhenKnockedDown);

	//by GASShooter
	//	Play revive particles or sounds here (we don't have any)
	//和訳
	//	蘇生のパーティクルやサウンドをここで再生する（我々は設定してませんが）
	if (AbilitySystemComponent.IsValid())
	{
		FGameplayCueParameters GCParameters;
		GCParameters.Location = GetActorLocation();
		AbilitySystemComponent->ExecuteGameplayCueLocal(FGameplayTag::RequestGameplayTag(TAG_GameplayCue_Hero_Revived), GCParameters);
	}
}

void AGHOHeroCharacterBase::InitializeAbilitySystemWeakObjects(class AGHOPlayerState* playerState)
{
	AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(playerState->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(playerState, this);

	AttributeSetBase = playerState->GetAttributeSetBase();
	AttributeSetAmmo = playerState->GetAttributeSetAmmo();
}

void AGHOHeroCharacterBase::InitializeAfterAbilitySystem()
{
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetController()))
	{
		PC->CreateHUD();
	}

	/*
	by GASDocumentation
		Simulated on proxies don't have their PlayerStates yet when BeginPlay is called so we call it again here
	和訳
		Simulated Proxy は BeginPlay が呼ばれたときにはまだ PlayerState を所持していないので、ここでもう一度呼び出します。
	*/
	InitializeFloatingStatusBar();

	/*
	解説
		Character Abilities の付与の際に死亡していると付与できないため、 ClearDead() を UGHOAbilitySystemComponent::InitializeAttributes() 内に移動します。
	*/
	//if (AbilitySystemComponent.IsValid())
	//{
	//	AbilitySystemComponent->ClearDead();
	//}
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->InitializeAttributesOnSpawned();
	}
}

void AGHOHeroCharacterBase::BindASCInput()
{
	if (!bASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"), FString("CancelTarget"), FString("EGHOAbilityInputID"), static_cast<int32>(EGHOAbilityInputID::Confirm), static_cast<int32>(EGHOAbilityInputID::Cancel)));

		bASCInputBound = true;
	}
}

void AGHOHeroCharacterBase::InitializeFloatingStatusBar()
{
	if (!UIFloatingStatusBarComponent || !UIFloatingStatusBarClass)
	{
		return;
	}
	// Only create once
	if (UIFloatingStatusBar)
	{
		return;
	}
	if(!AbilitySystemComponent.IsValid() || !AttributeSetBase.IsValid())
	{
		return;
	}

	/*
	by GASDocumentation
		Setup UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
	和訳
		ローカルに所有するプレーヤーのみに UI を設定し、 AI やサーバーのコピーの PlayerController には設定しない。
	*/
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (PC->IsLocalPlayerController())
		{
			UIFloatingStatusBar = CreateWidget<UGHOFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
			if (UIFloatingStatusBar)
			{
				UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

				// Setup the floating status bar
				UIFloatingStatusBar->SetCharacterName(FText());
				UIFloatingStatusBar->SetHealthPercentage(AttributeSetBase->GetHealth() / AttributeSetBase->GetHealthMax());
				UIFloatingStatusBar->SetManaPercentage(AttributeSetBase->GetMana() / AttributeSetBase->GetManaMax());
			}
		}
	}
}

AGHOWeapon* AGHOHeroCharacterBase::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

FName AGHOHeroCharacterBase::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

void AGHOHeroCharacterBase::OnRep_Inventory()
{
	if (GetLocalRole() == ROLE_AutonomousProxy && Inventory.Weapons.Num() > 0 && !CurrentWeapon)
	{
		/*
		by GASShooter
			Since we don't replicate the CurrentWeapon to the owning client, this is a way to ask the Server to sync the CurrentWeapon after it's been spawned via replication from the Server.
			The weapon spawning is replicated but the variable CurrentWeapon is not on the owning client.
		和訳
			CurrentWeapon を owning client にレプリケーションしていないので、これは、サーバーからのレプリケーションによってスポーンされた後に CurrentWeapon を同期するようにサーバーに依頼する方法です。
			武器のスポーンはレプリケーションされますが、変数 CurrentWeapon は owning client にはありません。
		*/
		RPCServerSyncCurrentWeapon();
	}
}

void AGHOHeroCharacterBase::OnRep_CurrentWeapon(AGHOWeapon* LastWeapon)
{
	bChangedWeaponLocally = false;
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void AGHOHeroCharacterBase::RPCServerSyncCurrentWeapon_Implementation()
{
	RPCClientSyncCurrentWeapon(CurrentWeapon);
}

bool AGHOHeroCharacterBase::RPCServerSyncCurrentWeapon_Validate()
{
	return true;
}

void AGHOHeroCharacterBase::RPCClientSyncCurrentWeapon_Implementation(AGHOWeapon* InWeapon)
{
	AGHOWeapon* LastWeapon = CurrentWeapon;
	CurrentWeapon = InWeapon;
	OnRep_CurrentWeapon(LastWeapon);
}

bool AGHOHeroCharacterBase::RPCClientSyncCurrentWeapon_Validate(AGHOWeapon* InWeapon)
{
	return true;
}

void AGHOHeroCharacterBase::RPCServerEquipWeapon_Implementation(AGHOWeapon* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

bool AGHOHeroCharacterBase::RPCServerEquipWeapon_Validate(AGHOWeapon* NewWeapon)
{
	return true;
}


void AGHOHeroCharacterBase::SetCurrentWeapon(AGHOWeapon* NewWeapon, AGHOWeapon* LastWeapon)
{
	if (NewWeapon == LastWeapon)
	{
		return;
	}

	/*
	by GASShooter
		Cancel active weapon abilities
	和訳
		アクティブな武器のアビリティをキャンセルする。
	*/
	if (AbilitySystemComponent.IsValid())
	{
		FGameplayTagContainer AbilityTagsToCancel = FGameplayTagContainer(WeaponAbilityTag);
		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel);
	}

	UnEquipWeapon(LastWeapon);

	if (NewWeapon)
	{
		if (AbilitySystemComponent.IsValid())
		{
			/*
			by GASShooter
				Clear out potential NoWeaponTag
			和訳
				潜在的な NoWeaponTag を消去
			*/
			AbilitySystemComponent->RemoveLooseGameplayTag(CurrentWeaponTag);
		}

		/*
		by GASShooter
			Weapons coming from OnRep_CurrentWeapon won't have the owner set
		和訳
			OnRep_CurrentWeapon から送られてくる武器は owner が設定されていない。
		*/
		CurrentWeapon = NewWeapon;
		CurrentWeapon->SetOwningCharacter(this);
		CurrentWeapon->Equip();
		CurrentWeaponTag = CurrentWeapon->WeaponTag;

		if (AbilitySystemComponent.IsValid())
		{
			AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);
		}

		if (AGHOPlayerController* PC = GetController<AGHOPlayerController>())
		{
			if (PC->IsLocalController())
			{
				PC->SetEquippedWeaponPrimaryIconFromSprite(CurrentWeapon->PrimaryIcon);
				PC->SetEquippedWeaponName(CurrentWeapon->WeaponName);
				PC->SetEquippedWeaponStatusText(CurrentWeapon->StatusText);
				PC->SetPrimaryAmmoType(CurrentWeapon->PrimaryAmmoType);
				PC->SetPrimaryClipAmmo(CurrentWeapon->GetPrimaryClipAmmo());
				PC->SetPrimaryReserveAmmo(GetPrimaryReserveAmmo());
				PC->SetSecondaryAmmoType(CurrentWeapon->SecondaryAmmoType);
				PC->SetSecondaryClipAmmo(CurrentWeapon->GetSecondaryClipAmmo());
				PC->SetSecondaryReserveAmmo(GetSecondaryReserveAmmo());
				PC->SetHUDReticle(CurrentWeapon->GetPrimaryHUDReticleClass());
			}
		}

		NewWeapon->OnPrimaryClipAmmoChanged.AddDynamic(this, &AGHOHeroCharacterBase::CurrentWeaponPrimaryClipAmmoChanged);
		NewWeapon->OnSecondaryClipAmmoChanged.AddDynamic(this, &AGHOHeroCharacterBase::CurrentWeaponSecondaryClipAmmoChanged);

		if (AbilitySystemComponent.IsValid())
		{
			PrimaryReserveAmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGHOAttributeSetAmmo::GetReserveAmmoAttributeFromTag(CurrentWeapon->PrimaryAmmoType)).AddUObject(this, &AGHOHeroCharacterBase::CurrentWeaponPrimaryReserveAmmoChanged);
			SecondaryReserveAmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGHOAttributeSetAmmo::GetReserveAmmoAttributeFromTag(CurrentWeapon->SecondaryAmmoType)).AddUObject(this, &AGHOHeroCharacterBase::CurrentWeaponSecondaryReserveAmmoChanged);
		}

#if 0
		//TODO: 1st / 3rd person view
		UAnimMontage* Equip1PMontage = CurrentWeapon->GetEquip1PMontage();
		if (Equip1PMontage && GetFirstPersonMesh())
		{
			GetFirstPersonMesh()->GetAnimInstance()->Montage_Play(Equip1PMontage);
		}

		UAnimMontage* Equip3PMontage = CurrentWeapon->GetEquip3PMontage();
		if (Equip3PMontage && GetThirdPersonMesh())
		{
			GetThirdPersonMesh()->GetAnimInstance()->Montage_Play(Equip3PMontage);
		}
#else
		if (GetMesh())
		{
			if (UAnimMontage* EquipMontage = CurrentWeapon->GetEquipMontage())
			{
				GetMesh()->GetAnimInstance()->Montage_Play(EquipMontage);
			}
		}
#endif
	}
	else
	{
		/*
		by GASShooter
			This will clear HUD, tags etc
		和訳
			HUD やタグなどが消去されます。
		*/
		UnEquipCurrentWeapon();
	}
}

void AGHOHeroCharacterBase::UnEquipWeapon(AGHOWeapon* WeaponToUnEquip)
{
	/*
	by GASShooter
		TODO this will run into issues when calling UnEquipWeapon explicitly and the WeaponToUnEquip == CurrentWeapon
	和訳
		TODO: これは、 UnEquipWeapon を明示的に呼び出し、 WeaponToUnEquip == CurrentWeapon の場合に問題になります。
	*/

	if (WeaponToUnEquip)
	{
		WeaponToUnEquip->OnPrimaryClipAmmoChanged.RemoveDynamic(this, &AGHOHeroCharacterBase::CurrentWeaponPrimaryClipAmmoChanged);
		WeaponToUnEquip->OnSecondaryClipAmmoChanged.RemoveDynamic(this, &AGHOHeroCharacterBase::CurrentWeaponSecondaryClipAmmoChanged);

		if (AbilitySystemComponent.IsValid())
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGHOAttributeSetAmmo::GetReserveAmmoAttributeFromTag(WeaponToUnEquip->PrimaryAmmoType)).Remove(PrimaryReserveAmmoChangedDelegateHandle);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGHOAttributeSetAmmo::GetReserveAmmoAttributeFromTag(WeaponToUnEquip->SecondaryAmmoType)).Remove(SecondaryReserveAmmoChangedDelegateHandle);
		}

		WeaponToUnEquip->UnEquip();
	}
}

void AGHOHeroCharacterBase::UnEquipCurrentWeapon()
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(CurrentWeaponTag);
		CurrentWeaponTag = NoWeaponTag;
		AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);
	}

	UnEquipWeapon(CurrentWeapon);
	CurrentWeapon = nullptr;

	if (AGHOPlayerController* PC = GetController<AGHOPlayerController>())
	{
		if (PC->IsLocalController())
		{
			PC->SetEquippedWeaponPrimaryIconFromSprite(nullptr);
			PC->SetEquippedWeaponName(FText());
			PC->SetEquippedWeaponStatusText(FText());
			PC->SetPrimaryAmmoType(WeaponAmmoTypeNoneTag);
			PC->SetPrimaryClipAmmo(0);
			PC->SetPrimaryReserveAmmo(0);
			PC->SetSecondaryAmmoType(WeaponAmmoTypeNoneTag);
			PC->SetSecondaryClipAmmo(0);
			PC->SetSecondaryReserveAmmo(0);
			PC->SetHUDReticle(nullptr);
		}
	}
}

bool AGHOHeroCharacterBase::AddWeaponToInventory(AGHOWeapon* NewWeapon, bool bEquipWeapon)
{
	if (DoesWeaponExistInInventory(NewWeapon))
	{
		if (USoundCue* PickupSound = NewWeapon->GetPickupSound())
		{
			if (IsLocallyControlled())
			{
				UGameplayStatics::SpawnSoundAttached(PickupSound, GetRootComponent());
			}
		}

		if (GetLocalRole() < ROLE_Authority)
		{
			return false;
		}

		/*
		by gasshooter
			create a dynamic instant gameplay effect to give the primary and secondary ammo
		和訳
			プライマリ弾とセカンダリ弾を与える動的な instant gameplay effect の生成
		*/
		UGameplayEffect* GEAmmo = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Ammo")));
		GEAmmo->DurationPolicy = EGameplayEffectDurationType::Instant;

		if (NewWeapon->PrimaryAmmoType != WeaponAmmoTypeNoneTag)
		{
			const int32 Idx = GEAmmo->Modifiers.Num();
			GEAmmo->Modifiers.SetNum(Idx + 1);

			FGameplayModifierInfo& InfoPrimaryAmmo = GEAmmo->Modifiers[Idx];
			InfoPrimaryAmmo.ModifierMagnitude = FScalableFloat(NewWeapon->GetPrimaryClipAmmo());
			InfoPrimaryAmmo.ModifierOp = EGameplayModOp::Additive;
			InfoPrimaryAmmo.Attribute = UGHOAttributeSetAmmo::GetReserveAmmoAttributeFromTag(NewWeapon->PrimaryAmmoType);
		}

		if (NewWeapon->SecondaryAmmoType != WeaponAmmoTypeNoneTag)
		{
			const int32 Idx = GEAmmo->Modifiers.Num();
			GEAmmo->Modifiers.SetNum(Idx + 1);

			FGameplayModifierInfo& InfoSecondaryAmmo = GEAmmo->Modifiers[Idx];
			InfoSecondaryAmmo.ModifierMagnitude = FScalableFloat(NewWeapon->GetSecondaryClipAmmo());
			InfoSecondaryAmmo.ModifierOp = EGameplayModOp::Additive;
			InfoSecondaryAmmo.Attribute = UGHOAttributeSetAmmo::GetReserveAmmoAttributeFromTag(NewWeapon->SecondaryAmmoType);
		}

		if (GEAmmo->Modifiers.Num() > 0)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(GEAmmo, 1.0f, AbilitySystemComponent->MakeEffectContext());
		}

		NewWeapon->Destroy();

		return false;
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		return false;
	}

	Inventory.Weapons.Add(NewWeapon);
	NewWeapon->SetOwningCharacter(this);
	NewWeapon->AddAbilities();

	if (bEquipWeapon)
	{
		EquipWeapon(NewWeapon);
		RPCClientSyncCurrentWeapon(CurrentWeapon);
	}

	return true;
}

bool AGHOHeroCharacterBase::RemoveWeaponFromInventory(AGHOWeapon* WeaponToRemove)
{
	if (DoesWeaponExistInInventory(WeaponToRemove))
	{
		if (WeaponToRemove == CurrentWeapon)
		{
			UnEquipCurrentWeapon();
		}

		Inventory.Weapons.Remove(WeaponToRemove);
		WeaponToRemove->RemoveAbilities();
		WeaponToRemove->SetOwningCharacter(nullptr);
		WeaponToRemove->ResetWeapon();

		/*
		by GASShooter
			Add parameter to drop weapon?
		和訳
			ドロップ武器にパラメータを追加する？
		*/

		return true;
	}

	return false;
}

void AGHOHeroCharacterBase::RemoveAllWeaponsFromInventory()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	UnEquipCurrentWeapon();

	const float radius = 50.0f;
	const float NumWeapons = Inventory.Weapons.Num();

	for (int32 i = Inventory.Weapons.Num() - 1; i >= 0; i--)
	{
		AGHOWeapon* Weapon = Inventory.Weapons[i];
		RemoveWeaponFromInventory(Weapon);

		/*
		by GASShooter
			Set the weapon up as a pickup
		和訳
			武器をピックアップに設定。
		*/

		const float OffsetX = radius * FMath::Cos((i / NumWeapons) * 2.0f * PI);
		const float OffsetY = radius * FMath::Sin((i / NumWeapons) * 2.0f * PI);
		Weapon->OnDropped(GetActorLocation() + FVector(OffsetX, OffsetY, 0.0f));
	}
}

void AGHOHeroCharacterBase::EquipWeapon(AGHOWeapon* NewWeapon)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		RPCServerEquipWeapon(NewWeapon);
		SetCurrentWeapon(NewWeapon, CurrentWeapon);
		bChangedWeaponLocally = true;
	}
	else
	{
		SetCurrentWeapon(NewWeapon, CurrentWeapon);
	}
}

int32 AGHOHeroCharacterBase::GetPrimaryClipAmmo() const
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetPrimaryClipAmmo();
	}

	return 0;
}

int32 AGHOHeroCharacterBase::GetMaxPrimaryClipAmmo() const
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetMaxPrimaryClipAmmo();
	}

	return 0;
}

int32 AGHOHeroCharacterBase::GetPrimaryReserveAmmo() const
{
	if (CurrentWeapon && AttributeSetAmmo.IsValid())
	//if (CurrentWeapon)
	{
		//FGameplayAttribute Attribute = AttributeSetAmmo->GetReserveAmmoAttributeFromTag(CurrentWeapon->PrimaryAmmoType);
		FGameplayAttribute Attribute = UGHOAttributeSetAmmo::GetReserveAmmoAttributeFromTag(CurrentWeapon->PrimaryAmmoType);
		return Attribute.IsValid() ? AbilitySystemComponent->GetNumericAttribute(Attribute) : 0;
	}

	return 0;
}

int32 AGHOHeroCharacterBase::GetSecondaryReserveAmmo() const
{
	if (CurrentWeapon && AttributeSetAmmo.IsValid())
	//if (CurrentWeapon)
	{
		//FGameplayAttribute Attribute = AttributeSetAmmo->GetReserveAmmoAttributeFromTag(CurrentWeapon->SecondaryAmmoType);
		FGameplayAttribute Attribute = UGHOAttributeSetAmmo::GetReserveAmmoAttributeFromTag(CurrentWeapon->SecondaryAmmoType);
		return Attribute.IsValid() ? AbilitySystemComponent->GetNumericAttribute(Attribute) : 0;
	}

	return 0;
}

void AGHOHeroCharacterBase::CurrentWeaponPrimaryClipAmmoChanged(int32 OldPrimaryClipAmmo, int32 NewPrimaryClipAmmo)
{
	if (AGHOPlayerController* PC = GetController<AGHOPlayerController>())
	{
		if (PC->IsLocalController())
		{
			PC->SetPrimaryClipAmmo(NewPrimaryClipAmmo);
		}
	}
}

void AGHOHeroCharacterBase::CurrentWeaponSecondaryClipAmmoChanged(int32 OldSecondaryClipAmmo, int32 NewSecondaryClipAmmo)
{
	if (AGHOPlayerController* PC = GetController<AGHOPlayerController>())
	{
		if (PC->IsLocalController())
		{
			PC->SetSecondaryClipAmmo(NewSecondaryClipAmmo);
		}
	}
}

void AGHOHeroCharacterBase::CurrentWeaponPrimaryReserveAmmoChanged(const FOnAttributeChangeData& Data)
{
	if (AGHOPlayerController* PC = GetController<AGHOPlayerController>())
	{
		if (PC->IsLocalController())
		{
			PC->SetPrimaryReserveAmmo(Data.NewValue);
		}
	}
}

void AGHOHeroCharacterBase::CurrentWeaponSecondaryReserveAmmoChanged(const FOnAttributeChangeData& Data)
{
	if (AGHOPlayerController* PC = GetController<AGHOPlayerController>())
	{
		if (PC->IsLocalController())
		{
			PC->SetSecondaryReserveAmmo(Data.NewValue);
		}
	}
}

void AGHOHeroCharacterBase::WeaponChangingDelayReplicationTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (CallbackTag == WeaponChangingDelayReplicationTag)
	{
		if (NewCount < 1)
		{
			/*
			by GASShooter
				We only replicate the current weapon to simulated proxies so manually sync it when the weapon changing delay replication tag is removed.
				We keep the weapon changing tag on for ~1s after the equip montage to allow for activating changing weapon again without the server trying to clobber the next locally predicted weapon.
			和訳
				現在の武器は simulated proxies のみにレプリケーションされるので、武器変更遅延レプリケーションタグが削除された時に手動で同期します。
				サーバーが、次に locally predicted （ローカル予測）した武器を壊すことなく、武器変更を再有効化できるように、武器変更タグを装備モンタージュ後一秒間、維持します。
			*/
			RPCClientSyncCurrentWeapon(CurrentWeapon);
		}
	}
}

void AGHOHeroCharacterBase::SpawnDefaultInventory()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	const int32 NumWeaponClasses = DefaultInventoryWeaponClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (!DefaultInventoryWeaponClasses[i])
		{
			/*
			by GASShooter
				An empty item was added to the Array in blueprint
			和訳
				ブループリントで空のアイテムが配列に追加されている。
			*/
			continue;
		}

		AGHOWeapon* NewWeapon = GetWorld()->SpawnActorDeferred<AGHOWeapon>(DefaultInventoryWeaponClasses[i],
			FTransform::Identity, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		//NewWeapon->bSpawnWithCollision = false;
		NewWeapon->SetSpawnWithCollision(false);
		NewWeapon->FinishSpawning(FTransform::Identity);

		const bool bEquipFirstWeapon = i == 0;
		AddWeaponToInventory(NewWeapon, bEquipFirstWeapon);
	}
}

bool AGHOHeroCharacterBase::DoesWeaponExistInInventory(AGHOWeapon* InWeapon)
{
	//UE_LOG(LogTemp, Log, TEXT("%s InWeapon class %s"), *FString(__FUNCTION__), *InWeapon->GetClass()->GetName());
	if (!InWeapon)
	{
		return false;
	}
	const auto inClass = InWeapon->GetClass();

	for (AGHOWeapon* Weapon : Inventory.Weapons)
	{
		//if (Weapon && InWeapon && Weapon->GetClass() == InWeapon->GetClass())
		if (Weapon && Weapon->GetClass() == inClass)
		{
			return true;
		}
	}

	return false;
}
