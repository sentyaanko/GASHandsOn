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

	UPROPERTY(BlueprintReadWrite)
	float Current;

	UPROPERTY(BlueprintReadWrite)
	float Max;

	UPROPERTY(BlueprintReadWrite)
	float RegenRate;
};

USTRUCT(BlueprintType)
struct GASHANDSON_API FGHOHUDWidgetParameter
{
	GENERATED_BODY()

	FGHOHUDWidgetParameter()
	: FGHOHUDWidgetParameter(0, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f)
	{
	}

	FGHOHUDWidgetParameter(int32 InHeroLevel, float InMoveSpeed, float InHealth, float InHealthMax, float InHealthRegenRate, float InMana, float InManaMax, float InManaRegenRate, float InStamina, float InStaminaMax, float InStaminaRegenRate)
		: HeroLevel(InHeroLevel)
		, MoveSpeed(InMoveSpeed)
		, Health(InHealth, InHealthMax, InHealthRegenRate)
		, Mana(InMana, InManaMax, InManaRegenRate)
		, Stamina(InStamina, InStaminaMax, InStaminaRegenRate)
	{
	}

	UPROPERTY(BlueprintReadWrite, Category = "HeroLevel")
	int32 HeroLevel;

	UPROPERTY(BlueprintReadWrite, Category = "MoveSpeed")
	float MoveSpeed;

	UPROPERTY(BlueprintReadWrite, Category = "Health")
	FGHOGaugeWidgetParameter Health;

	UPROPERTY(BlueprintReadWrite, Category = "Mana")
	FGHOGaugeWidgetParameter Mana;

	UPROPERTY(BlueprintReadWrite, Category = "Stamina")
	FGHOGaugeWidgetParameter Stamina;

};
