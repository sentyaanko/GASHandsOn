// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GHOHUDWidgetParameter.generated.h"


USTRUCT(BlueprintType)
struct GASHANDSON_API FGHOGaugeWidgetParameter
{
	GENERATED_BODY()

	FGHOGaugeWidgetParameter()
		: FGHOGaugeWidgetParameter(0.f, 0.f, 0.f)
	{
	}

	FGHOGaugeWidgetParameter(float InCurrent, float InMax, float InRegenRate)
		: Current(InCurrent)
		, Max(InMax)
		, RegenRate(InRegenRate)
	{
	}

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn")
	float Current;

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn")
	float Max;

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn")
	float RegenRate;
};

USTRUCT(BlueprintType)
struct GASHANDSON_API FGHOHUDWidgetParameter
{
	GENERATED_BODY()

	FGHOHUDWidgetParameter()
	: FGHOHUDWidgetParameter(0, 0, 0, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f)
	{
	}

	FGHOHUDWidgetParameter(int32 InHeroLevel, int32 InXP, int32 InGold, float InMoveSpeed, float InHealth, float InHealthMax, float InHealthRegenRate, float InMana, float InManaMax, float InManaRegenRate, float InStamina, float InStaminaMax, float InStaminaRegenRate, float InShield, float InShieldMax, float InShieldRegenRate)
		: HeroLevel(InHeroLevel)
		, XP(InXP)
		, Gold(InGold)
		, MoveSpeed(InMoveSpeed)
		, Health(InHealth, InHealthMax, InHealthRegenRate)
		, Mana(InMana, InManaMax, InManaRegenRate)
		, Stamina(InStamina, InStaminaMax, InStaminaRegenRate)
		, Shield(InShield, InShieldMax, InShieldRegenRate)
	{
	}

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn|HeroLevel")
	int32 HeroLevel;

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn|XP")
	int32 XP;

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn|Gold")
	int32 Gold;

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn|MoveSpeed")
	float MoveSpeed;

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn|Health")
	FGHOGaugeWidgetParameter Health;

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn|Mana")
	FGHOGaugeWidgetParameter Mana;

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn|Stamina")
	FGHOGaugeWidgetParameter Stamina;

	UPROPERTY(BlueprintReadWrite, Category = "GASHandsOn|Shield")
	FGHOGaugeWidgetParameter Shield;
	
};
