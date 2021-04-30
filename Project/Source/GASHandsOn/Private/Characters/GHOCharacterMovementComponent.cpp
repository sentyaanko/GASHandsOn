// (C) Sentya Anko 2021


#include "Characters/GHOCharacterMovementComponent.h"
#include "Characters/GHOCharacterBase.h"

UGHOCharacterMovementComponent::UGHOCharacterMovementComponent()
{
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
#if 0
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
#endif
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
//TODO :After merge Health implement.
//if(!Owner->IsMovable())
//{
//    return 0.f;
//}


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
return 600.f * GetSpeedMultiplier(MoveFlag);
#endif
}

void UGHOCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	//The Flags parameter contains the compressed input flags that are stored in the saved move.
	//UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
	//It basically just resets the movement component to the state when the move was made so it can simulate from there.
#if 0
	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;

	RequestToStartADS = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
#else
	if (Flags & FSavedMove_Character::FLAG_Custom_1)
		StartAimDownSights();
	else
		StopAimDownSights();
#endif
}

void UGHOCharacterMovementComponent::StartAimDownSights()
{
	EnumAddFlags(MoveFlag, EGHOMoveFlag::MF_AimDownSights);
}

void UGHOCharacterMovementComponent::StopAimDownSights()
{
	EnumRemoveFlags(MoveFlag, EGHOMoveFlag::MF_AimDownSights);
}

float UGHOCharacterMovementComponent::GetSpeedMultiplier(const EGHOMoveFlag& Flag)const
{
	if (EnumHasAnyFlags(Flag, EGHOMoveFlag::MF_AimDownSights))
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
	//Set which moves can be combined together. This will depend on the bit flags that are used.
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
	if(EnumHasAnyFlags(MoveFlag, EGHOMoveFlag::MF_AimDownSights))
		Result |= FLAG_Custom_1;
#endif
	return Result;

}

UGHOCharacterMovementComponent::FGHONetworkPredictionData_Client::FGHONetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement)
	:Super(ClientMovement)
{

}

FSavedMovePtr UGHOCharacterMovementComponent::FGHONetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FGHOSavedMove());
}
