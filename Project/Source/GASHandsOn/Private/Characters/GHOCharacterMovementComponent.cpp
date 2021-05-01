// (C) Sentya Anko 2021


#include "Characters/GHOCharacterMovementComponent.h"
#include "Characters/GHOCharacterBase.h"

UGHOCharacterMovementComponent::UGHOCharacterMovementComponent()
{
	/*
		基底クラス UCharacterMovementComponent の
		NetworkMaxSmoothUpdateDistance と NetworkNoSmoothUpdateDistance の値を変更する事で
		GetPredictionData_Client() の呼び出し時に new される
		FGHONetworkPredictionData_Client の基底クラス
		FNetworkPredictionData_Client_Character のメンバ変数
		MaxSmoothNetUpdateDist と NoSmoothNetUpdateDist の初期値となる。

		GASDocumentation では GetPredictionData_Client() で
		MaxSmoothNetUpdateDist と NoSmoothNetUpdateDist を直接変更していたが、
		コンソトラクタ実行時に値を変える機能を利用するほうが自然と感じたので、
		このプロジェクトではそちらを利用することで	同様に動作するように実装を変更している。
	*/
	NetworkMaxSmoothUpdateDistance = 92.f;
	NetworkNoSmoothUpdateDistance = 140.f;
	
	
	AimDownSightsSpeedMultiplier = 0.5f;
	MoveFlag = EGHOMoveFlag::MF_None;
}

FNetworkPredictionData_Client * UGHOCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UGHOCharacterMovementComponent* MutableThis = const_cast<UGHOCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FGHONetworkPredictionData_Client(*this);
/*
		//UCharacterMovementComponent::NetworkMaxSmoothUpdateDistance
		//UCharacterMovementComponent::NetworkNoSmoothUpdateDistance
		//の値を変更することでコンストラクタ呼び出し時に初期化されるように変更したので、こちらはコメントアウト。
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
*/
	}

	return ClientPredictionData;
}

float UGHOCharacterMovementComponent::GetMaxSpeed() const
{
	AGHOCharacterBase* Owner = Cast<AGHOCharacterBase>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
		return Super::GetMaxSpeed();
	}

#if 0
	if (!Owner->IsAlive())
	{
		return 0.0f;
	}

	if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))))
	{
	return 0.0f;
	}
#else
	if(!Owner->IsMovable())
	{
		return 0.f;
	}

#endif
#if 0
#if 0
	if (RequestToStartSprinting)
	{
		return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
	}
	#endif
	if (RequestToStartADS)
	{
		return Owner->GetMoveSpeed() * AimDownSightsSpeedMultiplier;
	}

	return Owner->GetMoveSpeed();
#else
	//TODO: Fix after MoveSpeed Attribute implemented.
	//return Owner->GetMoveSpeed() * GetSpeedMultiplier(MoveFlag);
	return 600.f * GetSpeedMultiplier();
#endif
}

void UGHOCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	/*
	by GASDocumentation
		The Flags parameter contains the compressed input flags that are stored in the saved move.
		UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
		It basically just resets the movement component to the state when the move was made so it can simulate from there.
	和訳
		Flags パラメタには、 SavedMove に格納されている圧縮されたフラグが含まれています。
		UpdateFromCompressed flags は SavedMove からフラグを MovementComponent にコピーするだけです。
		基本的には、 MovementComponent を、その移動が行われた時の状態にリセットするだけなので、そこからシミュレーションを行うことが出来ます。
	*/
#if 0
	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;

	RequestToStartADS = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
#else
	SetAimDownSights(Flags & FGHOSavedMove::FLAG_Custom_AimDownSights);
#endif
}

void UGHOCharacterMovementComponent::StartAimDownSights()
{
	SetAimDownSights(true);
}

void UGHOCharacterMovementComponent::StopAimDownSights()
{
	SetAimDownSights(false);
}

bool UGHOCharacterMovementComponent::IsAimDownSights()const
{
	return EnumHasAnyFlags(MoveFlag, EGHOMoveFlag::MF_AimDownSights);
}

void UGHOCharacterMovementComponent::SetAimDownSights(bool flag)
{
	if (flag)
		EnumAddFlags(MoveFlag, EGHOMoveFlag::MF_AimDownSights);
	else
		EnumRemoveFlags(MoveFlag, EGHOMoveFlag::MF_AimDownSights);
}

float UGHOCharacterMovementComponent::GetSpeedMultiplier()const
{
	if (IsAimDownSights())
		return AimDownSightsSpeedMultiplier;
	else
		return 1.f;
}

void UGHOCharacterMovementComponent::FGHOSavedMove::Clear()
{
	Super::Clear();
	MoveFlag = EGHOMoveFlag::MF_None;
}

void UGHOCharacterMovementComponent::FGHOSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	if (UGHOCharacterMovementComponent* CharacterMovement = Cast<UGHOCharacterMovementComponent>(Character->GetCharacterMovement()))
	{
		MoveFlag = CharacterMovement->GetMoveFlag();
	}
}

bool UGHOCharacterMovementComponent::FGHOSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	/*
	by GASDocumentation
		Set which moves can be combined together. 
		This will depend on the bit flags that are used.
	和訳
		どの動きを結合できるかを設定します。
		これは使用されているビットフラグによって異なります。
	*/
#if 0
	if (SavedRequestToStartSprinting != ((FGHOSavedMove*)&NewMove)->SavedRequestToStartSprinting)
	{
		return false;
	}

	if (SavedRequestToStartADS != ((FGHOSavedMove*)&NewMove)->SavedRequestToStartADS)
	{
		return false;
	}
#else
	/*
		基本的に MoveFlag の状態が異なる場合は結合することが出来ないため、単純に MoveFlag をそのまま比較しています。
	*/
	if (MoveFlag != ((FGHOSavedMove*)&NewMove)->MoveFlag)
	{
		return false;
	}
#endif
	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UGHOCharacterMovementComponent::FGHOSavedMove::PrepMoveFor(class ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	if (UGHOCharacterMovementComponent* CharacterMovement = Cast<UGHOCharacterMovementComponent>(Character->GetCharacterMovement()))
	{
	}

}

uint8 UGHOCharacterMovementComponent::FGHOSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

#if 0
	if (SavedRequestToStartSprinting)
	{
		Result |= FLAG_Custom_0;
	}

	if (SavedRequestToStartADS)
	{
		Result |= FLAG_Custom_1;
	}
#else
	if(IsAimDownSights())
		Result |= FLAG_Custom_AimDownSights;
#endif
	return Result;

}

bool UGHOCharacterMovementComponent::FGHOSavedMove::IsAimDownSights()const
{
	return EnumHasAnyFlags(MoveFlag, EGHOMoveFlag::MF_AimDownSights);
}

UGHOCharacterMovementComponent::FGHONetworkPredictionData_Client::FGHONetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement)
	:Super(ClientMovement)
{

}

FSavedMovePtr UGHOCharacterMovementComponent::FGHONetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FGHOSavedMove());
}
