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

	/*
	by GASDocumentation
		Get the enemy hero spawn point
	和訳
		敵のヒーローのスポーンポイントの取得
	*/
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
		PC->RPCClientSetRespawnCountdown(RespawnDelay);
	}
}

void AGHOGameModeBase::RespawnHero(AController* Controller)
{
	if (Controller->IsPlayerController())
	{
		/*
		by GASDocumentation
			Respawn player hero
		和訳
			プレイヤーのヒーローのリスポーン
		*/
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
		/*
		by GASDocumentation
			Respawn AI hero
		和訳
			AI のヒーローのリスポーン
		*/
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AGHOHeroCharacterBase* Hero = GetWorld()->SpawnActor<AGHOHeroCharacterBase>(HeroClass, EnemySpawnPoint->GetActorTransform(), SpawnParameters);

		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Hero);
	}
}
