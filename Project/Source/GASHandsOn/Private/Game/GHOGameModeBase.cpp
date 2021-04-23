// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GHOGameModeBase.h"
#include "Characters/GHOHeroCharacterBase.h"
#include "Player/GHOPlayerController.h"
#include "Game/GHOGameStateBase.h"
#include "Player/GHOPlayerState.h"

AGHOGameModeBase::AGHOGameModeBase()
{
	DefaultPawnClass = AGHOHeroCharacterBase::StaticClass();
	PlayerControllerClass = AGHOPlayerController::StaticClass();
	GameStateClass = AGHOGameStateBase::StaticClass();
	PlayerStateClass = AGHOPlayerState::StaticClass();
}

