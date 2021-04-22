// Fill out your copyright notice in the Description page of Project Settings.


#include "GHOGameModeBase.h"
#include "GHOHeroCharacterBase.h"
#include "GHOPlayerController.h"
#include "GHOGameStateBase.h"
#include "GHOPlayerState.h"

AGHOGameModeBase::AGHOGameModeBase()
{
	DefaultPawnClass = AGHOHeroCharacterBase::StaticClass();
	PlayerControllerClass = AGHOPlayerController::StaticClass();
	GameStateClass = AGHOGameStateBase::StaticClass();
	PlayerStateClass = AGHOPlayerState::StaticClass();
}

