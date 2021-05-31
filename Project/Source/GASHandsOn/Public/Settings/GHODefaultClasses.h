// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "UI/GHOFloatingDamageTextComponent.h"
#include "Characters/GHOHeroCharacterBase.h"
#include "UI/GHOHUDWidget.h"
//#include "GHODefaultClasses.generated.h"

class FGHODefaultClasses
{
public:
	static TSubclassOf<class UGHOHUDWidget> GetHUDWidgetClass();
	static TSubclassOf<class UGHOFloatingDamageTextComponent> GetFloatingDamageTextComponentClass();
	static TSubclassOf<class UGHOFloatingStatusBarWidget> GetFloatingStatusBarClass();
	static TSubclassOf<class UGHOFloatingStatusBarWidget> GetFloatingStatusBarForMinionClass();
	static TSubclassOf<class AGHOHeroCharacterBase> GetHeroClass();
};
