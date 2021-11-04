// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GHOFloatingStatusBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API UGHOFloatingStatusBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn")
	void SetCharacterName(const FText& InCharacterName);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn")
	void SetHealthPercentage(float InHealthPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn")
	void SetManaPercentage(float InManaPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn")
	void SetShieldPercentage(float InShieldPercentage);
};
