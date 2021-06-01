// (C) Sentya Anko 2021


#include "Game/GHOGameModeBase.h"
#include "GameFramework/SpectatorPawn.h"
#include "Characters/GHOHeroCharacterBase.h"
#include "Player/GHOPlayerController.h"
#include "Game/GHOGameStateBase.h"
#include "Player/GHOPlayerState.h"
#include "Settings/GHODefaultClasses.h"
#include "Kismet/GameplayStatics.h"


AGHOGameModeBase::AGHOGameModeBase()
{
	DefaultPawnClass = AGHOHeroCharacterBase::StaticClass();
	PlayerControllerClass = AGHOPlayerController::StaticClass();
	GameStateClass = AGHOGameStateBase::StaticClass();
	PlayerStateClass = AGHOPlayerState::StaticClass();

	RespawnDelay = 5.f;
	HeroClass = FGHODefaultClasses::GetHeroClass();
}

void AGHOGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Get the enemy hero spawn point
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		if (Actor->GetName() == FString("EnemyHeroSpawn"))
		{
			EnemySpawnPoint = Actor;
			break;
		}
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

	if (AGHOPlayerController* PC = Cast<AGHOPlayerController>(Controller))
	{
		PC->SetRespawnCountdown(RespawnDelay);
	}
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
	else
	{
		// Respawn AI hero
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AGHOHeroCharacterBase* Hero = GetWorld()->SpawnActor<AGHOHeroCharacterBase>(HeroClass, EnemySpawnPoint->GetActorTransform(), SpawnParameters);

		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Hero);
	}
}
