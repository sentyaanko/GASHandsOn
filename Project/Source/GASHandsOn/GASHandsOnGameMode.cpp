// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASHandsOnGameMode.h"
#include "GASHandsOnCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGASHandsOnGameMode::AGASHandsOnGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
