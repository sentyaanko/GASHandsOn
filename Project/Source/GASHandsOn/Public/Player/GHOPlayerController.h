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

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|UI")
	TSubclassOf<class UGHOHUDWidget> UIHUDWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "GASHandsOn|UI")
	TSubclassOf<class UGHOFloatingDamageTextComponent> UIFloatingDamageTextComponentClass;

private:
	UPROPERTY()
	class UGHOHUDWidget* UIHUDWidget;

};
