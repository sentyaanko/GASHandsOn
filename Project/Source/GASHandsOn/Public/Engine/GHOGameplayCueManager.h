// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "GHOGameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()
	
protected:
	/*
	by GASShooter
		This makes it so GameplayCues will load the first time that they're requested (or use your AssetManager to manually load them).
		By default it loads *every* GameplayCue in the project and all of their referenced assets when the map starts. 
		In a large game with lots of GCs, this could be hundreds of megabytes or more of unused assets loaded in RAM if particular GCs are not used in the current map.
	和訳
		これにより、 GameplayCues が最初にリクエストされたときにロードされるようになります（または AssetManager を使用して手動でロードするようになります）。
		デフォルトではマップのロード時に プロジェクト内の *すべての* GameplayCue と、その参照されるすべてのアセットがロードされます。
		たくさんの GC を使用する大規模のゲームでは、現在のマップで特定の GC が使用されていない場合、数百Mバイト以上の使用されないアセットが RAM にロードされる可能性があります。
	*/
	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const
	{
		UE_LOG(LogTemp, Error, TEXT("%s() XXXXXXXXXX"), *FString(__FUNCTION__));
		//return true; // Default
		return false;
	}
};
