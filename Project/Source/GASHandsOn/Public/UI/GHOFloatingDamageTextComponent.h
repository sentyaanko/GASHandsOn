// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GHOFloatingDamageTextComponent.generated.h"


UENUM(BlueprintType)
enum class EGHOFloatingTextType : uint8
{
	Normal				UMETA(DisplayName = "Normal"),
	Damage				UMETA(DisplayName = "Damage"),
	Critical			UMETA(DisplayName = "Critical")
};

/*
by GASDocumentation
	For the floating Damage Numbers when a Character receives damage.
和訳
	キャラクターがダメージを受けたときの FloatingDamageNumber
*/
UCLASS()
class GASHANDSON_API UGHOFloatingDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn")
	void SetDamage(float Damage, EGHOFloatingTextType FloatingTextType);
};
