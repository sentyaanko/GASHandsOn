// (C) Sentya Anko 2021


#include "Player/GHOPlayerController.h"
#include "Player/GHOPlayerState.h"
#include "AbilitySystemComponent.h"
#include "UI/GHOHUDWidget.h"
#include "Characters/Abilities/GHOAttributeSetBase.h"


AGHOPlayerController::AGHOPlayerController()
{

}

void AGHOPlayerController::CreateHUD()
{
	if (UIHUDWidget)
	{
		return;
	}
	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}
	// Only create a HUD for local player
	if (!IsLocalPlayerController())
	{
		return;
	}

	// Need a valid PlayerState to get attributes from
	if (AGHOPlayerState* playerState = GetPlayerState<AGHOPlayerState>())
	{
		if (UGHOAttributeSetBase* attr = Cast<UGHOAttributeSetBase>(playerState->GetAttributeSetBase()))
		{
			UIHUDWidget = CreateWidget<UGHOHUDWidget>(this, UIHUDWidgetClass);
			UIHUDWidget->AddToViewport();

			// Set attributes
			UIHUDWidget->SetParamter(attr->GetHUDParameter());
		}
	}
}

void AGHOPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	/*
	by GASDocumentation
		For edge cases where the PlayerState is repped before the Hero is possessed.
	和訳
		Hero の possessed が呼ばれる前に PlayerState が Rep される エッジケース対策
	*/
	CreateHUD();

}

void AGHOPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (AGHOPlayerState* playerState = GetPlayerState<AGHOPlayerState>())
	{
		playerState->GetAbilitySystemComponent()->InitAbilityActorInfo(playerState, aPawn);
	}
}
