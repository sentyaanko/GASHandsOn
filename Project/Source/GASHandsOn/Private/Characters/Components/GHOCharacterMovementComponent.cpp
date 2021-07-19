// (C) Sentya Anko 2021


#include "Characters/Components/GHOCharacterMovementComponent.h"
#include "Characters/GHOCharacterBase.h"
#include "Characters/AttributeSets/GHOAttributeSetBase.h"
#include "Settings/GHOGameplayTags.h"

UGHOCharacterMovementComponent::UGHOCharacterMovementComponent()
{
	/*
	解説
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
	

	SprintingSpeedMultiplier = 1.4f;
	AimDownSightsSpeedMultiplier = 0.5f;
	KnockedDownSpeedMultiplier = 0.4f;
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
		解説
			UCharacterMovementComponent::NetworkMaxSmoothUpdateDistance
			UCharacterMovementComponent::NetworkNoSmoothUpdateDistance
			の値を変更することでコンストラクタ呼び出し時に初期化されるように変更したので、こちらはコメントアウト。
		*/
		//MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		//MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
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

	if(!Owner->IsMovable())
	{
		return 0.f;
	}
	if (Owner->IsKnockedDown())
	{
		return Owner->GetAttributeSet()->GetMoveSpeed() * KnockedDownSpeedMultiplier;
	}
	return Owner->GetAttributeSet()->GetMoveSpeed() * GetSpeedMultiplier();
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
	SetSprinting(Flags & FGHOSavedMove::FLAG_Custom_Sprinting);
	SetAimDownSights(Flags & FGHOSavedMove::FLAG_Custom_AimDownSights);
}

void UGHOCharacterMovementComponent::StartSprinting()
{
	SetSprinting(true);
}

void UGHOCharacterMovementComponent::StopSprinting()
{
	SetSprinting(false);
}

void UGHOCharacterMovementComponent::StartAimDownSights()
{
	SetAimDownSights(true);
}

void UGHOCharacterMovementComponent::StopAimDownSights()
{
	SetAimDownSights(false);
}

bool UGHOCharacterMovementComponent::IsSprinting()const
{
	return EnumHasAnyFlags(MoveFlag, EGHOMoveFlag::MF_Sprinting);
}

void UGHOCharacterMovementComponent::SetSprinting(bool flag)
{
	if (flag)
		EnumAddFlags(MoveFlag, EGHOMoveFlag::MF_Sprinting);
	else
		EnumRemoveFlags(MoveFlag, EGHOMoveFlag::MF_Sprinting);
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
	if (IsSprinting())
		return SprintingSpeedMultiplier;
	else if (IsAimDownSights())
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
	/*
		基本的に MoveFlag の状態が異なる場合は結合することが出来ないため、単純に MoveFlag をそのまま比較しています。
	*/
	if (MoveFlag != ((FGHOSavedMove*)&NewMove)->MoveFlag)
	{
		return false;
	}
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

	if (IsSprinting())
		Result |= FLAG_Custom_Sprinting;
	if(IsAimDownSights())
		Result |= FLAG_Custom_AimDownSights;
	return Result;

}

bool UGHOCharacterMovementComponent::FGHOSavedMove::IsSprinting()const
{
	return EnumHasAnyFlags(MoveFlag, EGHOMoveFlag::MF_Sprinting);
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
