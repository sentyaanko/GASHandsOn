// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GHOHUDWidgetParameter.generated.h"


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
		, Health(InHealth)
		, HealthMax(InHealthMax)
		, HealthRegenRate(InHealthRegenRate)
		, Mana(InMana)
		, ManaMax(InManaMax)
		, ManaRegenRate(InManaRegenRate)
		, Stamina(InStamina)
		, StaminaMax(InStaminaMax)
		, StaminaRegenRate(InStaminaRegenRate)
	{
	}

	UPROPERTY(BlueprintReadWrite, Category = "HeroLevel")
	int32 HeroLevel;

	UPROPERTY(BlueprintReadWrite, Category = "MoveSpeed")
	float MoveSpeed;

	UPROPERTY(BlueprintReadWrite, Category = "Health")
	float Health;

	UPROPERTY(BlueprintReadWrite, Category = "Health")
	float HealthMax;

	UPROPERTY(BlueprintReadWrite, Category = "Health")
	float HealthRegenRate;

	UPROPERTY(BlueprintReadWrite, Category = "Mana")
	float Mana;

	UPROPERTY(BlueprintReadWrite, Category = "Mana")
	float ManaMax;

	UPROPERTY(BlueprintReadWrite, Category = "Mana")
	float ManaRegenRate;

	UPROPERTY(BlueprintReadWrite, Category = "Stamina")
	float Stamina;

	UPROPERTY(BlueprintReadWrite, Category = "Stamina")
	float StaminaMax;

	UPROPERTY(BlueprintReadWrite, Category = "Stamina")
	float StaminaRegenRate;
};
