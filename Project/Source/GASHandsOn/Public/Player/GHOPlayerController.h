// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "UI/GHOFloatingDamageTextComponent.h"
#include "GHOPlayerController.generated.h"

class UPaperSprite;


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

	/**
	* Weapon HUD info
	*/

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|UI")
	void SetEquippedWeaponPrimaryIconFromSprite(UPaperSprite* InSprite);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|UI")
	void SetEquippedWeaponName(const FText& WeaponName);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|UI")
	void SetEquippedWeaponStatusText(const FText& StatusText);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|UI")
	void SetPrimaryAmmoType(const FGameplayTag& AmmoType);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|UI")
	void SetPrimaryClipAmmo(int32 ClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|UI")
	void SetPrimaryReserveAmmo(int32 ReserveAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|UI")
	void SetSecondaryAmmoType(const FGameplayTag& AmmoType);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|UI")
	void SetSecondaryClipAmmo(int32 SecondaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|UI")
	void SetSecondaryReserveAmmo(int32 SecondaryReserveAmmo);

	UFUNCTION(BlueprintCallable, Category = "GASHandsOn|UI")
	void SetHUDReticle(TSubclassOf<class UGHOHUDReticle> ReticleClass);

	/*
	解説
		WithValidation は UE 4.26 では無くてもコンパイルできるので無くしています。
	*/
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
	/*
	解説
		WithValidation は UE 4.26 では無くてもコンパイルできるので無くしています。
	*/
//	UFUNCTION(Client, Reliable, WithValidation)
	UFUNCTION(Client, Reliable)
	void RPCClientSetRespawnCountdown(float RespawnTimeRemaining);
	void RPCClientSetRespawnCountdown_Implementation(float RespawnTimeRemaining);
//	bool RPCClientSetRespawnCountdown_Validate(float RespawnTimeRemaining);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|UI")
	TSubclassOf<class UGHOHUDWidget> UIHUDWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "GASHandsOn|UI")
	TSubclassOf<class UGHOFloatingDamageTextComponent> UIFloatingDamageTextComponentClass;

private:
	UPROPERTY()
	class UGHOHUDWidget* UIHUDWidget;

};
