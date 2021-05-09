// (C) Sentya Anko 2021


#include "Characters/GHOHeroCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Characters/Abilities/GHOAbilitySystemComponent.h"
#include "Characters/Abilities/GHOAttributeSetBase.h"
#include "Player/GHOPlayerState.h"
#include "Player/GHOPlayerController.h"


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
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

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
		//	/*
		//	by GASDocumentation
		//		Bind player input to the AbilitySystemComponent. 
		//		Also called in SetupPlayerInputComponent because of a potential race condition.
		//	和訳
		//		プレイヤーの入力を AbilitySystemComponent にバインドします。
		//		レースコンディションの可能性があるため、 SetupPlayerInputComponent でも呼び出されます。
		//	*/
		//	BindASCInput();
		//}

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
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGHOHeroCharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
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

void AGHOHeroCharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGHOHeroCharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
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

void AGHOHeroCharacterBase::InitializeAbilitySystemWeakObjects(class AGHOPlayerState* playerState)
{
	AbilitySystemComponent = Cast<UGHOAbilitySystemComponent>(playerState->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(playerState, this);

	AttributeSetBase = playerState->GetAttributeSetBase();
}

void AGHOHeroCharacterBase::InitializeAfterAbilitySystem()
{
	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(GetController()))
	{
		PC->CreateHUD();
	}
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->ClearDead();
	}
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->InitializeAttributesOnSpawned();
	}
}

void AGHOHeroCharacterBase::BindASCInput()
{
	if (!bASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
//		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"), FString("CancelTarget"), FString("EGHOAbilityInputID"), static_cast<int32>(EGHOAbilityInputID::Confirm), static_Cast<int32>(EGHOAbilityInputID::Cancel)));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString(), FString(), FString("EGHOAbilityInputID")));

		bASCInputBound = true;
	}
}
