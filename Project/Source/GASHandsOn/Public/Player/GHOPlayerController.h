// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASHandsOn|UI")
	TSubclassOf<class UGHOHUDWidget> UIHUDWidgetClass;

private:
	UPROPERTY()
	class UGHOHUDWidget* UIHUDWidget;

};
