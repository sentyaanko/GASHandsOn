// (C) Sentya Anko 2021


#include "Game/GHOGameModeBase.h"
#include "GameFramework/SpectatorPawn.h"
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

	RespawnDelay = 5.f;
	HeroClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/GASHandsOn/Characters/Heroes/Default/BP_DefaultHero.BP_DefaultHero_C"));
	if (!HeroClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find HeroClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
	}
}

void AGHOGameModeBase::HeroDied(AController* Controller)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, Controller->GetPawn()->GetActorTransform(), SpawnParameters);

	Controller->UnPossess();
	Controller->Possess(SpectatorPawn);

	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;

	RespawnDelegate = FTimerDelegate::CreateUObject(this, &AGHOGameModeBase::RespawnHero, Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);

	//TODO: Call AGHOPlayerController::SetRespawnCountdown() for display RespawnCountdown.
}

void AGHOGameModeBase::RespawnHero(AController* Controller)
{
	if (Controller->IsPlayerController())
	{
		// Respawn player hero
		AActor* PlayerStart = FindPlayerStart(Controller);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AGHOHeroCharacterBase* Hero = GetWorld()->SpawnActor<AGHOHeroCharacterBase>(HeroClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParameters);

		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Hero);
	}
	//TODO: Implement for AI Hero.
}
