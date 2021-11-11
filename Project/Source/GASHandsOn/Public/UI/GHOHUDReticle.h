// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GHOHUDReticle.generated.h"

/**
by GASShooter
	A reticle on the player's HUD. Can be changed by weapons, abilities, etc.
和訳
	プレイヤーの HUD に表示されるレティクルです。武器やアビリティなどで変化します。
 */
UCLASS()
class GASHANDSON_API UGHOHUDReticle : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	by GASShooter
		Essentially an interface that all child classes in Blueprint will have to fill out
	和訳
		基本的には、 Blueprint の全ての子クラスで実装しなければならないインターフェイスです。
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GASHandsOn")
	void SetColor(FLinearColor Color);
};
