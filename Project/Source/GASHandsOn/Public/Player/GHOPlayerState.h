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
	
	// AActor interface
protected:
	/** Overridable native event for when play begins for this actor. */
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// End of AActor interface

	// IAbilitySystemInterface interface
public:
	/** Returns the ability system component to use for this actor. It may live on another actor, such as a Pawn using the PlayerState's component */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// End of IAbilitySystemInterface interface

public:
	class UGHOAttributeSetBase* GetAttributeSetBase()const;


private:
	// Attribute changed callbacks
	void HealthChanged(const struct FOnAttributeChangeData& Data);
	void MaxHealthChanged(const struct FOnAttributeChangeData& Data);

private:
	UPROPERTY()
	class UGHOAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UGHOAttributeSetBase* AttributeSetBase;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;

};
