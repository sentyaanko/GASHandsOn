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
	
	// AController interface
protected:
	virtual void OnRep_PlayerState() override;
	virtual void OnPossess(APawn* aPawn) override;

	// End of AController interface
};
