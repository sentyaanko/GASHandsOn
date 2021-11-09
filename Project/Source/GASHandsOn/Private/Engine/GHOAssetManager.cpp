// (C) Sentya Anko 2021


#include "Engine/GHOAssetManager.h"
#include "AbilitySystemGlobals.h"

/*
解説
	詳しくは
	* [「GASDocumentation」の「3. Setting Up a Project Using GAS」](https://github.com/tranek/GASDocumentation#setup) [(和訳)](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#setup)
	* [「GASDocumentation」の「4.9.1 InitGlobalData()」](https://github.com/tranek/GASDocumentation#concepts-asg-initglobaldata) [(和訳)](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#concepts-asg-initglobaldata)
	* [「GASDocumentation」の「4.11.1 Target Data」](https://github.com/tranek/GASDocumentation#concepts-targeting-data) [(和訳)](https://github.com/sentyaanko/GASDocumentation/blob/lang-ja/README.jp.md#concepts-targeting-data)
	を参照。
*/

UGHOAssetManager& UGHOAssetManager::Get()
{
	UGHOAssetManager* Singleton = Cast<UGHOAssetManager>(GEngine->AssetManager);

	if (Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be GHOAssetManager!"));
		return *NewObject<UGHOAssetManager>();	 // never calls this
	}
}


void UGHOAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}
