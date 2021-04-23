// (C) Sentya Anko 2021


#include "Player/GHOPlayerController.h"
#include "Player/GHOPlayerState.h"
#include "AbilitySystemComponent.h"


void AGHOPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void AGHOPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (AGHOPlayerState* playerState = GetPlayerState<AGHOPlayerState>())
	{
		playerState->GetAbilitySystemComponent()->InitAbilityActorInfo(playerState, aPawn);
	}
}
