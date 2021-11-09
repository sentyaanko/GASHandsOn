// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GHOAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UGHOAssetManager& Get();

public:
	/*
	by Epic
		Starts initial load, gets called from InitializeObjectReferences
	和訳
		InitializeObjectReferences から呼び出され、イニシャルロードを開始する。
	*/
	virtual void StartInitialLoading() override;

};
