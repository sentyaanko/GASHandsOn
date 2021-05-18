// (C) Sentya Anko 2021


#include "Settings/GHODefaultClasses.h"


enum EFGHODefaultClassNameID
{
	HUDWidgetClass,
	FloatingDamageTextComponentClass,
	FloatingStatusBarClass,
	HeroClass,
	NumberOfTable,
};


const TCHAR* GetDefaultClassName(EFGHODefaultClassNameID NameID)
{
	static const TCHAR* Names[EFGHODefaultClassNameID::NumberOfTable] =
	{
		TEXT("/Game/GASHandsOn/UI/HUD/WBP_HUD.WBP_HUD_C"),
		TEXT("/Game/GASHandsOn/UI/FloatingDamageText/BP_FloatingDamageTextComponent.BP_FloatingDamageTextComponent_C"),
		TEXT("/Game/GASHandsOn/UI/FloatingStatusBar/WBP_FloatingStatusBar.WBP_FloatingStatusBar_C"),
		TEXT("/Game/GASHandsOn/Characters/Heroes/Default/BP_DefaultHero.BP_DefaultHero_C"),
	};
	return Names[NameID];
}

template<typename T_> TSubclassOf<T_> GetDefaultClass(const EFGHODefaultClassNameID NameID)
{
	const TCHAR* Name = GetDefaultClassName(NameID);
	TSubclassOf<T_> ret = StaticLoadClass(UObject::StaticClass(), nullptr, Name);
	if (!ret)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find [%s]. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__), Name);
	}
	return ret;
}

TSubclassOf<class UGHOHUDWidget> FGHODefaultClasses::GetHUDWidgetClass()
{
	return GetDefaultClass<UGHOHUDWidget>(EFGHODefaultClassNameID::HUDWidgetClass);
}

TSubclassOf<class UGHOFloatingDamageTextComponent> FGHODefaultClasses::GetFloatingDamageTextComponentClass()
{
	return GetDefaultClass<UGHOFloatingDamageTextComponent>(EFGHODefaultClassNameID::FloatingDamageTextComponentClass);
}

TSubclassOf<class UGHOFloatingStatusBarWidget> FGHODefaultClasses::GetFloatingStatusBarClass()
{
	return GetDefaultClass<UGHOFloatingStatusBarWidget>(EFGHODefaultClassNameID::FloatingStatusBarClass);
}

TSubclassOf<class AGHOHeroCharacterBase> FGHODefaultClasses::GetHeroClass()
{
	return GetDefaultClass<AGHOHeroCharacterBase>(EFGHODefaultClassNameID::HeroClass);
}

