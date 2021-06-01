// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/GHOFloatingDamageTextComponent.h"
#include "GHOPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API AGHOPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGHOPlayerController();

	// AController interface
protected:
	virtual void OnRep_PlayerState() override;
	virtual void OnPossess(APawn* aPawn) override;

	// End of AController interface


public:
	void CreateHUD();
	class UGHOHUDWidget* GetHUD() { return UIHUDWidget; }

//	UFUNCTION(Client, Reliable, WithValidation)
	UFUNCTION(Client, Reliable)
	void RPCClientShowDamageText(float DamageAmount, EGHOFloatingTextType FloatingTextType, class AGHOCharacterBase* TargetCharacter);
	void RPCClientShowDamageText_Implementation(float DamageAmount, EGHOFloatingTextType FloatingTextType, class AGHOCharacterBase* TargetCharacter);
//	bool RPCClientShowDamageText_Validate(float DamageAmount, EGHOFloatingTextType FloatingTextType, class AGHOCharacterBase* TargetCharacter);

	/*
	by GASDocumentation
		Simple way to RPC to the client the countdown until they respawn from the GameMode. 
		Will be latency amount of out sync with the Server.
	和訳
		GameMode から、リスポーンまでのカウントダウンをクライアントに RPC する簡単な方法。
		サーバーとの同期が取れない分、レイテンシーが発生します。
	*/
//	UFUNCTION(Client, Reliable, WithValidation)
	UFUNCTION(Client, Reliable)
	void SetRespawnCountdown(float RespawnTimeRemaining);
	void SetRespawnCountdown_Implementation(float RespawnTimeRemaining);
//	bool SetRespawnCountdown_Validate(float RespawnTimeRemaining);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|UI")
	TSubclassOf<class UGHOHUDWidget> UIHUDWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "GASHandsOn|UI")
	TSubclassOf<class UGHOFloatingDamageTextComponent> UIFloatingDamageTextComponentClass;

private:
	UPROPERTY()
	class UGHOHUDWidget* UIHUDWidget;

};
