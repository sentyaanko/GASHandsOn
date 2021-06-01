// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GHOHUDWidgetParameter.h"
#include "GHOHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn")
	void ShowAbilityConfirmCancelText(bool bShowText);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn")
	void SetRespawnCountdown(float RespawnTimeRemaining);

	/**
	* Attribute setters
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn")
	void SetParamter(const struct FGHOHUDWidgetParameter& param);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|HeroLevel")
	void SetHeroLevel(int32 HeroLevel);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|XP")
	void SetXP(int32 XP);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Gold")
	void SetGold(int32 Gold);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|MoveSpeed")
	void SetMoveSpeed(float MoveSpeed);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Health")
	void SetHealth(float Health);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Health")
	void SetHealthMax(float HealthMax);

	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Health")
	//void SetHealthPercentage(float HealthPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Health")
	void SetHealthRegenRate(float HealthRegenRate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Mana")
	void SetMana(float Mana);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Mana")
	void SetManaMax(float ManaMax);

	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Mana")
	//void SetManaPercentage(float ManaPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Mana")
	void SetManaRegenRate(float ManaRegenRate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Stamina")
	void SetStamina(float Stamina);	

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Stamina")
	void SetStaminaMax(float StaminaMax);

	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Stamina")
	//void SetStaminaPercentage(float StaminaPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn|Stamina")
	void SetStaminaRegenRate(float StaminaRegenRate);
};
