// (C) Sentya Anko 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GHOPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GASHANDSON_API AGHOPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGHOPlayerState();

	// override from IAbilitySystemInterface begin
public:
	/** Returns the ability system component to use for this actor. It may live on another actor, such as a Pawn using the PlayerState's component */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// override from IAbilitySystemInterface end

	class UGHOAttributeSetBase* GetAttributeSetBase()const;


private:
	UPROPERTY()
	class UGHOAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UGHOAttributeSetBase* AttributeSetBase;
};
