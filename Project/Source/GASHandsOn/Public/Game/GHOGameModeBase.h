// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GHOGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API AGHOGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGHOGameModeBase();

	// AActor interface
protected:
	/*
	by Epic
		Overridable native event for when play begins for this actor.
	和訳
		このアクタの再生が始まるときの、オーバーライド可能なネイティブイベント。
	*/
	virtual void BeginPlay();

	// End of AActor interface

public:
	void HeroDied(AController* Controller);

private:
	void RespawnHero(AController* Controller);

private:
	float RespawnDelay;
	TSubclassOf<class AGHOHeroCharacterBase> HeroClass;

	AActor* EnemySpawnPoint;
};
